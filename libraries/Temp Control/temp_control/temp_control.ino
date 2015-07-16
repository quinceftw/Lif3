/*
/* Grove - Temprature Sensor demo v1.0
 *  This sensor detects the enviroment temprature,
 *  Connect the signal of this sensor to A0, use the 
 *  Serial monitor to get the result.
 *  By: http://www.seeedstudio.com
 */
#include <IoTkit.h>    // include IoTkit.h to use the Intel IoT Kit
#include <Ethernet.h>  // must be included to use IoTkit
#include <aJSON.h>
#include <math.h>
#include <Wire.h>
#include "rgb_lcd.h"


IoTkit iotkit;
int a;
float temperature; // temperature reading; gets updated with latest reading in loop
float setpoint = 30.0; // target temperature; constant
float danger = 35.0;
int B=3975;                  //B value of the thermistor, constant
float resistance; // Resistance of the thermistor; gets updated with latest reading in loop

rgb_lcd lcd;

const int colorR = 120;
const int colorG = 120;
const int colorB = 120;

//code for heater
int peltier = 3; //The N-Channel MOSFET is on digital pin 3
int power = 0; //Power level fro 0 to 99%
int peltier_level = map(power, 0, 99, 0, 255); //This is a value from 0 to 255 that actually controls the MOSFET

void setup(){
    Serial.begin(9600);
    iotkit.begin();
  
    lcd.begin(16, 2);
    
    lcd.setRGB(colorR, colorG, colorB);
    
    // Print a message to the LCD.
 

  //pinMode(peltier, OUTPUT);
}

void loop(){
  
  a=analogRead(0);
  resistance=(float)(1023-a)*10000/a; //get the resistance of the sensor;
  temperature=1/(log(resistance/10000)/B+1/298.15)-273.15;//convert to temperature via datasheet ;

  delay(500);
  Serial.print("Current temperature is "); Serial.println(temperature);
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
    
  
  Serial.println("Setpoint is "); Serial.println(setpoint);
  
  float diff = temperature - setpoint;
  Serial.println("Difference is "); Serial.println(diff);
  iotkit.send("temp", temperature);

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









