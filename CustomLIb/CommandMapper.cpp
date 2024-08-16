#include "CommandMapper.h"

CommandMapper::CommandMapper(MotorController& motor) : _motor(motor) {
    // Initialize commands using only the first letter
    _commands[0] = Command("F", &MotorController::setDirectionForward, nullptr); // forward
    _commands[1] = Command("R", &MotorController::setDirectionReverse, nullptr); // reverse
    _commands[2] = Command("L", nullptr, &MotorController::setSteeringLeft); // left
    _commands[3] = Command("E", nullptr, &MotorController::setSteeringRight); // right
    _commands[4] = Command("C", &MotorController::setDirectionForward, &MotorController::setSpeed); // stop
}

void CommandMapper::processCommand(const char* command) {
    char direction = command[0];
    int speed = atoi(command + 1); // Extract the speed part

    for (int i = 0; i < 5; i++) { // Adjust the loop to cover all commands
        if (_commands[i].direction != nullptr && _commands[i].direction[0] == direction) {
            if (_commands[i].directionMethod != nullptr) {
                (_motor.*(_commands[i].directionMethod))(speed);
            }
            if (_commands[i].speedMethod != nullptr) {
                (_motor.*(_commands[i].speedMethod))(speed);
            }
            return;
        }
    }
    Serial.println("Invalid command");
}
