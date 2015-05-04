

/*
 PROJECT: Wheeled Baton Robot
 VERSION: 1.0
 VERSION_NAME: Thousands of fleeing souls
 PROGRAMMER: Derek Phanekham
*/

#include <Servo.h>
#include <Wire.h> 
#include <Adafruit_L3GD20.h>
 


#define START_CMD_CHAR '*'
#define END_CMD_CHAR '#'
#define MAX_SERVO_CONTROL 179
#define MIN_SERVO_CONTROL 0 
#define MAX_MILLI_WRITE 2000
#define MIN_MILLI_WRITE 700
#define MAX_ANALOGWRITE 255

//#define DEBUG


//actual ESCs to send values to
Servo esc1;
Servo esc2;

//pin number from serial
int pin_num;
//pin value from serial
int pin_value;

//speed for ESCs
int esc1_speed;
int esc2_speed;

//limiter on speed
int limit;


//parameters for accelerometer
const int xInput = A2;
const int yInput = A1;
const int zInput = A0;
const int SAMPLE_SIZE = 10;
int xRawMin;
int xRawMax;
int yRawMin;
int yRawMax;
int zRawMin;
int zRawMax;
int xScaled;
int yScaled;
int zScaled;

//gyroscope
Adafruit_L3GD20 gyro;
int gyroX;
int gyroY;
int gyroZ;

boolean upsideDown;
boolean rotating;

int count;
int COUNT_LIMIT;

unsigned int timeout;
unsigned int TIMEOUT_MAX = 100000;

void setup() {
  
  //set AREF pin to 3v on accelerometer
  analogReference(EXTERNAL);
  
  Serial.begin(9600);
  Serial.println("The Nightmares: Wheeled baton robot");
  Serial.println("version 1.0: 'Thousands of fleeing souls'");
  Serial.println("now with accelerometer and gyroscope!");
  Serial.flush();
  
  //attach esc1 to pin 10
  esc1.attach(10);
  //attach esc2 to pin 11
  esc2.attach(11);
  
  pin_num = 0;
  pin_value = 100;
  esc1_speed = 100;
  esc2_speed = 100;
  
  //set limit lower to limit max speed of robot
  limit = 89; //max limit is 89;
  
  //values for accelerometer, derived from calibration
  //min is -1 g and max is 1 g
  xRawMin = 409;
  xRawMax = 610;

  yRawMin = 400;
  yRawMax = 606;

  zRawMin = 421;
  zRawMax = 623;
  
  xScaled = 0;
  yScaled = 0;
  zScaled = 0;
  
  
  //initialize gyroscope
  if (!gyro.begin(gyro.L3DS20_RANGE_250DPS))
  {
    Serial.println("Oops ... unable to initialize the L3GD20. Check your wiring!");
  }
  
  gyroX = 0;
  gyroY = 0;
  gyroZ = 0;
  
  int throttle = 89; //this the middle value for the servo library
  //throttle = 89 is equivalent to a speed of zero
  esc1.write(throttle);
  esc2.write(throttle);
  esc1.write(throttle);
  esc2.write(throttle);
  esc1.write(throttle);
  esc2.write(throttle);
  
  COUNT_LIMIT = 2000;
  count = 0;
  
  //put pin 4 as input
  //pinMode(4, INPUT);
   
  
  timeout = 0;
}


