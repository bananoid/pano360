#include "AxeMotor.h"

AxeMotor::AxeMotor(int stepPin, int dirPin, int homePin, long stepsPerRevolution){
  stepper = AccelStepper(AccelStepper::DRIVER, stepPin, dirPin);  
  status = STATUS_HIDLE;
  
  pinMode(homePin, INPUT);

  this->stepsPerRevolution = stepsPerRevolution;
  this->homePin = homePin;

  resetMotor();
}

void AxeMotor::resetMotor(){
  stepper.setMaxSpeed(stepsPerRevolution / 30);
  stepper.setAcceleration(stepsPerRevolution / 40);  
  stepper.setSpeed(2000);
}

void AxeMotor::setStatus(int status){
  this->status = status;
  subStatus = 0;

  switch(status){
    case STATUS_HIDLE:
      Serial.println("STATUS_HIDLE");
      break;
    case STATUS_HOMING:
      Serial.println("STATUS_HOMING");
      break;
    case STATUS_SPIRAL:
      Serial.println("STATUS_SPIRAL");
      break;
  };

}

void AxeMotor::update(){
  switch(status){
    case STATUS_HIDLE:
      
      break;
    case STATUS_HOMING:
      doHoming();
      break;
    case STATUS_SPIRAL:

      break;
  };
}

void AxeMotor::doHoming(){
  int homingTriggered = !digitalRead(homePin);
  
  int speed = 200;
  
  switch (subStatus) {
    case 0:
      stepper.setSpeed(speed); 
      subStatus++;
      Serial.print("START HOMING ");
      Serial.println(speed);
      break;      
    case 1:
      
      if(homingTriggered){
        subStatus++;
        Serial.print("subStatus:");
        Serial.println(subStatus);
      }else{
        stepper.runSpeed();
      }

      break;
    case 2:
      
      if(!homingTriggered){
        subStatus++;
        Serial.print("subStatus:2 ZeroPos ");
        Serial.println(stepper.currentPosition());
        stepper.setCurrentPosition(0);
        stepper.setSpeed(-speed);
      }else{
        stepper.runSpeed();
      }

      break;
    case 3:

      if(homingTriggered){
        subStatus++;
        Serial.print("subStatus:3 currentPos:");
        Serial.println(stepper.currentPosition());
      }else{
        stepper.runSpeed();
      }
      
      break;
    case 4:

      if(!homingTriggered){
        subStatus++;
        Serial.print("subStatus:4 currentPos:");
        Serial.println(stepper.currentPosition());
      }else{
        stepper.runSpeed();
      }

      break;
    case 5:

      if(homingTriggered){
        subStatus++;
        stepsPerRevolution = stepper.currentPosition()*-1;
        resetMotor();
        // stepper.setCurrentPosition(stepsPerRevolution);

        Serial.print("subStatus:5 stepsPerRevolution:");
        Serial.println(stepsPerRevolution);
        stepper.moveTo(0);

      }else{
        stepper.runSpeed();
      }

      break;
    case 6:
      stepper.run();
      
      if(stepper.distanceToGo() == 0){
        status = STATUS_HIDLE;  
      }

      break;
  };
}