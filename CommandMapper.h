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
        const char* direction;
        void (MotorController::*directionMethod)(int);
        void (MotorController::*speedMethod)(int);

        // Default constructor
        Command() : direction(nullptr), directionMethod(nullptr), speedMethod(nullptr) {}

        // Parameterized constructor
        Command(const char* dir, void (MotorController::*dirMethod)(int), void (MotorController::*spdMethod)(int))
            : direction(dir), directionMethod(dirMethod), speedMethod(spdMethod) {}
    };


    Command _commands[5];
};

#endif