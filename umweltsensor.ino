/*
  This Sketch helps to build an 
  environment Sensor.
  Using external sensors it is able to read
  - temperatur
  - pressure
  - mox
  
  The circuit:
   5V to Arduino 5V pin
   GND to Arduino GND pin
   CLK to Analog #5
   DAT to Analog #4
*/

// include the library code:
#include "Arduino.h"
#include "Wire.h"
#include "Adafruit_LiquidCrystal.h"
#include <RTClib.h>
#include <Adafruit_MPL115A2.h>
#include "Adafruit_SGP30.h"

// include the SD library:
#include <SPI.h>
#include <SD.h>




Adafruit_MPL115A2 mpl115a2;
RTC_DS1307 rtc;
Adafruit_SGP30 sgp;

// Connect via i2c, default address #0 (A0-A2 not jumpered)
Adafruit_LiquidCrystal lcd(0);

void setup() {
  // set up the LCD's number of rows and columns:
  lcd.begin(20, 4);
  lcd.setBacklight(HIGH);
  // Print a message to the LCD.
  lcd.print("DH7YAX");

  Serial.begin(57600);
  if (! rtc.begin()) {
    Serial.println("Couldn't find RTC");
    while (1);
  } // end rtc.begin()

  if (! rtc.isrunning()) {
    Serial.println("RTC is NOT running!");
    // following line sets the RTC to the date & time this sketch was compiled
    // rtc.adjust(DateTime(F(__DATE__), F(__TIME__)));
    // This line sets the RTC with an explicit date & time, for example to set
    // January 21, 2014 at 3am you would call:
    //rtc.adjust(DateTime(2020, 1, 12, 17, 14, 0));
  }
  mpl115a2.begin();

  if (! sgp.begin()){
    Serial.println("Sensor not found :(");
    while (1);
  }
  
  Serial.print("Initializing SD card...");

  if (!SD.begin(10)) {
    Serial.println("initialization failed!");
    while (1);
  }
  Serial.println("initialization done.");

}

char tempBuffer [8];

void writeRecord(DateTime dt, float pressure, float temp, uint16_t tvoc, uint16_t eco2){
  char myFileName[13];
  char tempString[3];
  itoa(dt.year()-2000, tempString, 10);
  strcpy(myFileName, tempString);
  strcat(myFileName, "-");
  itoa(dt.month(), tempString, 10);
  strcat(myFileName, tempString);
  strcat(myFileName, "-");
  itoa(dt.day(), tempString, 10);
  strcat(myFileName, tempString);
  strcat(myFileName, ".csv");
  File myFile = SD.open(myFileName, FILE_WRITE);
  
  // if the file opened okay, write to it:
  if (myFile) {
    Serial.print("Writing to ");
    // Write Timestamp to Day File
    myFile.print(dt.timestamp());
    myFile.print("|");
    //Write Pressure and Temperatur
    dtostrf(pressure*10,4,1,tempBuffer);
    myFile.print(tempBuffer);
    myFile.print("|");
    dtostrf(temp,2,1,tempBuffer);
    myFile.print(tempBuffer);
    myFile.print("|");
    myFile.print(tvoc);
    myFile.print("|");
    myFile.print(eco2);
    myFile.print("|");
    
    // close the file:
    myFile.close();
    Serial.println(" done.");
  } else {
    // if the file didn't open, print an error:
    Serial.println("error opening File");
  }
}

float correctTemp(float incorrect) {
  // *T = ((float)temp - 498.0F) / -5.35F + 25.0F;  

  float intermediate = ((incorrect - 25.0F) * 5.35F) + 498.0F;
  float corr = (intermediate - 498.0F) / -5.35F + 25.0F;
  return incorrect;
}

void loop() {
  DateTime now = rtc.now();
  float pressureKPA = 0, temperatureC = 0, correctedTemp =0;
  mpl115a2.getPT(&pressureKPA, &temperatureC);
  correctedTemp = correctTemp(temperatureC);
  lcd.setCursor(0, 1);
  lcd.print("Luftdruck :");
  lcd.print(pressureKPA * 10 , 1);
  lcd.print("hPa");

  lcd.setCursor(0, 2);
  lcd.print("Temperatur: ");
  lcd.print(correctedTemp, 1);
  lcd.print(" C");


  lcd.setCursor(0, 3);

  //Serial.println(now.timestamp());
  lcd.print(now.timestamp());

  if (! sgp.IAQmeasure()) {
    Serial.println("Measurement failed");
    return;
  }
  lcd.setCursor(0,0);
  char zero[]="0";
  lcd.print("TVOC");
  if(sgp.TVOC <10) lcd.print(zero); 
  if(sgp.TVOC <100) lcd.print(zero); 
  if(sgp.TVOC <1000) lcd.print(zero); 
  if(sgp.TVOC <10000) lcd.print(zero); 
  lcd.print(sgp.TVOC); 
  lcd.print(" eCO2 "); 
  if(sgp.eCO2 <10) lcd.print(zero);
  if(sgp.eCO2 <100) lcd.print(zero);
  if(sgp.eCO2 <1000) lcd.print(zero);
  if(sgp.eCO2 <10000) lcd.print(zero);
  lcd.print(sgp.eCO2);
  

  writeRecord(now, pressureKPA, correctedTemp, sgp.TVOC, sgp.eCO2);
 
 
  delay(1000);

}