//main loop for program
void loop()
{
  
  timeout++;
  
  //int isConnected = digitalRead(4);
  //Serial.print("isConnected: ");
  //Serial.println(isConnected);
  
  
  //delay(2); //FIXME, for testing only
  if(count < COUNT_LIMIT)
    count++;
  else
    count = 0;
  
  if(count == COUNT_LIMIT)
  {
    int xRaw = ReadAxis(xInput);
    int yRaw = ReadAxis(yInput);
    int zRaw = ReadAxis(zInput);
    xScaled = map(xRaw, xRawMin, xRawMax, -1000, 1000);
    yScaled = map(yRaw, yRawMin, yRawMax, -1000, 1000);
    zScaled = map(zRaw, zRawMin, zRawMax, -1000, 1000);
    
#ifdef DEBUG
    printAccelerometer(xRaw, yRaw, zRaw, xScaled, yScaled, zScaled);
#endif
   
    gyro.read();
    gyroX = (int) gyro.data.x;
    gyroY = (int) gyro.data.y;
    gyroZ = (int) gyro.data.z;
    
#ifdef DEBUG
    Serial.print("X: "); Serial.print(gyroX);   Serial.print(" ");
    Serial.print("Y: "); Serial.print(gyroY);   Serial.print(" ");
    Serial.print("Z: "); Serial.println(gyroZ); Serial.print(" ");
#endif
  }
  

  
  if (pin_num == 10) {
    esc1_speed = pin_value;
    int throttle = 0;
    
    if(!isUpsideDown(xScaled, yScaled, zScaled))
      throttle = map(esc1_speed, 0, 200, 89-limit, 89+limit);
    else
      throttle = map(esc1_speed, 0, 200, 89+limit, 89-limit);
    
    Serial.print("pin 10 speed: ");
    Serial.println(throttle);
    
    esc1.write(throttle);
    pin_num = 0;
    timeout = 0;
  }
  else if (pin_num == 11) {
    esc2_speed = pin_value;
    
    int throttle = 0;
    
    if(!isUpsideDown(xScaled, yScaled, zScaled))
      throttle = map(esc2_speed, 0, 200, 89-limit, 89+limit);
    else
      throttle = map(esc2_speed, 0, 200, 89+limit, 89-limit);
    
    Serial.print("pin 11 speed: ");
    Serial.println(throttle);
    
    esc2.write(throttle);
    pin_num = 0;
    timeout = 0;
  }
  else if(timeout > TIMEOUT_MAX){
    Serial.print("TIMEOUT!! :");
    Serial.println(timeout);
    int throttle = 89;
    esc1_speed = 100;
    esc2_speed = 100;
    esc1.write(throttle);
    esc2.write(throttle);
    esc1.write(throttle);
    esc2.write(throttle);
    esc1.write(throttle);
    esc2.write(throttle);
  }
  
  //Serial.println();
  //analogWrite(  pin_num, pin_value ); 
  
  
  //GET COMMANDS FROM ANDROID
  Serial.flush();
  char get_char = ' ';  //read serial

  // wait for incoming data
  if (Serial.available() < 1) return; // if serial empty, return to loop().

  // parse incoming command start flag 
  get_char = Serial.read();
  
  if (get_char == '$') {
    Serial.println("STOP!");
    esc1_speed = 100;
    esc1.write(89);
    esc2_speed = 100;
    esc2.write(89);
    pin_num = 0;
  }
  else if(get_char == '!'){
    timeout = 0;
  }
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


//read an axis from the accelerometer
int ReadAxis(int axisPin)
{
  long reading = 0;
  analogRead(axisPin);
  delay(1);
  for (int i = 0; i < SAMPLE_SIZE; i++)
  {
    reading += analogRead(axisPin);
  }
  return reading/SAMPLE_SIZE;
}


//print all values from the accelerometer
void printAccelerometer(int xRaw, int yRaw, int zRaw, long x, long y, long z)
{
  Serial.print("Raw Ranges: X: ");
  Serial.print(xRawMin);
  Serial.print("-");
  Serial.print(xRawMax);
  
  Serial.print(", Y: ");
  Serial.print(yRawMin);
  Serial.print("-");
  Serial.print(yRawMax);
  
  Serial.print(", Z: ");
  Serial.print(zRawMin);
  Serial.print("-");
  Serial.print(zRawMax);
  Serial.println();
  Serial.print(xRaw);
  Serial.print(", ");
  Serial.print(yRaw);
  Serial.print(", ");
  Serial.print(zRaw);
  
  Serial.print(" :: ");
  Serial.print(x);
  Serial.print("mG, ");
  Serial.print(y);
  Serial.print("mG, ");
  Serial.print(z);
  Serial.println("mG");
}

boolean isFlipping(int gX, int gY, int gZ)
{
  if(gX > 100 || gY > 100 || gZ > 100)
    return true;
  else
    return false;
}

boolean isUpsideDown(int x, int y, int z)
{
  if(z >= -50)
    return false;
  else
    return true;
}
