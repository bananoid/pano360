#include <AccelStepper.h>
#include "AxeMotor.h"
#include <math.h>


#define LON_INIT_STEP_PER_REVOLUTION 5800
#define LAT_INIT_STEP_PER_REVOLUTION 25600

// The X Stepper pins
#define LON_STEPPER_STEP_PIN 2
#define LON_STEPPER_DIR_PIN 3
#define LON_STEPPER_HOMING_PIN 4
// The Y stepper pins
#define LAT_STEPPER_STEP_PIN 6
#define LAT_STEPPER_DIR_PIN 7
#define LAT_STEPPER_HOMING_PIN 8

#define SHUTTER_PIN 13
#define START_BTN_PIN 9

#define PHI 0.6180339887498949
#define GA  3.883222077450933

#define JOYSTIC_X_MIN 300
#define JOYSTIC_X_MAX 600
#define JOYSTIC_Y_MIN 298
#define JOYSTIC_Y_MAX 818 

AxeMotor *lonAxe;
AxeMotor *latAxe;

int nbrPoints = 64;
int currentPtInx = 0;

int incomingByte = 0;
int shutterTime = 2000;

bool runing = true;

enum {
  MODE_JOYSTIC,
  MODE_HOMING,
  MODE_PSPIRAL
};

int mode = MODE_JOYSTIC;

void setup()
{
  pinMode(SHUTTER_PIN, OUTPUT);
  Serial.begin(9600);
  Serial.println("------------CORRA TROTTOLA-----------");
  Serial.println("COMMAND j = JOYSTIC MODE");
  Serial.println("COMMAND h = HOMING MODE");
  Serial.println("COMMAND s = SPIRAL MODE");

  latAxe = new AxeMotor(
    LAT_STEPPER_STEP_PIN,
    LAT_STEPPER_DIR_PIN,
    LAT_STEPPER_HOMING_PIN,
    LAT_INIT_STEP_PER_REVOLUTION
    );


  lonAxe = new AxeMotor(
    LON_STEPPER_STEP_PIN, 
    LON_STEPPER_DIR_PIN, 
    LON_STEPPER_HOMING_PIN,
    LON_INIT_STEP_PER_REVOLUTION
    );

  // latAxe->setStatus(AxeMotor::STATUS_HOMING);
  // lonAxe->setStatus(AxeMotor::STATUS_HOMING);

}


void setMode(int m){
  mode = m;
  switch (mode) {
    case MODE_JOYSTIC:
      break;
    case MODE_HOMING:
      // do something
      latAxe->setStatus(AxeMotor::STATUS_HOMING);
      lonAxe->setStatus(AxeMotor::STATUS_HOMING);
      break;
    case MODE_PSPIRAL:
      currentPtInx = 0;
      latAxe->setStatus(AxeMotor::STATUS_HIDLE);
      lonAxe->setStatus(AxeMotor::STATUS_HIDLE);
      break;
  }
}


void loop()
{

  if (Serial.available() > 0){
    char incomingByte = Serial.read();
    
    if(incomingByte == 'j'){
      Serial.println("JOYSTIC MODE");
      setMode(MODE_JOYSTIC);
    }else if(incomingByte == 'h'){
      Serial.println("HOMING MODE");
      setMode(MODE_HOMING);  
    }else if(incomingByte == 's'){
      Serial.println("SPIRAL MODE");
      setMode(MODE_PSPIRAL);
    }
  }

  switch (mode) {
      case MODE_JOYSTIC:
        // do something
        moveWithJoistic();
        break;
      case MODE_HOMING:
        // do something
        latAxe->update();
        lonAxe->update();
        break;
      case MODE_PSPIRAL:
        do360Spiral();
        latAxe->update();
        lonAxe->update();
        break;
  }
  
  // if(
  //   latAxe->status ==  AxeMotor::STATUS_HIDLE 
  //   && 
  //   lonAxe->status ==  AxeMotor::STATUS_HIDLE
  //   ){
  //   //
    
  // }

  // // 
  // // 

}


