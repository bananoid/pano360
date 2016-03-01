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

AxeMotor *lonAxe;
AxeMotor *latAxe;

int nbrPoints = 60;
int currentPtInx = 0;

int incomingByte = 0;
int shutterTime = 10;

bool runing = false;
int homingStatus = 3; // 0=off - 1=homing lon - 2=homing lat

void setup()
{
  pinMode(SHUTTER_PIN, OUTPUT);
  Serial.begin(9600);

  // lonAxe = new AxeMotor(
  //   LON_STEPPER_STEP_PIN, 
  //   LON_STEPPER_DIR_PIN, 
  //   LON_STEPPER_HOMING_PIN,
  //   LON_INIT_STEP_PER_REVOLUTION
  //   );

  latAxe = new AxeMotor(
    LAT_STEPPER_STEP_PIN,
    LAT_STEPPER_DIR_PIN,
    LAT_STEPPER_HOMING_PIN,
    LAT_INIT_STEP_PER_REVOLUTION
    );

  latAxe->setStatus(AxeMotor::STATUS_HOMING);  
}

void loop()
{
  // lonAxe->update();
  latAxe->update();
}


// void do360Spiral(){
//   if(!runing) return;

//   if(lonStepper.distanceToGo() == 0 && latStepper.distanceToGo() == 0){

//     delay(shutterTime);

//     digitalWrite(SHUTTER_PIN, 1);
//     delay(300);
//     digitalWrite(SHUTTER_PIN, 0);
//     delay(30);

//     int lonSteps = radToStep( getLon(currentPtInx), lonStepPerRevolution );
//     int latSteps = radToStep( getLat(currentPtInx), latStepPerRevolution );
    
//     // lonSteps = loopStep(lonSteps, lonStepPerRevolution );

//     lonStepper.moveTo(lonSteps);
//     latStepper.moveTo(latSteps);
    
//     Serial.print(currentPtInx);
//     Serial.print("\t");
//     Serial.print( getLon(currentPtInx) );
//     Serial.print("\t");
//     Serial.print( getLat(currentPtInx) );
//     Serial.println();

//     currentPtInx++;  
//     currentPtInx = currentPtInx % nbrPoints;   
    
//     if(currentPtInx == 0){
//       runing = false;
//     }
  
//   }
// }

// float getLon(int ptinx){
//   float lon = GA * ptinx;
//   lon /= 2*PI; lon -= floor(lon); lon *= 2*PI;
//   if (lon > PI)  lon -= 2*PI;
//   return lon;
// }

// float getLat(int ptinx){
//   float lat = asin(-1.0 + 2.0 * (float)ptinx/(float)nbrPoints);
//   return lat;
// }

// int radToStep(float rad, int revSteps ){
//   float step = rad / (2*PI) * (float)revSteps;
//   return int(step); 
// }

// int loopStep( int step , int revSteps ){
//   step = (int)round(step) % revSteps;
//   if(step < 0){
//     step = revSteps + step;
//   }
//   return step;
// }