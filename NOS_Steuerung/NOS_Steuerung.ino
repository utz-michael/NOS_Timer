#include <EEPROM.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// constants won't change. They're used here to 
// set pin numbers:
const int TransbrakePIN = 2;     // the number of the pushbutton pin Transbrake Button
//const int ledPin1 =  13;      // the number of the LED pin Tranbrake
const int NOSFoggerPIN =  12;      // the number of the LED pin NOS
const int NOSPlatePIN =  13;


// variables will change:
int keyPress;                  // LCD Button
int buttonState = LOW;         // variable for reading the pushbutton status
        //


int ledState = LOW;             // ledState used to set the LED
int n = 0;                    // indikator ob Delay_Fogger durchgelaufen
long previousMillis = 0;        // will store last time LED was updated
long interval = 200;           // LCD blink geschwindigkeit

volatile int nosactive = 0; // nos status fÃ¼r interupt

unsigned long lastDelay = 0;
unsigned long lastNOS = 0;
unsigned long vNOS = 0;

unsigned long mDelay;
unsigned long vDelay;
unsigned long NOS_Fogger = 1000;    // default wert  
unsigned long Delay_Fogger = 300;  // default wert
unsigned long NOS_Plate = 1000;    // default wert  
unsigned long Delay_Plate = 100;  // default wert
int x = 0;

