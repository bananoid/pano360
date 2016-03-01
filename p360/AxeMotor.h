#include "Arduino.h"
#include <AccelStepper.h>

class AxeMotor
{
public:
  AccelStepper stepper;
  
  long stepPerRevolution;
  
  enum {
    STATUS_HIDLE,
    STATUS_HOMING,
    STATUS_SPIRAL
  };

  int status;
  unsigned int homePin;

  int subStatus;

  long stepsPerRevolution;


  AxeMotor(unsigned int stepPin, unsigned int dirPin, unsigned int homePin, long stepsPerRevolution);

  void resetMotor();
  
  void update();

  void setStatus(int status);

  void doHoming();

private:
  long strepFromLastState;
  long lastHomingTriggeredTime;
  bool lastHomingTriggered;
};
