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
void set_dist()
{
  float avg = 0.0;
  int samples = 16;
   
  // Set stick full forward
  Serial.println("Set forward distance.....");
  
  // Debounce on the button press
  while(digitalRead(USER_SW) == HIGH){}
  Serial.println("Low detected. 500 ms debounce.");
  delay(500);

  // Iterate through 32 measurements from sonar and average them
  for(int i = 0; i < samples; i++)
  {
    avg += (rangeSensorPW_L.getRange() + rangeSensorPW_R.getRange()) / 2;
    Serial.print(i);
    Serial.print(": ");
    Serial.println(avg);
  }

  // Debounce on the button release
  while(digitalRead(USER_SW) == LOW){}
  Serial.println("High detected. 500 ms debounce.");
  delay(500);

  // Calculate minimum distance
  min_dist = (avg / samples);
  Serial.print("MIN: ");
  Serial.println(min_dist);
  Serial.println();
  
  // Set stick full aft and record measurement
  Serial.println("Set aft distance.....");
  
  // Debounce on the button press
  while(digitalRead(USER_SW) == HIGH){}
  Serial.println("Low detected. 500 ms debounce.");
  delay(500);

  // Reset the average
  avg = 0.0;
  
  // Iterate through 32 measurements from sonar and average them
  for(int i = 0; i < samples; i++)
  {
    avg += (rangeSensorPW_L.getRange() + rangeSensorPW_R.getRange()) / 2;
    Serial.print(i);
    Serial.print(": ");
    Serial.println(avg);
  }

  // Debounce on the button release
  while(digitalRead(USER_SW) == LOW){}
  Serial.println("High detected. 500 ms debounce.");
  delay(500);
  
  max_dist = (avg / samples);
  Serial.print("MAX: ");
  Serial.println(max_dist);
  Serial.println();
  
  // Calculate total distance
  total_dist = max_dist - min_dist;
  Serial.print("Dist: ");
  Serial.println(total_dist);
  Serial.println();    
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
  digitalWrite(MOTOR, LOW);
}

void loop()
{
  if(min_dist == 0.0 || max_dist == 0.0)
  {
    Serial.println("No valid distance presets detected.");
    set_dist();
  }

  // Average both readings and load into measurement variable
  curr_meas = (rangeSensorPW_L.getRange() + rangeSensorPW_R.getRange()) / 2;
  Serial.print(curr_meas);
  Serial.print(" / ");
  Serial.println(total_dist);
  
  // Compare current range measurement against 90% of full range.
  // If >90%, shake the stick; otherwise, leave it off...
  if((max_dist - curr_meas) <= (total_dist * 0.1))
  {
    Serial.println("<< STALL WARNING >>");
    digitalWrite(MOTOR, HIGH);
  }
  else
  {
    Serial.println(">> You're Fine...<<");
    digitalWrite(MOTOR, LOW);
  }
}


