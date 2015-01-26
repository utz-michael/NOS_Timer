#include <EEPROM.h>

#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// constants won't change. They're used here to 
// set pin numbers:
const int TransbrakePIN = 2;     // the number of the pushbutton pin Transbrake Button
//const int ledPin1 =  13;      // the number of the LED pin Tranbrake
const int NOSFoggerPIN =  12;      // the number of the LED pin NOS



// variables will change:
int keyPress;                  // LCD Button
int buttonState = LOW;         // variable for reading the pushbutton status
        //


int ledState = LOW;             // ledState used to set the LED
int n = 0;                    // indikator ob delay durchgelaufen
long previousMillis = 0;        // will store last time LED was updated
long interval = 3000;           // LCD blink geschwindigkeit

volatile int nosactive = 0; // nos status fÃ¼r interupt

unsigned long lastDelay = 0;
unsigned long lastNOS = 0;
unsigned long vNOS = 0;

unsigned long mDelay;
unsigned long vDelay;
unsigned long NOS = 1000;    // default wert  
unsigned long Delay = 500;  // default wert
int x = 0;

float RetardCourve = 1.5 ; //Retard kurve 1.0 bis 3.0 per 50 ps NOS
float Retard = 1.5 ; // aktuell gesetztes retard
int retard;
void setup() {
  // initialize the LED pin as an output:

//  pinMode(ledPin1, OUTPUT);     // Transbarke Indikator nicht genutzt
  pinMode(NOSFoggerPIN, OUTPUT);    // Nos Aktiv

  
  // initialize the pushbutton pin as an input:
  pinMode(TransbrakePIN, INPUT);     //transbrake
  
 //NOS ausgang ausschalten
  digitalWrite(NOSFoggerPIN, LOW); 

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

}
void loop(){
   
   unsigned long currentMillis = millis();
  
   if(currentMillis - previousMillis > interval) {
     // save the last time you blinked the LCD 
     previousMillis = currentMillis;   

     // if the LED is off turn it on and vice-versa:
     if (ledState == LOW){
       ledState = HIGH;
    lcd.setCursor(0, 0);
    lcd.print("delay:");
    lcd.setCursor(7, 0);
    lcd.print(Delay);
    lcd.setCursor(11, 0);
    lcd.print("ms");
    lcd.setCursor(0, 1);
    keyPress = analogRead(0); 
    lcd.print("duration:");
    lcd.setCursor(11, 1);
    lcd.print(NOS);
    lcd.setCursor(15, 1);
    lcd.print("ms");
     }
     else
     {
       ledState = LOW;
    lcd.setCursor(0, 0);
    keyPress = analogRead(0); 
    lcd.print("Retard per 50HP");
    lcd.setCursor(7, 1);
    lcd.print(RetardCourve);
     }
 
   }
   
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
   lcd.print("delay:");
   lcd.setCursor(7, 0);
    lcd.print(vDelay / 1000); // ausgabe zeit 
    lcd.setCursor(11, 0);
    lcd.print("ms");
   lcd.setCursor(0, 12);
   lcd.print("Retard:");
   lcd.setCursor(9, 1);
    lcd.print(Retard);
    lcd.setCursor(1, 12);
    lcd.print("°");
          
 mDelay = micros();           // MicrosekundenzÃ¤hler auslesen
 vDelay = mDelay - lastDelay;  // Differenz zum letzten Durchlauf berechnen
  
   if (vDelay > Delay * 1000 && n == 0) { 
    digitalWrite(NOSFoggerPIN, HIGH);
 
 
lastNOS = mDelay ;
 n = 1;
  }
   vNOS = mDelay - lastNOS;
   
  if (vNOS > NOS * 1000 && n == 1){ 
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
  delay (500); //Blocken nach run     
  lcd.clear();
        
      
      


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
      lcd.print("Setup Delay    ");
      lcd.setCursor(0, 1);
      lcd.print(Delay);
      lcd.print("ms        ");
      
      keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        Delay = Delay + 100;
        if (Delay >= 10000){Delay = 10000;}
     // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
    }
      
         // down
  if(keyPress < 395 && keyPress > 230 ){
        Delay = Delay - 100;
        if (Delay <= 0){Delay = 0;}
    if (Delay >= 10000){Delay = 0;  
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
      lcd.print("Setup NOS length");
      lcd.setCursor(0, 1);
      lcd.print(NOS);
      lcd.print("ms        ");
    
      
     keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        NOS = NOS + 100;
        if (NOS >= 20000){NOS = 20000;}
     
     
       // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
      }
     
         // down
    if(keyPress < 395 && keyPress > 230 ){
        NOS = NOS - 100;
        if (NOS <= 0){NOS = 0;}
      if (NOS >= 20000){NOS = 0;} 
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


      // Retard einstellen ------------------------------------------------------------------------------
      lcd.clear();
       do
{
      lcd.setCursor(0, 0);
      lcd.print("Retard per 50 HP");
      lcd.setCursor(0, 1);
      lcd.print(Retard);
      lcd.print("°");
    
      
     keyPress = analogRead(0);
      // up
   if(keyPress < 221 && keyPress > 66 ){
        Retard = Retard + 0.10;
        if (Retard >= 3.00){Retard = 3.00;}
     
     
       // Teste entprellen
     do {
     keyPress = analogRead(0); 
  }while (keyPress < 900 ); 
  //------------------------------ 
      }
     
         // down
    if(keyPress < 395 && keyPress > 230 ){
        Retard = Retard - 0.1;
        if (Retard <= 1.0){Retard = 1.0;}
      if (Retard >= 3.0){Retard = 1.0;} 
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
 
     
    } 

  
}
void writemem () {
  retard = Retard * 10;
// integer in byte umwandeln
  byte firstByte = byte(Delay >> 8);
  byte secondByte = byte(Delay & 0x00FF);
  byte thirdByte = byte(NOS >> 8);
  byte forthByte = byte(NOS & 0x00FF);
  byte fifthByte = byte(retard >> 8);
  byte sixthByte = byte(retard & 0x00FF);
 
    //eeprom schreiben
      EEPROM.write(0, firstByte);
      EEPROM.write(1, secondByte);
      EEPROM.write(2, thirdByte);
      EEPROM.write(3, forthByte);
      EEPROM.write(4, fifthByte);
      EEPROM.write(5, sixthByte);
   
      return;
}

void readmem () {

 // eeprom lesen   
   byte  firstByte = EEPROM.read(0);
   byte  secondByte = EEPROM.read(1);
   byte  thirdByte = EEPROM.read(2);
   byte  forthByte = EEPROM.read(3);
   byte  fifthByte = EEPROM.read(4);
   byte  sixthByte = EEPROM.read(5);
 
  //byte in integer wandeln   
   Delay = int(firstByte << 8) + int(secondByte);
   NOS = int(thirdByte << 8) + int(forthByte);
   retard = int(fifthByte << 8) + int (sixthByte);
    Retard = retard / 10;
return;
}


 

