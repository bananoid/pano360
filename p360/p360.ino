#include <AccelStepper.h>
#include <math.h>

#define LON_STEP_PER_REVOLUTION 5800
#define LAT_STEP_PER_REVOLUTION 25600

// The X Stepper pins
#define LON_STEPPER_STEP_PIN 2
#define LON_STEPPER_DIR_PIN 3
// The Y stepper pins
#define LAT_STEPPER_STEP_PIN 6
#define LAT_STEPPER_DIR_PIN 7

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
bool runing = true;

void setup()
{
  pinMode(SHUTTER_PIN, OUTPUT);

  lonStepper.setMaxSpeed(LON_STEP_PER_REVOLUTION / 10); // piatto
  lonStepper.setAcceleration(LON_STEP_PER_REVOLUTION / 20);
  // lonStepper.moveTo(0);
  
  latStepper.setMaxSpeed(LAT_STEP_PER_REVOLUTION / 30); // macchina fotografica
  latStepper.setAcceleration(LAT_STEP_PER_REVOLUTION / 40);
  // latStepper.moveTo(0);


  Serial.begin(9600);
  
  // for(int i=0; i<=nbrPoints; i++){
  //   Serial.print(i);
  //   Serial.print("\t");
  //   Serial.print( getLat(i) );
  //   Serial.print("\t");
  //   Serial.print( getLon(i) );
  //   Serial.println();
  // }

  int lonSteps = radToStep( getLon(0), LON_STEP_PER_REVOLUTION );
  int latSteps = radToStep( getLat(0), LAT_STEP_PER_REVOLUTION );

  lonStepper.setCurrentPosition(lonSteps);
  latStepper.setCurrentPosition(latSteps);

}

void loop()
{
    
    if(runing && lonStepper.distanceToGo() == 0 && latStepper.distanceToGo() == 0){
      delay(shutterTime);

      digitalWrite(SHUTTER_PIN, 1);
      delay(300);
      digitalWrite(SHUTTER_PIN, 0);
      delay(30);

      int lonSteps = radToStep( getLon(currentPtInx), LON_STEP_PER_REVOLUTION );
      int latSteps = radToStep( getLat(currentPtInx), LAT_STEP_PER_REVOLUTION );
      
      // lonSteps = loopStep(lonSteps, LON_STEP_PER_REVOLUTION );

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