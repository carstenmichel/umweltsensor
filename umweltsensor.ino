/*
  Demonstration sketch for Adafruit i2c/SPI LCD backpack
  using MCP23008 I2C expander
  ( http://www.ladyada.net/products/i2cspilcdbackpack/index.html )

  This sketch prints "Hello World!" to the LCD
  and shows the time.

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

Adafruit_MPL115A2 mpl115a2;
RTC_DS1307 rtc;

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
}

void loop() {
  DateTime now = rtc.now();
  float pressureKPA = 0, temperatureC = 0;
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):

  // Get Temperatur and Pressure
  mpl115a2.getPT(&pressureKPA, &temperatureC);
  lcd.setCursor(0, 1);
  lcd.print("Luftdruck :");
  lcd.print(pressureKPA * 10 , 1);
  lcd.print("hPa");

  lcd.setCursor(0, 2);
  lcd.print("Temperatur: ");
  lcd.print(temperatureC, 1);
  lcd.print(" C");


  lcd.setCursor(0, 3);

  Serial.println(now.timestamp());
  lcd.print(now.timestamp());

  delay(1000);

}
