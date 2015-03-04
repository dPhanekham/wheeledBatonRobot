

/*
 PROJECT: Wheeled Baton Robot
 PROGRAMMER: Derek Phanekham
*/

#include <Servo.h>
 


#define START_CMD_CHAR '*'
#define END_CMD_CHAR '#'
#define MAX_SERVO_CONTROL 179
#define MIN_SERVO_CONTROL 0 
#define MAX_MILLI_WRITE 2000
#define MIN_MILLI_WRITE 700
#define MAX_ANALOGWRITE 255
#define PIN_HIGH 3
#define PIN_LOW 2

Servo esc1;
Servo esc2;
int pin_num;
int pin_value;
int esc1_speed;
int esc2_speed;
int limit;

void setup() {
  Serial.begin(9600);
  Serial.println("Nightmares: Wheeled baton robot 0.5");
  Serial.flush();
  esc1.attach(10);
  esc2.attach(11);
  pin_num = 0;
  pin_value = 100;
  esc1_speed = 100;
  esc2_speed = 100;
  
  limit = 89; //max limit is 89;
  
  int throttle = 89; //this the middle value for the servo library
  esc1.write(throttle);
  esc2.write(throttle);
  esc1.write(throttle);
  esc2.write(throttle);
  esc1.write(throttle);
  esc2.write(throttle);
}

void loop()
{
  if (pin_num == 10) {
    esc1_speed = pin_value;
    int throttle = map(esc1_speed, 0, 200, 89-limit, 89+limit);
    
    Serial.print("pin 10 speed: ");
    Serial.println(throttle);
    
    esc1.write(throttle);
    pin_num = 0;
  }
  else if (pin_num == 11) {
    esc2_speed = pin_value;
    int throttle = map(esc2_speed, 0, 200, 89-limit, 89+limit);
    
    Serial.print("pin 11 speed: ");
    Serial.println(throttle);
    
    esc2.write(throttle);
    pin_num = 0;
  }
  
  //Serial.println();
  //analogWrite(  pin_num, pin_value ); 
  
  
  Serial.flush();


  char get_char = ' ';  //read serial

  // wait for incoming data
  if (Serial.available() < 1) return; // if serial empty, return to loop().

  // parse incoming command start flag 
  get_char = Serial.read();
  if (get_char != START_CMD_CHAR) return; // if no command start flag, return to loop().
  
  // parse incoming pin# and value  
  pin_num = Serial.parseInt(); // read the pin
  pin_value = Serial.parseInt();  // read the value
  
  if(pin_value > 200)
    pin_value = 200;
  else if (pin_value < 0)
    pin_value = 0;

    
   get_char = Serial.read();
  
  
  
  return;  // Done. return to loop();

}


