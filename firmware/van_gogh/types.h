#pragma once

#include "pins.h"

// Shared with the sketch. Arduino's prototype generator has to see these
// before it invents prototypes for the .ino.

enum SignalKind : uint8_t {
  kSignalPwm = 0,
  kSignalDshot150 = 1,
  kSignalDshot300 = 2,
  kSignalDshot600 = 3
};

enum WheelStop : uint8_t {
  kWheelCut = 0,
  kWheelCoast = 1
};

enum MotorStop : uint8_t {
  kMotorCoast = 0,
  kMotorBrake = 1
};

enum LedPattern : uint8_t {
  kLedOff = 0,
  kLedSolid = 1,
  kLedOne = 2,
  kLedChase = 3
};

struct Flywheel {
  int pin;
  bool armed;
  uint8_t percent;
  uint16_t pulseUs;
  uint16_t dshotValue;
  bool timed;
  uint32_t runStartMs;
  uint32_t runForMs;
  WheelStop stopKind;
  bool coasting;
  uint32_t coastStartMs;
  uint16_t coastFromUs;
};

struct Gearmotor {
  int pinA;
  int pinB;
  bool running;
  bool forward;
  MotorStop stopKind;
  uint32_t runStartMs;
  uint32_t runForMs;
};
