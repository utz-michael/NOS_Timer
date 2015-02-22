#include <EEPROM.h>
#include <SPI.h> 
#include <LiquidCrystal.h>

LiquidCrystal lcd(8, 9, 4, 5, 6, 7);

// constants won't change. They're used here to 
// set pin numbers:
const int TransbrakePIN = 2;     // the number of the pushbutton pin Transbrake Button
//const int ledPin1 =  13;      // the number of the LED pin Tranbrake
const int RevoPIN =  3;      // the number of the LED pin NOS



// variables will change:
int keyPress;                  // LCD Button
int buttonState = LOW;         // variable for reading the pushbutton status
int test;
        //


int ledState = LOW;             // ledState used to set the LED
int n = 0;                    // indikator ob delay durchgelaufen
long previousMillis = 0;        // will store last time LED was updated
long interval = 7000;           // LCD blink geschwindigkeit

volatile int nosactive = 0; // nos status fÃ¼r interupt

unsigned long lastDelay = 0;
unsigned long lastNOS = 0;
unsigned long vNOS = 0;

unsigned long mDelay;
unsigned long vDelay;
unsigned long NOS = 1000;    // default wert  
unsigned long Delay = 500;  // default wert
int x = 0;

float RetardCurve = 1.5 ; //Retard kurve 1.0 bis 3.0 per 50 ps NOS
float Retard = 1.5 ; // aktuell gesetztes retard
int retard;
int RetardEingang;
int WiederstandRAW;
int WiederstandRAW_Old = 0;
int MaxHP =600; // Maximale Leistung bei eingang 5V muss auf den flow angepasst werden 
// Rolling average
#define filterSamples   9              // filterSamples should  be an odd number, no smaller than 3
int sensSmoothArray1 [filterSamples];   // array for holding raw sensor values for sensor1 

// spi port für wiederstand
const int csPin = 10;


void setup() {
  // initialize the LED pin as an output:
Serial.begin(9600);
//  pinMode(ledPin1, OUTPUT);     // Transbarke Indikator nicht genutzt
  pinMode(RevoPIN, OUTPUT);    // Nos Aktiv
  
  
  // initialize the pushbutton pin as an input:
  pinMode(TransbrakePIN, INPUT);     //transbrake
  
 //NOS ausgang ausschalten
  digitalWrite(RevoPIN, HIGH); 
  







}
void loop(){
   
 
       lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("delay:");
    lcd.setCursor(7, 0);
    lcd.print(Delay);
    lcd.setCursor(11, 0);
    lcd.print("ms");
    lcd.setCursor(0, 1);
  
 retardFormel ();
 
}

void retardFormel(){

// Retart steuerung
 
  RetardEingang = digitalSmooth(analogRead(5), sensSmoothArray1) ; // einlesen und Filtern der Analogen Spannung vom REVO Controller
   //----- Berechnung des Wiederstandes und des Retards------------------------------------------------
 
   
  Retard =  ((MaxHP/(1023.0/RetardEingang)) /50.0)*RetardCurve;
  
  WiederstandRAW = Retard *1000.0 /48.828125 ;
  
  Serial.print("AnalogIn ");
  Serial.print(RetardEingang);   
  Serial.print(" Spannung ");
  Serial.print(RetardEingang*0.0049);   
  Serial.print(" retard raw ");
....
  
  //if ( (WiederstandRAW_Old - WiederstandRAW) >= 2 || (WiederstandRAW -WiederstandRAW_Old) >= 2) {
 // digitalPotWrite(0,WiederstandRAW); // Wiederstandswert setzen  48.82 Ohm pro einheit 
 // }
 // WiederstandRAW_Old = WiederstandRAW;

 return;
 //-------------------------------------------------------------------------------------------------
 }