void setup() {
  // initialize the LED pin as an output:

//  pinMode(ledPin1, OUTPUT);     // Transbarke Indikator nicht genutzt
  pinMode(NOSFoggerPIN, OUTPUT);    // Nos Fogger output
  pinMode(NOSPlatePIN, OUTPUT);    // Nos Plate output

  
  // initialize the pushbutton pin as an input:
  pinMode(TransbrakePIN, INPUT);     //transbrake
  
 //NOS ausgang ausschalten
  digitalWrite(NOSFoggerPIN, LOW); 
  digitalWrite(NOSPlatePIN, LOW); 

 // set up the LCD's number of columns and rows: 
  lcd.begin(16, 2);
   lcd.setCursor(0, 0);
    lcd.print("    NOS delay   ");
    lcd.setCursor(0, 1);
    lcd.print("   Controller   ");
  delay(5000);
 
   buttonState = digitalRead(TransbrakePIN); // abfrage beim einschalten 

// default werte herstellen beim einschalten

   if (buttonState == HIGH) {    
     
   
  
    writemem ();  // default werte in eeprom speichern
       
 lcd.setCursor(0, 0);
  lcd.print("Reset Default   ");  
  delay(5000);
 
  do
    {
     buttonState = digitalRead(TransbrakePIN);
     lcd.setCursor(0, 0);
     lcd.print("Release Button  "); 
    } 
    while (buttonState == HIGH);
}
  lcd.clear();

   readmem (); // eeprom lesen
 lcd.setCursor(0, 0);
delay(1000); 

 
nosactive = 0; // nos sicher deaktivieren
anzeige (); // Aufruf anzeige
}
void loop(){
  /* 
   unsigned long currentMillis = millis();
  
   if(currentMillis - previousMillis > interval) {
     // save the last time you blinked the LCD 
     previousMillis = currentMillis;    
        lcd.scrollDisplayLeft();
    
   }
 */   
 
   
   
    buttonState = digitalRead(TransbrakePIN); // abfrage transbrake
    if ( buttonState == HIGH ) {
      //digitalWrite(ledPin1, HIGH);
  
     lcd.setCursor(0, 0);
        lcd.print("   Transbrake   ");
        lcd.setCursor(0, 1);
        lcd.print("     active     ");
}
   
   
    else
  {
  //digitalWrite(ledPin1, LOW);
}
    keyPress = analogRead(0); 
if (keyPress < 600 && keyPress > 400 ) {nosactive = 1;}
  
 buttonState = digitalRead(TransbrakePIN); // abfrage ob transbrake gedrückt


// Abfrage der steigenden flanke des Transbrake Buttons
   if (buttonState == HIGH && x==0 ) {  
     delay (1000);
     x = 1; // steigende Flanke dedektiert
     nosactive = 0; // nos timer sicher ausgeschaltet
     }
     
// Abfrage der fallenden Flanke des Transbrake Buttons     
if (buttonState == LOW && x==1 ) { 
  nosactive = 1; // nos timer einschalten
  x=0; //Flanken dedektierung zurücksetzen
  }
  

 //nos starten ------------------------------------------------------------------------------------------------------------------------------------------------
 if (nosactive == 1) {
   mDelay = micros(); // zeit speichern am anfang der warteschleife
   lastDelay = mDelay;
   lcd.clear();
   do {
 
    
    
      lcd.setCursor(0, 0);
        lcd.print("   Go Baby Go   ");
      lcd.setCursor(0, 1);
 
   lcd.print(vDelay / 1000); // ausgabe zeit 
   lcd.print("ms");    // einschalt verzÃ¶gerung 
        
 mDelay = micros();                 // MicrosekundenzÃ¤hler auslesen
 vDelay = mDelay - lastDelay;  // Differenz zum letzten Durchlauf berechnen
  
   if (vDelay > Delay_Fogger * 1000 && n == 0) { 
    digitalWrite(NOSFoggerPIN, HIGH);
 
 
lastNOS = mDelay ;
 n = 1;
  }
   vNOS = mDelay - lastNOS;
   
  if (vNOS > NOS_Fogger * 1000 && n == 1){ 
    digitalWrite(NOSFoggerPIN, LOW);  // nos dauer
    nosactive = 0;
     n = 0 ;
     
      }
 buttonState = digitalRead(TransbrakePIN); // abfrage während des laufes
   
if (buttonState == HIGH && x==0 ) { // abbruch kriterium und neustart
  digitalWrite(NOSFoggerPIN, LOW);
  nosactive = 0;
  n=0;
  x=1;
 }
 }
     while (nosactive == 1);   
  delay (2000); //Blocken nach run     
  lcd.clear();
  anzeige (); // Aufruf anzeige      
      
      


 }
   else {nosactive = 0;}
nosactive = 0;
 // display button abfrage setup routine einschalten ------------------------------------------------
 
   keyPress = analogRead(0); 


 if(keyPress < 873 && keyPress > 603){
     
  
   
  do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 );
    lcd.clear();
   // setup start delay  ------------------------------------------------------------------------------------
      do
{
      lcd.setCursor(0, 0);
      lcd.print("SetupFoggerDelay");
      lcd.setCursor(0, 1);
      lcd.print(Delay_Fogger);
      lcd.print("ms        ");
      
      keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        Delay_Fogger = Delay_Fogger + 100;
        if (Delay_Fogger >= 10000){Delay_Fogger = 10000;}
     // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
    }
      
         // down
  if(keyPress < 395 && keyPress > 230 ){
        Delay_Fogger = Delay_Fogger - 100;
        if (Delay_Fogger <= 0){Delay_Fogger = 0;}
    if (Delay_Fogger >= 10000){Delay_Fogger = 0;  
    }
  // Teste entprellen
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------
    }
     
      
      
      } while (keyPress > 50 ); // rechte taste abfragen
   // Teste entprellen    
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 );
  
   //------------------------------
      
      // Nos dauer einstellen ------------------------------------------------------------------------------
      lcd.clear();
       do
{
      lcd.setCursor(0, 0);
      lcd.print("SetupFoggerlengt");
      lcd.setCursor(0, 1);
      lcd.print(NOS_Fogger);
      lcd.print("ms        ");
    
      
     keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        NOS_Fogger = NOS_Fogger + 100;
        if (NOS_Fogger >= 20000){NOS_Fogger = 20000;}
     
     
       // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
      }
     
         // down
    if(keyPress < 395 && keyPress > 230 ){
        NOS_Fogger = NOS_Fogger - 100;
        if (NOS_Fogger <= 0){NOS_Fogger = 0;}
      if (NOS_Fogger >= 20000){NOS_Fogger = 0;} 
   // Teste entprellen
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------
    }
     
      
      
      } while (keyPress > 50 ); // rechte taste abfragen
   // Teste entprellen    
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 );
  
   //------------------------------
       
