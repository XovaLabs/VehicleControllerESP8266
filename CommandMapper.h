// CommandMapper.h

#ifndef CommandMapper_h
#define CommandMapper_h
#include "MotorControl.h"

#include <Arduino.h>

class MotorController;

class CommandMapper {
  public:
    CommandMapper(MotorController& motor);
    void processCommand(const char* command);

  private:
    MotorController& _motor;
    struct Command {
      char direction;
      void (MotorController::*directionMethod)(boolean);
      void (MotorController::*speedMethod)(int);
    };
    Command _commands[4];
};

#endif