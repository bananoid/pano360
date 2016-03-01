#include "AxeMotor.h"

AxeMotor::AxeMotor(unsigned int stepPin, unsigned int dirPin, unsigned int homePin, long stepsPerRevolution){
  
  stepper = AccelStepper(AccelStepper::DRIVER, stepPin, dirPin);  
  status = STATUS_HIDLE;
  
  this->stepsPerRevolution = stepsPerRevolution;
  this->homePin = homePin;

  pinMode(this->homePin, INPUT);

  resetMotor();
}

void AxeMotor::resetMotor(){
  stepper.setMaxSpeed(stepsPerRevolution / 30);
  stepper.setAcceleration(stepsPerRevolution / 40);  
  
  stepper.setSpeed(stepsPerRevolution / 29);
}

void AxeMotor::setStatus(int status){
  this->status = status;
  subStatus = 0;

  switch(status){
    case STATUS_HIDLE:
      Serial.println("STATUS_HIDLE");
      break;
    case STATUS_HOMING:
      lastHomingTriggeredTime = 0;
      lastHomingTriggered = 0;

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
  int readHomingTriggered = !digitalRead(this->homePin);
    
  long deltaTime = millis() - lastHomingTriggeredTime;

  if(deltaTime > 100 && lastHomingTriggered != readHomingTriggered ){
    lastHomingTriggeredTime = millis();
    lastHomingTriggered = readHomingTriggered;
    Serial.print("lastHomingTriggered " );
    Serial.println( this->homePin );
    

  }

  int homingTriggered = lastHomingTriggered;
  // homingTriggered = readHomingTriggered;

  // Serial.print("this->homePin:");
  // Serial.print(this->homePin);
  // Serial.print("homingTriggered:");
  // Serial.println(lastHomingTriggeredTime);

  int speed = stepsPerRevolution / 30;
  
  switch (subStatus) {
    case 0:
      stepper.setSpeed(speed); 
      subStatus++;
      Serial.print("START HOMING ");
      Serial.println(speed);
      strepFromLastState = 0;
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