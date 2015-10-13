/*
  Maxbotix advanced test

  Instructions:
  - Same as simple test

  Filters:
  * NONE (default): No filtering
  * MEDIAN: Take the median of a sample
  * HIGHEST_MODE: Take the mode of a sample. In case more than
  one mode, the highest one is returned
  * LOWEST_MODE: Take the mode of a sample. In case more than
  one mode, the lowest one is returned
  * BEST: Take the mode of a sample. In case more than one
  mode is found, the median is returned
  * SIMPLE: Continue reading until sample_size consecutive readings
  are issued

*/
#include "Maxbotix.h"

#define SW_VER  0.1
#define USER_SW 9


Maxbotix rangeSensorPW_L(6, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);
Maxbotix rangeSensorPW_R(5, Maxbotix::PW, Maxbotix::LV, Maxbotix::BEST);

float min_dist = 0.0;
float max_dist = 0.0;


void print_data()
{
  Serial.print("L");
  Serial.print(rangeSensorPW_L.getRange());
  Serial.print('\n');
  
  Serial.print("R");
  Serial.print(rangeSensorPW_R.getRange());
  Serial.print('\n');
}

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
  Serial.begin(115200);
  pinMode(USER_SW, INPUT_PULLUP);

  Serial.println("===================================");
  Serial.println("== SAVANNAH EAA SICK SHAKER DEMO ==");
  Serial.println("==     Code By Patrick Lloyd     ==");
  Serial.print(  "==         Version: ");
  Serial.print(SW_VER);
  Serial.println(                        "         ==");
  Serial.println("===================================\n");
    
}

void loop()
{
  if(min_dist == 0.0 || max_dist == 0.0){
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
   
    Serial.print("Dist: ");
    Serial.println(max_dist - min_dist);
    Serial.println();
  }
  
  //{
    
  //}
  
  //i = analogRead(A0);
  //val=(6762/(i-9))-4;

  //Serial.print("IR: ");
  //Serial.println(rangeSensorPW_LED.getRange());
  
  // PW
  
  
}


