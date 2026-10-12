#pragma once

// Logic-board outputs. Connector detail is in board.md.

static const int kFlywheelPin1 = 1;  // J1 pin 5, ESC 1 signal
static const int kFlywheelPin2 = 2;  // J1 pin 6, ESC 2 signal
static const int kDialPinA = 4;      // DRV8833 INA1, J2
static const int kDialPinB = 5;      // DRV8833 INA2, J2
static const int kConePinA = 6;      // DRV8833 INA3, J3
static const int kConePinB = 7;      // DRV8833 INA4, J3
static const int kLaserPin = 8;      // Q1 gate, J5
static const int kSolenoidPin = 9;   // Q2 gate, J6
static const int kLedPin = 10;       // WS2812 data, J4

static const int kFlywheelCount = 2;
static const int kFlywheelPins[kFlywheelCount] = {kFlywheelPin1, kFlywheelPin2};

static const uint32_t kPwmHz = 50;
static const uint8_t kPwmBits = 14;
static const uint32_t kPwmPeriodUs = 1000000u / kPwmHz;
static const uint16_t kPulseIdleUs = 1000;
static const uint16_t kPulseFullUs = 2000;

static const uint32_t kRunMsMin = 50;
static const uint32_t kRunMsMax = 60000;
static const uint32_t kRunMsDefault = 3000;
static const uint32_t kCoastMs = 1000;

static const uint32_t kSolenoidMaxMs = 8000;
static const uint8_t kThrottleCapDefault = 40;

static const uint16_t kLedCountDefault = 12;
static const uint16_t kLedCountMax = 64;
