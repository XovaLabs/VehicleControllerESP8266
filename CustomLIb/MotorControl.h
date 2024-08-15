// MotorController.h

#ifndef MotorControl_h
#define MotorControl_h

#include <Arduino.h>

class MotorController {
  public:
    MotorController(int forwardPin, int reversePin, int leftPin, int rightPin);
    ~MotorController();

    void setSpeed(int speed);
    void setDirectionForward(int speed);
    void setDirectionReverse(int speed);
    void setSteeringLeft(int speed);
    void setSteeringRight(int speed);

    void setSteering(int direction);
    void setDirection(boolean forward);

  private:
    int _forwardPin;
    int _reversePin;
    int _leftPin;
    int _rightPin;
    int _currentSpeed;
    boolean _currentDirection;
    int _currentSteering;
};

#endif