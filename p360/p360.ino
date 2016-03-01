#include <AccelStepper.h>
#include <math.h>

#define LON_STEP_PER_REVOLUTION 5800
#define LAT_STEP_PER_REVOLUTION 25600

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

// Define some steppers and the pins the will use
AccelStepper lonStepper(AccelStepper::DRIVER, LON_STEPPER_STEP_PIN, LON_STEPPER_DIR_PIN);
AccelStepper latStepper(AccelStepper::DRIVER, LAT_STEPPER_STEP_PIN, LAT_STEPPER_DIR_PIN);

int nbrPoints = 60;
int currentPtInx = 0;

int incomingByte = 0;
int shutterTime = 10;

bool runing = false;
int homingStatus = 3; // 0=off - 1=homing lon - 2=homing lat

int lonStepPerRevolution = LON_STEP_PER_REVOLUTION;
int latStepPerRevolution = LAT_STEP_PER_REVOLUTION;

void setup()
{
  pinMode(SHUTTER_PIN, OUTPUT);
  pinMode(LON_STEPPER_HOMING_PIN, INPUT);
  pinMode(LAT_STEPPER_HOMING_PIN, INPUT);

  Serial.begin(9600);
  
  // for(int i=0; i<=nbrPoints; i++){
  //   Serial.print(i);
  //   Serial.print("\t");
  //   Serial.print( getLat(i) );
  //   Serial.print("\t");
  //   Serial.print( getLon(i) );
  //   Serial.println();
  // }

  resetMotors();

}

void doHoming(){
  if(homingStatus <= 0) return;

  if(homingStatus == 1){
    
    if(lonStepper.distanceToGo() == 0){
      int nextPosition = lonStepper.currentPosition() + 1000;
      lonStepper.moveTo(nextPosition);
    }

    int lonHomingTriggered = !digitalRead(LON_STEPPER_HOMING_PIN);
    if(lonHomingTriggered && lonStepper.currentPosition() > 300 ){ 
      // lonStepper.moveTo(lonStepper.currentPosition());  
      
      Serial.print("first zero at ");
      Serial.println(lonStepper.currentPosition());

      lonStepper.setCurrentPosition(0);
      lonStepper.moveTo(0);

      homingStatus++;
    }

  }else if(homingStatus == 2){ 
    if(lonStepper.distanceToGo() == 0){
      int nextPosition = lonStepper.currentPosition() + 1000;
      lonStepper.moveTo(nextPosition);
    }
    int lonHomingTriggered = !digitalRead(LON_STEPPER_HOMING_PIN);
    if(lonHomingTriggered && lonStepper.currentPosition() > 300 ){

      lonStepPerRevolution = lonStepper.currentPosition();

      Serial.print("lonStepPerRevolution: ");
      Serial.println(lonStepPerRevolution);

      // lonStepper.setCurrentPosition(0);
      lonStepper.moveTo(0);

      homingStatus++; 
    }
  }else if(homingStatus == 3){
    
    if(latStepper.distanceToGo() == 0){
      int nextPosition = latStepper.currentPosition() + 1000;
      latStepper.moveTo(nextPosition);
    }

    int latHomingTriggered = !digitalRead(LAT_STEPPER_HOMING_PIN);
    if(latHomingTriggered && latStepper.currentPosition() > 300 ){ 
      // latStepper.moveTo(latStepper.currentPosition());  
      
      Serial.print("first zero at ");
      Serial.println(latStepper.currentPosition());

      latStepper.setCurrentPosition(0);
      latStepper.moveTo(0);

      homingStatus++;
    }

  }else if(homingStatus == 4){ 
    if(latStepper.distanceToGo() == 0){
      int nextPosition = latStepper.currentPosition() + 1000;
      latStepper.moveTo(nextPosition);
    }
    int latHomingTriggered = !digitalRead(LAT_STEPPER_HOMING_PIN);
    if(latHomingTriggered && latStepper.currentPosition() > 300 ){

      latStepPerRevolution = latStepper.currentPosition();

      Serial.print("latStepPerRevolution: ");
      Serial.println(latStepPerRevolution);

      // latStepper.setCurrentPosition(0);
      latStepper.moveTo(0);

      homingStatus++; 
    }
  }



}

void resetMotors(){
  lonStepper.setMaxSpeed(lonStepPerRevolution / 10); // piatto
  lonStepper.setAcceleration(lonStepPerRevolution / 20);
  // lonStepper.moveTo(0);
  
  latStepper.setMaxSpeed(latStepPerRevolution / 30); // macchina fotografica
  latStepper.setAcceleration(latStepPerRevolution / 40);
  // latStepper.moveTo(0);

  // int lonSteps = radToStep( getLon(0), lonStepPerRevolution );
  // int latSteps = radToStep( getLat(0), latStepPerRevolution );
  // lonStepper.setCurrentPosition(lonSteps);
  // latStepper.setCurrentPosition(latSteps);
}

void do360Spiral(){
  if(!runing) return;

  if(lonStepper.distanceToGo() == 0 && latStepper.distanceToGo() == 0){

    delay(shutterTime);

    digitalWrite(SHUTTER_PIN, 1);
    delay(300);
    digitalWrite(SHUTTER_PIN, 0);
    delay(30);

    int lonSteps = radToStep( getLon(currentPtInx), lonStepPerRevolution );
    int latSteps = radToStep( getLat(currentPtInx), latStepPerRevolution );
    
    // lonSteps = loopStep(lonSteps, lonStepPerRevolution );

    lonStepper.moveTo(lonSteps);
    latStepper.moveTo(latSteps);
    
    Serial.print(currentPtInx);
    Serial.print("\t");
    Serial.print( getLon(currentPtInx) );
    Serial.print("\t");
    Serial.print( getLat(currentPtInx) );
    Serial.println();

    currentPtInx++;  
    currentPtInx = currentPtInx % nbrPoints;   
    
    if(currentPtInx == 0){
      runing = false;
    }
  
  }


}

void loop()
{
  
  doHoming();

  // if (Serial.available())
  // {
  //   int f = Serial.parseInt();  

  //   lonStepper.moveTo(f);
  //   Serial.println(f); 
  // } 



  // if (lonStepper.distanceToGo() == 0)
  //   lonStepper.moveTo(-lonStepper.currentPosition());

  // if (latStepper.distanceToGo() == 0)
  //   latStepper.moveTo(-latStepper.currentPosition());

  lonStepper.run();
  latStepper.run();
}

float getLon(int ptinx){
  float lon = GA * ptinx;
  lon /= 2*PI; lon -= floor(lon); lon *= 2*PI;
  if (lon > PI)  lon -= 2*PI;
  return lon;
}

float getLat(int ptinx){
  float lat = asin(-1.0 + 2.0 * (float)ptinx/(float)nbrPoints);
  return lat;
}

int radToStep(float rad, int revSteps ){
  float step = rad / (2*PI) * (float)revSteps;
  return int(step); 
}

int loopStep( int step , int revSteps ){
  step = (int)round(step) % revSteps;
  if(step < 0){
    step = revSteps + step;
  }
  return step;
}