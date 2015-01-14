#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <math.h>
#include <Wire.h>
#include "rgb_lcd.h"

IoTkit iotkit;
float temperature; // temperature reading; gets updated with latest reading in loop
float humidity;
float setpoint = 30.0; // target temperature; constant
float danger = 35.0;

rgb_lcd lcd;

const int colorR = 120;
const int colorG = 120;
const int colorB = 120;

//variables for peltier heater/cooler
int peltier = 3; //The N-Channel MOSFET is on digital pin 3
int power = 0; //Power level fro 0 to 99%
int peltier_level = map(power, 0, 99, 0, 255); //This is a value from 0 to 255 that actually controls the MOSFET

//variables for temp/humidity sensor
#define DHTPIN A0 // what pin we're connected to
#define DHTTYPE DHT11 // DHT 11
DHT dht(DHTPIN, DHTTYPE);

void setup(){
    Serial.begin(9600);
    iotkit.begin();
    lcd.begin(16, 2);
    lcd.setRGB(colorR, colorG, colorB);
    dht.begin();
    
}

void temp_control(temperature) {
  lcd.print("Setpoint: "); lcd.print(setpoint);
  lcd.setCursor(0, 1);
  lcd.print("Current: "); lcd.print(temperature);
  lcd.setCursor(0, 0);
  
  if(temperature < danger){
    lcd.setRGB(colorR, colorG, colorB);
    lcd.print("Setpoint: "); lcd.print(setpoint);
    lcd.setCursor(0, 1);
  }
  else {
    lcd.setRGB(250, 0, 0);
    lcd.print("DANGER DANGER!!!");
    lcd.setCursor(0, 1);
  }
}

void heat_control(diff) {

	// this peltier moves heat into the controlled system so we turn it on if temp is too low
  if(diff < 0) {
    power = 100 * -diff / 2.2; // max power half a degree away from setpoint; else proportional to difference
  } else {
    power = 0; // don't turn on if already hotter than setpoint
  }
  if(power > 99) power = 99; if(power < 0) power = 0; // make sure to stay within valid range
  peltier_level = map(power, 0, 99, 0, 255);
  Serial.print("Power=");
  Serial.print(power);
  Serial.print(" PLevel=");
  Serial.println(peltier_level);

  analogWrite(peltier, peltier_level); //Write this new value out to the port
}

void loop(){
  delay(500);
  humidity = dht.readHumidity();
  temperature = dht.readTemperature();
  float diff = temperature - setpoint;
  iotkit.send("temp", temperature);

  Serial.print("Current temperature/humidity is "); Serial.println(temperature);
  Serial.println("Setpoint is "); Serial.println(setpoint);
  Serial.println("Difference is "); Serial.println(diff);

  temp_control(temperature);
  heat_control(diff);
  
}