//start Plate setup       
   lcd.clear();
   // setup start delay  ------------------------------------------------------------------------------------
      do
{
      lcd.setCursor(0, 0);
      lcd.print("SetupPlateDelay");
      lcd.setCursor(0, 1);
      lcd.print(Delay_Plate);
      lcd.print("ms        ");
      
      keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        Delay_Plate = Delay_Plate + 100;
        if (Delay_Plate >= 10000){Delay_Plate = 10000;}
     // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
    }
      
         // down
  if(keyPress < 395 && keyPress > 230 ){
        Delay_Plate = Delay_Plate - 100;
        if (Delay_Plate <= 0){Delay_Plate = 0;}
    if (Delay_Plate >= 10000){Delay_Plate = 0;  
    }
  // Teste entprellen
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------
    }
     
      
      
      } while (keyPress > 50 ); // rechte taste abfragen
   // Teste entprellen    
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 );
  
   //------------------------------
      
      // Nos dauer einstellen ------------------------------------------------------------------------------
      lcd.clear();
       do
{
      lcd.setCursor(0, 0);
      lcd.print("SetupPlatelengt");
      lcd.setCursor(0, 1);
      lcd.print(NOS_Plate);
      lcd.print("ms        ");
    
      
     keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        NOS_Plate = NOS_Plate + 100;
        if (NOS_Plate >= 20000){NOS_Plate = 20000;}
     
     
       // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
      }
     
         // down
    if(keyPress < 395 && keyPress > 230 ){
        NOS_Plate = NOS_Plate - 100;
        if (NOS_Plate <= 0){NOS_Plate = 0;}
      if (NOS_Plate >= 20000){NOS_Plate = 0;} 
   // Teste entprellen
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------
    }
     
      
      
      } while (keyPress > 50 ); // rechte taste abfragen
   // Teste entprellen    
       do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 );
  
   //------------------------------
      
  lcd.clear();
   writemem ();   // daten speichern
 anzeige (); // Aufruf anzeige
     
    } 

  
}
void writemem () {
  
// integer in byte umwandeln
  byte firstByte = byte(Delay_Fogger >> 8);
  byte secondByte = byte(Delay_Fogger & 0x00FF);
  byte thirdByte = byte(NOS_Fogger >> 8);
  byte forthByte = byte(NOS_Fogger & 0x00FF);
  byte fifthByte = byte(Delay_Plate >> 8);
  byte sixthByte = byte(Delay_Plate & 0x00FF);
  byte seventhByte = byte(NOS_Plate >> 8);
  byte eightByte = byte(NOS_Plate & 0x00FF);
 
    //eeprom schreiben
      EEPROM.write(0, firstByte);
      EEPROM.write(1, secondByte);
      EEPROM.write(2, thirdByte);
      EEPROM.write(3, forthByte);
      EEPROM.write(4, fifthByte);
      EEPROM.write(5, sixthByte);
      EEPROM.write(6, seventhByte);
      EEPROM.write(7, eightByte);
   
      return;
}

void readmem () {

 // eeprom lesen   
   byte  firstByte = EEPROM.read(0);
   byte  secondByte = EEPROM.read(1);
   byte  thirdByte = EEPROM.read(2);
   byte  forthByte = EEPROM.read(3);
   byte fifthByte = EEPROM.read(4);
   byte sixthByte = EEPROM.read(5);
   byte seventhByte = EEPROM.read(6);
   byte eightByte = EEPROM.read(7);
 
  //byte in integer wandeln   
   Delay_Fogger = int(firstByte << 8) + int(secondByte);
   NOS_Fogger = int(thirdByte << 8) + int(forthByte);
  Delay_Plate = int(fifthByte << 8) + int(sixthByte);
   NOS_Plate = int(seventhByte << 8) + int(eightByte);
return;
}

void anzeige () {
  lcd.setCursor(0, 0);
 /*
     lcd.print("Fogger delay/duration: ");
    lcd.print(Delay_Fogger);
    lcd.print(" / ");
    keyPress = analogRead(0); 
    lcd.print(NOS_Fogger);    
    lcd.print("ms ");
    lcd.setCursor(0, 1);
    lcd.print("Plate  delay/duration: ");
    lcd.print(Delay_Plate);
    lcd.print(" / ");
    keyPress = analogRead(0); 
    lcd.print(NOS_Plate);    
    lcd.print("ms ");
   */ 
       lcd.print("Fogger:");
    lcd.print(Delay_Fogger);
    lcd.print("/");
    keyPress = analogRead(0); 
    lcd.print(NOS_Fogger);    
    lcd.print("ms");
    lcd.setCursor(0, 1);
    lcd.print("Plate: ");
    lcd.print(Delay_Plate);
    lcd.print("/");
    keyPress = analogRead(0); 
    lcd.print(NOS_Plate);    
    lcd.print("ms");
    return;
    }
 

