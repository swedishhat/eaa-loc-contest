/*
===================================
== SAVANNAH EAA SICK SHAKER DEMO ==
==     Code By Patrick Lloyd     ==
===================================

Version History:
  * 0.1: Preliminary breadboard example

TODO:
  * Add EEPROM support for saving set points
  * Add CALL/ACK routine to communicate w/
    external process.
*/
#include "Maxbotix.h"

// Program constants
#define SW_VER  0.1
#define RANGE_R 5
#define RANGE_L 6
#define MOTOR   8
#define USER_SW 9

// Create Sonar objects
Maxbotix rangeSensorPW_R(RANGE_R, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);
Maxbotix rangeSensorPW_L(RANGE_L, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);

float min_dist   = 0.0;
float max_dist   = 0.0;
float total_dist = 0.0;
float curr_meas  = 0.0;


/*
print_data()
===========================
Print range data to serial port. Format for parsing by external
process such as PySerial or ESP8266 / NodeMCU
*/
void print_data()
{
  Serial.print("L");
  Serial.print(rangeSensorPW_L.getRange());
  Serial.println("l");
  
  Serial.print("R");
  Serial.print(rangeSensorPW_R.getRange());
  Serial.println('r');
}


/*
set_dist()
===========================
Read and average distance measurements
*/
float set_dist()
{
  float avg = 0.0;
  int samples = 32;
  while(digitalRead(USER_SW) == HIGH){}
  Serial.println("Low detected. 500 ms debounce.");
  delay(500);
  
  for(int i = 0; i < samples; i++)
  {
    avg += (rangeSensorPW_L.getRange() + rangeSensorPW_R.getRange()) / 2;
    Serial.print(i);
    Serial.print(": ");
    Serial.println(avg);
  }

  while(digitalRead(USER_SW) == LOW){}
  Serial.println("High detected. 500 ms debounce.");
  delay(500);
  
  return (avg / samples);  
}

void setup()
{
  // Serial Initialization and "welcome" message
  Serial.begin(115200);
  Serial.println("===================================");
  Serial.println("== SAVANNAH EAA SICK SHAKER DEMO ==");
  Serial.println("==     Code By Patrick Lloyd     ==");
  Serial.print(  "==         Version: ");
  Serial.print(SW_VER);
  Serial.println(                        "         ==");
  Serial.println("===================================\n");

  // Set up GPIO
  pinMode(USER_SW, INPUT_PULLUP);
  pinMode(RANGE_R, INPUT);
  pinMode(RANGE_L, INPUT);
  pinMode(MOTOR,   OUTPUT);
}

void loop()
{
  if(min_dist == 0.0 || max_dist == 0.0)
  {
    Serial.println("No valid distance presets detected.");
    
    // Set Full Forward
    Serial.println("Set minimum distance.....");
    min_dist = set_dist();
    Serial.print("MIN: ");
    Serial.println(min_dist);
    Serial.println();
    
    // Set Full Aft
    Serial.println("Set maximum distance.....");
    max_dist = set_dist();
    Serial.print("MAX: ");
    Serial.println(max_dist);
    Serial.println();
    
    // Calculate total distance
    total_dist = max_dist - min_dist;
    Serial.print("Dist: ");
    Serial.println(total_dist);
    Serial.println();
  }

  // Average both readings and load into measurement variable
  curr_meas = (rangeSensorPW_L.getRange() + rangeSensorPW_R.getRange()) / 2;

  // Compare current range measurement against 90% of full range.
  // If >90% shake the stick, otherwise, leave it off...
  if(curr_meas >= (total_dist * 0.9)) digitalWrite(MOTOR, HIGH);
  else digitalWrite(MOTOR, LOW);
}


