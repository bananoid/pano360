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
  int homePin;

  int subStatus;

  long stepsPerRevolution;

  AxeMotor(int stepPin, int dirPin, int homePin, long stepsPerRevolution);

  void resetMotor();
  
  void update();

  void setStatus(int status);

  void doHoming();
};
