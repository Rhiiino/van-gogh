#pragma once

#include <Arduino.h>

#include "types.h"

#if __has_include("esp32-hal-rmt.h")
#include "esp32-hal-rmt.h"
#define VAN_GOGH_RMT 1
#else
#define VAN_GOGH_RMT 0
#endif

// DShot frame: 11-bit throttle, telemetry bit, 4-bit XOR checksum. MSB first.
inline uint16_t dshotFrame(uint16_t throttle, bool telemetry) {
  if (throttle > 2047) {
    throttle = 2047;
  }
  uint16_t value = (uint16_t)((throttle << 1) | (telemetry ? 1u : 0u));
  uint16_t checksum = 0;
  uint16_t nibbles = value;
  for (int i = 0; i < 3; ++i) {
    checksum ^= nibbles;
    nibbles >>= 4;
  }
  checksum &= 0x0Fu;
  return (uint16_t)((value << 4) | checksum);
}

inline uint32_t dshotBitNs(SignalKind kind) {
  if (kind == kSignalDshot600) {
    return 1667u;
  }
  if (kind == kSignalDshot300) {
    return 3333u;
  }
  return 6667u;
}

inline uint16_t dshotFromPercent(uint8_t percent, uint8_t capPercent) {
  if (percent == 0 || capPercent == 0) {
    return 0;
  }
  const uint32_t scaled = (uint32_t)percent * capPercent / 100u;
  if (scaled == 0) {
    return 48;
  }
  return (uint16_t)(48u + ((2047u - 48u) * scaled / 100u));
}

inline int dshotSlot(int pin) {
  return pin == kFlywheelPin1 ? 0 : 1;
}

#if VAN_GOGH_RMT

inline void dshotFillSymbols(rmt_data_t *symbols, uint16_t frame, SignalKind kind) {
  const uint32_t bitTicks = dshotBitNs(kind) / 100u;
  const uint32_t t0h = bitTicks * 37u / 100u;
  const uint32_t t1h = bitTicks * 75u / 100u;
  for (int bit = 15; bit >= 0; --bit) {
    const bool one = (frame >> bit) & 1u;
    rmt_data_t &symbol = symbols[15 - bit];
    symbol.level0 = 1;
    symbol.duration0 = one ? t1h : t0h;
    symbol.level1 = 0;
    symbol.duration1 = bitTicks - symbol.duration0;
  }
}

#if ESP_ARDUINO_VERSION_MAJOR >= 3

struct DshotPorts {
  bool ready[kFlywheelCount];
};

inline DshotPorts &dshotPorts() {
  static DshotPorts ports = {};
  return ports;
}

inline bool dshotPrepare(int pin) {
  DshotPorts &ports = dshotPorts();
  const int slot = dshotSlot(pin);
  if (ports.ready[slot]) {
    return true;
  }
  if (!rmtInit(pin, RMT_TX_MODE, RMT_MEM_NUM_BLOCKS_1, 10000000)) {
    return false;
  }
  ports.ready[slot] = true;
  return true;
}

inline bool dshotWriteRmt(int pin, uint16_t frame, SignalKind kind) {
  if (!dshotPrepare(pin)) {
    return false;
  }
  rmt_data_t symbols[16];
  dshotFillSymbols(symbols, frame, kind);
  return rmtWrite(pin, symbols, 16, 10);
}

inline void dshotRelease(int pin) {
  DshotPorts &ports = dshotPorts();
  const int slot = dshotSlot(pin);
  if (!ports.ready[slot]) {
    return;
  }
  rmtDeinit(pin);
  ports.ready[slot] = false;
}

#else

struct DshotPorts {
  rmt_obj_t *port[kFlywheelCount];
};

inline DshotPorts &dshotPorts() {
  static DshotPorts ports = {};
  return ports;
}

inline bool dshotPrepare(int pin) {
  DshotPorts &ports = dshotPorts();
  const int slot = dshotSlot(pin);
  if (ports.port[slot] != nullptr) {
    return true;
  }
  rmt_obj_t *port = rmtInit(pin, RMT_TX_MODE, RMT_MEM_64);
  if (port == nullptr) {
    return false;
  }
  rmtSetTick(port, 100.0f);
  ports.port[slot] = port;
  return true;
}

inline bool dshotWriteRmt(int pin, uint16_t frame, SignalKind kind) {
  if (!dshotPrepare(pin)) {
    return false;
  }
  rmt_data_t symbols[16];
  dshotFillSymbols(symbols, frame, kind);
  return rmtWrite(dshotPorts().port[dshotSlot(pin)], symbols, 16);
}

inline void dshotRelease(int pin) {
  DshotPorts &ports = dshotPorts();
  const int slot = dshotSlot(pin);
  if (ports.port[slot] == nullptr) {
    return;
  }
  rmtDeinit(ports.port[slot]);
  ports.port[slot] = nullptr;
}

#endif

#else

inline void dshotRelease(int pin) {
  (void)pin;
}

#endif

inline void dshotWriteBitbang(int pin, uint16_t frame, SignalKind kind) {
  const uint32_t bitUs = dshotBitNs(kind) / 1000u;
  const uint32_t t0h = bitUs * 37u / 100u;
  const uint32_t t1h = bitUs * 75u / 100u;
  pinMode(pin, OUTPUT);
  noInterrupts();
  for (int bit = 15; bit >= 0; --bit) {
    const bool one = (frame >> bit) & 1u;
    const uint32_t highUs = one ? t1h : t0h;
    const uint32_t lowUs = bitUs - highUs;
    digitalWrite(pin, HIGH);
    delayMicroseconds(highUs);
    digitalWrite(pin, LOW);
    delayMicroseconds(lowUs);
  }
  interrupts();
}

inline void dshotWrite(int pin, uint16_t frame, SignalKind kind) {
#if VAN_GOGH_RMT
  if (!dshotWriteRmt(pin, frame, kind)) {
    dshotWriteBitbang(pin, frame, kind);
  }
#else
  dshotWriteBitbang(pin, frame, kind);
#endif
}
