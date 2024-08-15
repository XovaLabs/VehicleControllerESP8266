// MotorControl.cpp

#include "MotorControl.h"

// Define constants
const int MAX_SPEED = 255;
const int MIN_SPEED = 0;
const int MAX_STEERING = 255;
const int MIN_STEERING = 0;
const int DELAY_TIME = 0;

MotorController::MotorController(int forwardPin, int reversePin, int leftPin, int rightPin) {
  // Check if pins are valid
  if (forwardPin < 0 || reversePin < 0 || leftPin < 0 || rightPin < 0) {
    // Handle error
    Serial.println("Invalid pin numbers");
    return;
  }

  _forwardPin = forwardPin;
  _reversePin = reversePin;
  _leftPin = leftPin;
  _rightPin = rightPin;
  pinMode(_forwardPin, OUTPUT);
  pinMode(_reversePin, OUTPUT);
  pinMode(_leftPin, OUTPUT);
  pinMode(_rightPin, OUTPUT);
  _currentSpeed = 0;
  _currentDirection = true;
  _currentSteering = 0;
}

void MotorController::setSpeed(int speed) {
  // Check if speed is within valid range
  if (speed < MIN_SPEED) {
    speed = MIN_SPEED;
  } else if (speed > MAX_SPEED) {
    speed = MAX_SPEED;
  }

  if (speed > _currentSpeed) {
    for (int i = _currentSpeed; i <= speed; i++) {
      if (_currentDirection) {
        analogWrite(_forwardPin, i);
      } else {
        analogWrite(_reversePin, i);
      }
      delay(DELAY_TIME);
    }
  } else if (speed < _currentSpeed) {
    for (int i = _currentSpeed; i >= speed; i--) {
      if (_currentDirection) {
        analogWrite(_forwardPin, i);
      } else {
        analogWrite(_reversePin, i);
      }
      delay(DELAY_TIME);
    }
  }
  _currentSpeed = speed;
}

void MotorController::setDirection(boolean forward) {
  if (forward != _currentDirection) {
    // Gradually lower speed to 0
    for (int i = _currentSpeed; i >= 0; i--) {
      if (_currentDirection) {
        analogWrite(_forwardPin, i);
      } else {
        analogWrite(_reversePin, i);
      }
      delay(DELAY_TIME);
    }

    // Change direction
    _currentDirection = forward;
    if (forward) {
      digitalWrite(_reversePin, LOW);
      digitalWrite(_forwardPin, HIGH);
    } else {
      digitalWrite(_forwardPin, LOW);
      digitalWrite(_reversePin, HIGH);
    }

    // Gradually increase speed to previous value
    for (int i = 0; i <= _currentSpeed; i++) {
      if (_currentDirection) {
        analogWrite(_forwardPin, i);
      } else {
        analogWrite(_reversePin, i);
      }
      delay(DELAY_TIME);
    }
  }
}

void MotorController::setSteering(int direction) {
  // Check if direction is within valid range
  if (direction < MIN_STEERING) {
    direction = MIN_STEERING;
  } else if (direction > MAX_STEERING) {
    direction = MAX_STEERING;
  }

  if (direction > _currentSteering) {
    for (int i = _currentSteering; i <= direction; i++) {
      analogWrite(_rightPin, i);
      analogWrite(_leftPin, MAX_STEERING - i);
      delay(DELAY_TIME);
    }
  } else if (direction < _currentSteering) {
    for (int i = _currentSteering; i >= direction; i--) {
      analogWrite(_rightPin, i);
      analogWrite(_leftPin, MAX_STEERING - i);
      delay(DELAY_TIME);
    }
  }
  _currentSteering = direction;
}

void MotorController::setDirectionForward(int speed) {
  this->setDirection(true);
  this->setSpeed(speed);
}

void MotorController::setDirectionReverse(int speed) {
  this->setDirection(false);
  this->setSpeed(speed);
}

void MotorController::setSteeringLeft(int speed) {
  setSteering(MAX_STEERING - speed);
}

void MotorController::setSteeringRight(int speed) {
  setSteering(speed);
}

MotorController::~MotorController() {
  // Release any resources
  digitalWrite(_forwardPin, LOW);
  digitalWrite(_reversePin, LOW);
  digitalWrite(_leftPin, LOW);
  digitalWrite(_rightPin, LOW);
}