void do360Spiral(){
  if(!runing) return;


  if(lonAxe->stepper.distanceToGo() == 0 && latAxe->stepper.distanceToGo() == 0){

    delay(500);

    digitalWrite(SHUTTER_PIN, 1);
    delay(300);
    digitalWrite(SHUTTER_PIN, 0);
    delay(30);

    delay(shutterTime);

    int modIndex = currentPtInx * 8;
    modIndex = modIndex % nbrPoints;


    long lonSteps = radToStep( getLon(modIndex), lonAxe->stepsPerRevolution );
    long latSteps = radToStep( getLat(modIndex), latAxe->stepsPerRevolution );
    
    // lonSteps = loopStep(lonSteps, latAxe->stepperRevolution );

    lonAxe->stepper.moveTo(lonSteps);
    latAxe->stepper.moveTo(latSteps);
    
    Serial.print(currentPtInx);
    Serial.print("\t");
    Serial.print( lonSteps / (float)lonAxe->stepsPerRevolution  * 360.0 , DEC );
    Serial.print("\t");
    Serial.print( lonAxe->stepsPerRevolution );
    Serial.print("\t");
    Serial.print( latSteps / (float)latAxe->stepsPerRevolution  * 360.0 , DEC);
    Serial.print("\t");
    Serial.print( latAxe->stepsPerRevolution );
    Serial.println();

    currentPtInx++;  
    currentPtInx = currentPtInx % nbrPoints;   
    
    if(currentPtInx == 0){
      runing = false;
    }
  
  }
}

float getLon(int ptinx){
  float lon = GA * ptinx;
  // lon = PI * ptinx;
  // lon /= 2*PI; lon -= floor(lon); lon *= 2*PI;
  // if (lon > PI)  lon -= 2*PI;
  return fmod(lon , (2.0*PI));
  return lon;
}

float getLat(int ptinx){
  float lat = asin(-1.0 + 2.0 * (float)ptinx/(float)nbrPoints);
  lat += PI/2;
  // return ptinx * PI;
  return lat;
}

long radToStep(float rad, int revSteps ){
  float step = rad / (2*PI) * (float)revSteps;
  return (long)step; 
}

int loopStep( int step , int revSteps ){
  step = (int)round(step) % revSteps;
  if(step < 0){
    step = revSteps + step;
  }
  return step;
}




void moveWithJoistic()
{
  int xVal = analogRead(A0);
  xVal = min(xVal,JOYSTIC_X_MAX);
  xVal = max(xVal,JOYSTIC_X_MIN);
  float xValMap = mapf((float)xVal, (float)JOYSTIC_X_MIN, (float)JOYSTIC_X_MAX, -1,1);
  xValMap = round(xValMap*10.0)/10.0 * 2;

  int yVal = analogRead(A1);
  yVal = min(yVal,JOYSTIC_Y_MAX);
  yVal = max(yVal,JOYSTIC_Y_MIN);
  float yValMap = mapf((float)yVal, (float)JOYSTIC_Y_MIN, (float)JOYSTIC_Y_MAX, -1,1);
  yValMap = round(yValMap*10.0)/10.0 * 2;
  

  // int moveX = (xValMap * xValMap * xValMap)/3;  
  int speedX = xValMap * (float)lonAxe->stepsPerRevolution/20.0;
  int speedY = yValMap * (float)latAxe->stepsPerRevolution/20.0;

  // latAxe->stepper->move(moveX);

  lonAxe->stepper.setSpeed(speedX);
  lonAxe->stepper.runSpeed();

  latAxe->stepper.setSpeed(speedY);
  latAxe->stepper.runSpeed();

  // latAxe->stepper.move(speedY);
  // Serial.print("raw x:");
  // Serial.print(xVal);
  // Serial.print("\tmap x:");
  // Serial.print(xValMap);
  // Serial.print("\tpos x:");
  // Serial.print(speedX);  
  // Serial.println("");

  // Serial.print("raw y:");
  // Serial.print(yVal);
  // Serial.print("\tmap y:");
  // Serial.print(yValMap);
  // Serial.print("\tspeed y:");
  // Serial.print(speedY);  
  // Serial.println("");

}

float mapf(float x, float in_min, float in_max, float out_min, float out_max)
{
  return (float)(x - in_min) * (out_max - out_min) / (float)(in_max - in_min) + out_min;
}


