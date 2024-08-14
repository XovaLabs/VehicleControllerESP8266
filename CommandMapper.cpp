// CommandMapper.cpp

#include "CommandMapper.h"

CommandMapper::CommandMapper(MotorController& motor) : _motor(motor) {
  _commands[0] = {'f', &MotorController::setDirection, &MotorController::setSpeed}; // forward
  _commands[1] = {'r', &MotorController::setDirection, &MotorController::setSpeed}; // reverse
  _commands[2] = {'l', nullptr, &MotorController::setSteering}; // left
  _commands[3] = {'r', nullptr, &MotorController::setSteering}; // right
}

void CommandMapper::processCommand(const char* command) {
  char direction = command[0];
  int speed = atoi(command + 1);

  for (int i = 0; i < 4; i++) {
    if (_commands[i].direction == direction) {
      if (_commands[i].directionMethod != nullptr) {
        (_motor.*(_commands[i].directionMethod))(direction == 'f');
      }
      (_motor.*(_commands[i].speedMethod))(speed);
      return;
    }
  }
  Serial.println("Invalid command");
}