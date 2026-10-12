/*
  Van Gogh barrel — component test page for the ESP32-S3 Mini on U3.

  Arduino IDE:
    Board: ESP32S3 Dev Module
    USB CDC On Boot: Enabled
    Upload Speed: 115200
  Libraries:
    WiFiManager by tzapu
    Adafruit NeoPixel by Adafruit

  Boot leaves every output off. The page is served at http://van-gogh.local
  after the board joins Wi-Fi. Open firmware/index.html to preview that page
  without flashing.
*/

#include <WiFi.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <WiFiManager.h>
#include <Adafruit_NeoPixel.h>

#include "pins.h"
#include "types.h"
#include "json_util.h"
#include "dshot.h"
#include "page.h"

#if __has_include("secrets.h")
#include "secrets.h"
#endif

#ifndef WIFI_SSID
#define WIFI_SSID ""
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD ""
#endif

static const char *kHost = "van-gogh";
static const char *kSetupAp = "Van-Gogh-Setup";
static const uint32_t kDshotGapMs = 4;
static const uint32_t kChaseMs = 120;

WebServer server(80);
Adafruit_NeoPixel gStrip(kLedCountMax, kLedPin, NEO_GRB + NEO_KHZ800);

SignalKind gSignal = kSignalPwm;
uint8_t gCapPercent = kThrottleCapDefault;
bool gPwmOn[kFlywheelCount];
Flywheel gWheel[kFlywheelCount];
uint32_t gDshotSentMs = 0;

Gearmotor gDial = {kDialPinA, kDialPinB, false, true, kMotorCoast, 0, 0};
Gearmotor gCone = {kConePinA, kConePinB, false, true, kMotorCoast, 0, 0};

bool gLaserOn = false;
bool gLaserTimed = false;
uint32_t gLaserStartMs = 0;
uint32_t gLaserForMs = 0;

bool gSolenoidOn = false;
uint32_t gSolenoidStartMs = 0;
uint32_t gSolenoidForMs = 0;

uint16_t gLedCount = kLedCountDefault;
LedPattern gLedPattern = kLedOff;
uint8_t gLedR = 180;
uint8_t gLedG = 120;
uint8_t gLedB = 40;
uint8_t gLedBright = 40;
uint16_t gLedPixel = 0;
uint16_t gChaseIndex = 0;
uint32_t gChaseMs = 0;

bool gMdnsUp = false;

uint8_t clampPercent(int value) {
  if (value < 0) {
    return 0;
  }
  if (value > 100) {
    return 100;
  }
  return (uint8_t)value;
}

uint8_t clampByte(int value) {
  if (value < 0) {
    return 0;
  }
  if (value > 255) {
    return 255;
  }
  return (uint8_t)value;
}

uint8_t clampCap(int value) {
  if (value < 1) {
    return 1;
  }
  if (value > 100) {
    return 100;
  }
  return (uint8_t)value;
}

uint32_t clampRun(long value) {
  if (value < (long)kRunMsMin) {
    return kRunMsMin;
  }
  if (value > (long)kRunMsMax) {
    return kRunMsMax;
  }
  return (uint32_t)value;
}

const char *signalName(SignalKind kind) {
  if (kind == kSignalDshot150) {
    return "dshot150";
  }
  if (kind == kSignalDshot300) {
    return "dshot300";
  }
  if (kind == kSignalDshot600) {
    return "dshot600";
  }
  return "pwm";
}

SignalKind signalFromName(const String &name) {
  if (name == "dshot150") {
    return kSignalDshot150;
  }
  if (name == "dshot300") {
    return kSignalDshot300;
  }
  if (name == "dshot600") {
    return kSignalDshot600;
  }
  return kSignalPwm;
}

WheelStop wheelStopFromName(const String &name) {
  return name == "coast" ? kWheelCoast : kWheelCut;
}

MotorStop motorStopFromName(const String &name) {
  return name == "brake" ? kMotorBrake : kMotorCoast;
}

const char *ledName(LedPattern pattern) {
  if (pattern == kLedSolid) {
    return "solid";
  }
  if (pattern == kLedOne) {
    return "one";
  }
  if (pattern == kLedChase) {
    return "chase";
  }
  return "off";
}

int wheelIndexFromMotor(int motor) {
  if (motor == 1) {
    return 0;
  }
  if (motor == 2) {
    return 1;
  }
  return -1;
}

uint32_t dutyForPulse(uint16_t pulseUs) {
  const uint32_t maxDuty = (1u << kPwmBits) - 1u;
  return (uint32_t)pulseUs * maxDuty / kPwmPeriodUs;
}

uint16_t pulseForPercent(uint8_t percent) {
  const uint32_t scaled = (uint32_t)percent * gCapPercent / 100u;
  const uint32_t span = kPulseFullUs - kPulseIdleUs;
  return (uint16_t)(kPulseIdleUs + span * scaled / 100u);
}

void attachPwm(int index) {
  if (gPwmOn[index]) {
    return;
  }
  const int pin = gWheel[index].pin;
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcAttach(pin, kPwmHz, kPwmBits);
#else
  ledcSetup(index, kPwmHz, kPwmBits);
  ledcAttachPin(pin, index);
#endif
  gPwmOn[index] = true;
}

void detachPwm(int index) {
  if (!gPwmOn[index]) {
    return;
  }
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcDetach(gWheel[index].pin);
#else
  ledcDetachPin(gWheel[index].pin);
#endif
  gPwmOn[index] = false;
}

void writePulse(Flywheel &wheel, uint16_t pulseUs) {
  const int index = wheel.pin == kFlywheelPin1 ? 0 : 1;
  const uint32_t duty = dutyForPulse(pulseUs);
#if ESP_ARDUINO_VERSION_MAJOR >= 3
  ledcWrite(wheel.pin, duty);
#else
  ledcWrite(index, duty);
#endif
  wheel.pulseUs = pulseUs;
}

void sendDshot() {
  for (int i = 0; i < kFlywheelCount; ++i) {
    const uint16_t value = gWheel[i].armed ? gWheel[i].dshotValue : 0;
    dshotWrite(gWheel[i].pin, dshotFrame(value, false), gSignal);
  }
}

void cutWheel(Flywheel &wheel) {
  wheel.armed = false;
  wheel.percent = 0;
  wheel.dshotValue = 0;
  wheel.timed = false;
  wheel.coasting = false;
  wheel.pulseUs = kPulseIdleUs;
  if (gSignal == kSignalPwm) {
    writePulse(wheel, kPulseIdleUs);
  }
}

void stopWheel(Flywheel &wheel, WheelStop kind) {
  const bool canCoast = gSignal == kSignalPwm && kind == kWheelCoast &&
                        wheel.pulseUs > kPulseIdleUs;
  if (!canCoast) {
    cutWheel(wheel);
    if (gSignal != kSignalPwm) {
      sendDshot();
    }
    return;
  }
  wheel.timed = false;
  wheel.coasting = true;
  wheel.coastStartMs = millis();
  wheel.coastFromUs = wheel.pulseUs;
  wheel.stopKind = kind;
}

void useSignal(SignalKind next) {
  if (next == gSignal) {
    return;
  }
  for (int i = 0; i < kFlywheelCount; ++i) {
    if (gSignal == kSignalPwm) {
      detachPwm(i);
    } else {
      dshotRelease(gWheel[i].pin);
    }
    gWheel[i].armed = false;
    gWheel[i].percent = 0;
    gWheel[i].dshotValue = 0;
    gWheel[i].timed = false;
    gWheel[i].coasting = false;
    gWheel[i].pulseUs = kPulseIdleUs;
    pinMode(gWheel[i].pin, OUTPUT);
    digitalWrite(gWheel[i].pin, LOW);
  }
  gSignal = next;
  if (next == kSignalPwm) {
    for (int i = 0; i < kFlywheelCount; ++i) {
      attachPwm(i);
      writePulse(gWheel[i], kPulseIdleUs);
    }
    return;
  }
  sendDshot();
}

bool armWheel(Flywheel &wheel) {
  if (wheel.coasting || wheel.percent > 0 || wheel.dshotValue > 0) {
    return false;
  }
  wheel.armed = true;
  if (gSignal == kSignalPwm) {
    writePulse(wheel, kPulseIdleUs);
  } else {
    sendDshot();
  }
  return true;
}

bool setThrottle(Flywheel &wheel, uint8_t percent, uint32_t runMs, WheelStop kind) {
  if (percent == 0) {
    stopWheel(wheel, kind);
    return true;
  }
  if (!wheel.armed) {
    return false;
  }
  wheel.coasting = false;
  wheel.percent = percent;
  wheel.stopKind = kind;
  wheel.timed = true;
  wheel.runStartMs = millis();
  wheel.runForMs = runMs;
  if (gSignal == kSignalPwm) {
    writePulse(wheel, pulseForPercent(percent));
  } else {
    wheel.dshotValue = dshotFromPercent(percent, gCapPercent);
    sendDshot();
  }
  return true;
}

void serviceWheel(Flywheel &wheel) {
  if (wheel.coasting && gSignal == kSignalPwm) {
    const uint32_t gone = millis() - wheel.coastStartMs;
    if (gone >= kCoastMs) {
      cutWheel(wheel);
    } else {
      const uint32_t span = (uint32_t)wheel.coastFromUs - kPulseIdleUs;
      const uint32_t left = kCoastMs - gone;
      writePulse(wheel, (uint16_t)(kPulseIdleUs + span * left / kCoastMs));
    }
  }
  if (wheel.timed && (millis() - wheel.runStartMs) >= wheel.runForMs) {
    stopWheel(wheel, wheel.stopKind);
  }
}

void serviceDshot() {
  if (gSignal == kSignalPwm) {
    return;
  }
  if (millis() - gDshotSentMs < kDshotGapMs) {
    return;
  }
  gDshotSentMs = millis();
  sendDshot();
}

void haltMotor(Gearmotor &motor, MotorStop kind) {
  motor.running = false;
  motor.stopKind = kind;
  const bool hold = kind == kMotorBrake;
  digitalWrite(motor.pinA, hold ? HIGH : LOW);
  digitalWrite(motor.pinB, hold ? HIGH : LOW);
}

void runMotor(Gearmotor &motor, bool forward, uint32_t runMs, MotorStop kind) {
  motor.forward = forward;
  motor.stopKind = kind;
  motor.running = true;
  motor.runStartMs = millis();
  motor.runForMs = runMs;
  digitalWrite(motor.pinA, forward ? HIGH : LOW);
  digitalWrite(motor.pinB, forward ? LOW : HIGH);
}

void serviceMotor(Gearmotor &motor) {
  if (!motor.running) {
    return;
  }
  if ((millis() - motor.runStartMs) >= motor.runForMs) {
    haltMotor(motor, motor.stopKind);
  }
}

void laserOff() {
  digitalWrite(kLaserPin, LOW);
  gLaserOn = false;
  gLaserTimed = false;
}

void solenoidOff() {
  digitalWrite(kSolenoidPin, LOW);
  gSolenoidOn = false;
}

void paintLeds() {
  gStrip.setBrightness((uint8_t)((uint16_t)gLedBright * 255 / 100));
  for (uint16_t i = 0; i < kLedCountMax; ++i) {
    gStrip.setPixelColor(i, 0);
  }
  const uint32_t color = gStrip.Color(gLedR, gLedG, gLedB);
  if (gLedPattern == kLedSolid) {
    for (uint16_t i = 0; i < gLedCount; ++i) {
      gStrip.setPixelColor(i, color);
    }
  } else if (gLedPattern == kLedOne) {
    uint16_t pixel = gLedPixel;
    if (pixel >= gLedCount) {
      pixel = gLedCount - 1;
    }
    gStrip.setPixelColor(pixel, color);
  } else if (gLedPattern == kLedChase) {
    gStrip.setPixelColor(gChaseIndex % gLedCount, color);
  }
  gStrip.show();
}

void ledsOff() {
  gLedPattern = kLedOff;
  paintLeds();
}

void parkAll() {
  for (int i = 0; i < kFlywheelCount; ++i) {
    cutWheel(gWheel[i]);
  }
  if (gSignal != kSignalPwm) {
    sendDshot();
  }
  haltMotor(gDial, kMotorCoast);
  haltMotor(gCone, kMotorCoast);
  laserOff();
  solenoidOff();
  ledsOff();
}

void setupOutputs() {
  for (int i = 0; i < kFlywheelCount; ++i) {
    gWheel[i].pin = kFlywheelPins[i];
    gWheel[i].armed = false;
    gWheel[i].percent = 0;
    gWheel[i].pulseUs = kPulseIdleUs;
    gWheel[i].dshotValue = 0;
    gWheel[i].timed = false;
    gWheel[i].coasting = false;
    gWheel[i].stopKind = kWheelCoast;
    gPwmOn[i] = false;
    attachPwm(i);
    writePulse(gWheel[i], kPulseIdleUs);
  }

  pinMode(kDialPinA, OUTPUT);
  pinMode(kDialPinB, OUTPUT);
  pinMode(kConePinA, OUTPUT);
  pinMode(kConePinB, OUTPUT);
  pinMode(kLaserPin, OUTPUT);
  pinMode(kSolenoidPin, OUTPUT);
  digitalWrite(kDialPinA, LOW);
  digitalWrite(kDialPinB, LOW);
  digitalWrite(kConePinA, LOW);
  digitalWrite(kConePinB, LOW);
  digitalWrite(kLaserPin, LOW);
  digitalWrite(kSolenoidPin, LOW);

  gStrip.begin();
  gStrip.clear();
  gStrip.show();
}

void serviceLaser() {
  if (gLaserOn && gLaserTimed && (millis() - gLaserStartMs) >= gLaserForMs) {
    laserOff();
  }
}

void serviceSolenoid() {
  if (gSolenoidOn && (millis() - gSolenoidStartMs) >= gSolenoidForMs) {
    solenoidOff();
  }
}

void serviceChase() {
  if (gLedPattern != kLedChase) {
    return;
  }
  if (millis() - gChaseMs < kChaseMs) {
    return;
  }
  gChaseMs = millis();
  gChaseIndex = (uint16_t)((gChaseIndex + 1) % gLedCount);
  paintLeds();
}

String requestBody() {
  return server.hasArg("plain") ? server.arg("plain") : "";
}

void sendOk() {
  server.sendHeader("Connection", "close");
  server.send(200, "application/json", "{\"ok\":true}");
}

void sendErr(int code, const char *message) {
  String body = "{\"ok\":false,\"error\":\"";
  body += jsonEscape(String(message));
  body += "\"}";
  server.sendHeader("Connection", "close");
  server.send(code, "application/json", body);
}

void handleRoot() {
  server.sendHeader("Connection", "close");
  server.sendHeader("Cache-Control", "no-store");
  server.send_P(200, "text/html", kPageHtml);
}

void appendWheelJson(String &body, int index) {
  const Flywheel &wheel = gWheel[index];
  body += "{\"id\":";
  body += String(index + 1);
  body += ",\"gpio\":";
  body += String(wheel.pin);
  body += ",\"armed\":";
  body += wheel.armed ? "true" : "false";
  body += ",\"percent\":";
  body += String(wheel.percent);
  body += ",\"coasting\":";
  body += wheel.coasting ? "true" : "false";
  body += "}";
}

void handleStatus() {
  String body = "{\"ok\":true,\"hostname\":\"";
  body += kHost;
  body += "\",\"ip\":\"";
  body += WiFi.localIP().toString();
  body += "\",\"ssid\":\"";
  body += jsonEscape(WiFi.SSID());
  body += "\",\"flywheels\":{\"protocol\":\"";
  body += signalName(gSignal);
  body += "\",\"capPercent\":";
  body += String(gCapPercent);
  body += ",\"motors\":[";
  appendWheelJson(body, 0);
  body += ",";
  appendWheelJson(body, 1);
  body += "]},\"dial\":{\"running\":";
  body += gDial.running ? "true" : "false";
  body += ",\"forward\":";
  body += gDial.forward ? "true" : "false";
  body += "},\"cone\":{\"running\":";
  body += gCone.running ? "true" : "false";
  body += ",\"forward\":";
  body += gCone.forward ? "true" : "false";
  body += "},\"laser\":{\"on\":";
  body += gLaserOn ? "true" : "false";
  body += "},\"solenoid\":{\"on\":";
  body += gSolenoidOn ? "true" : "false";
  body += "},\"leds\":{\"mode\":\"";
  body += ledName(gLedPattern);
  body += "\",\"count\":";
  body += String(gLedCount);
  body += ",\"brightness\":";
  body += String(gLedBright);
  body += "}}";

  server.sendHeader("Connection", "close");
  server.sendHeader("Cache-Control", "no-store");
  server.send(200, "application/json", body);
}

void handleEstop() {
  parkAll();
  Serial.println("E-stop");
  sendOk();
}

void handleFlywheelSettings() {
  const String body = requestBody();
  if (jsonHas(body, "capPercent")) {
    gCapPercent = clampCap(jsonInt(body, "capPercent", gCapPercent));
  }
  if (jsonHas(body, "protocol")) {
    useSignal(signalFromName(jsonText(body, "protocol", "pwm")));
  }
  sendOk();
}

void handleFlywheelArm() {
  const int index = wheelIndexFromMotor(jsonInt(requestBody(), "motor", 0));
  if (index < 0) {
    sendErr(400, "Use motor 1 or 2");
    return;
  }
  if (!armWheel(gWheel[index])) {
    sendErr(409, "Stop the wheel before arming");
    return;
  }
  sendOk();
}

void handleFlywheelThrottle() {
  const String body = requestBody();
  const int index = wheelIndexFromMotor(jsonInt(body, "motor", 0));
  if (index < 0) {
    sendErr(400, "Use motor 1 or 2");
    return;
  }
  if (jsonHas(body, "capPercent")) {
    gCapPercent = clampCap(jsonInt(body, "capPercent", gCapPercent));
  }
  const uint8_t percent = clampPercent(jsonInt(body, "percent", 0));
  const uint32_t runMs = clampRun(jsonInt(body, "runMs", (int)kRunMsDefault));
  const WheelStop kind = wheelStopFromName(jsonText(body, "stop", "coast"));
  if (!setThrottle(gWheel[index], percent, runMs, kind)) {
    sendErr(409, "Arm this flywheel first");
    return;
  }
  sendOk();
}

void handleFlywheelStop() {
  const String body = requestBody();
  const int index = wheelIndexFromMotor(jsonInt(body, "motor", 0));
  if (index < 0) {
    sendErr(400, "Use motor 1 or 2");
    return;
  }
  stopWheel(gWheel[index], wheelStopFromName(jsonText(body, "stop", "coast")));
  sendOk();
}

void handleMotorRun(Gearmotor &motor, const char *name) {
  const String body = requestBody();
  const bool forward = jsonText(body, "direction", "forward") != "reverse";
  const uint32_t runMs = clampRun(jsonInt(body, "runMs", (int)kRunMsDefault));
  const MotorStop kind = motorStopFromName(jsonText(body, "stop", "coast"));
  runMotor(motor, forward, runMs, kind);
  Serial.printf("%s %s %lu ms\n", name, forward ? "forward" : "reverse",
                (unsigned long)runMs);
  sendOk();
}

void handleMotorStop(Gearmotor &motor) {
  const MotorStop kind =
      motorStopFromName(jsonText(requestBody(), "stop", "coast"));
  haltMotor(motor, kind);
  sendOk();
}

void handleDialRun() {
  handleMotorRun(gDial, "dial");
}

void handleDialStop() {
  handleMotorStop(gDial);
}

void handleConeRun() {
  handleMotorRun(gCone, "cone");
}

void handleConeStop() {
  handleMotorStop(gCone);
}

void handleLaser() {
  const String body = requestBody();
  if (!jsonBool(body, "on", false)) {
    laserOff();
    sendOk();
    return;
  }
  const long raw = jsonInt(body, "pulseMs", 500);
  digitalWrite(kLaserPin, HIGH);
  gLaserOn = true;
  if (raw <= 0) {
    gLaserTimed = false;
  } else {
    gLaserTimed = true;
    gLaserStartMs = millis();
    gLaserForMs = clampRun(raw);
  }
  sendOk();
}

void handleSolenoid() {
  uint32_t pulseMs = clampRun(jsonInt(requestBody(), "pulseMs", 300));
  if (pulseMs > kSolenoidMaxMs) {
    pulseMs = kSolenoidMaxMs;
  }
  digitalWrite(kSolenoidPin, HIGH);
  gSolenoidOn = true;
  gSolenoidStartMs = millis();
  gSolenoidForMs = pulseMs;
  sendOk();
}

void handleLeds() {
  const String body = requestBody();
  if (jsonHas(body, "count")) {
    int count = jsonInt(body, "count", gLedCount);
    if (count < 1) {
      count = 1;
    }
    if (count > kLedCountMax) {
      count = kLedCountMax;
    }
    gLedCount = (uint16_t)count;
  }
  if (jsonHas(body, "brightness")) {
    gLedBright = clampPercent(jsonInt(body, "brightness", gLedBright));
  }
  if (jsonHas(body, "r")) {
    gLedR = clampByte(jsonInt(body, "r", gLedR));
  }
  if (jsonHas(body, "g")) {
    gLedG = clampByte(jsonInt(body, "g", gLedG));
  }
  if (jsonHas(body, "b")) {
    gLedB = clampByte(jsonInt(body, "b", gLedB));
  }
  if (jsonHas(body, "pixel")) {
    int pixel = jsonInt(body, "pixel", 0);
    if (pixel < 0) {
      pixel = 0;
    }
    gLedPixel = (uint16_t)pixel;
  }

  const String mode = jsonText(body, "mode", "off");
  if (mode == "solid") {
    gLedPattern = kLedSolid;
  } else if (mode == "one") {
    gLedPattern = kLedOne;
  } else if (mode == "chase") {
    gLedPattern = kLedChase;
    gChaseIndex = 0;
    gChaseMs = millis();
  } else {
    gLedPattern = kLedOff;
  }
  paintLeds();
  sendOk();
}

void handleNotFound() {
  server.sendHeader("Connection", "close");
  server.send(404, "application/json", "{\"ok\":false,\"error\":\"not found\"}");
}

void serviceWifi() {
  if (WiFi.status() != WL_CONNECTED) {
    if (gMdnsUp) {
      MDNS.end();
      gMdnsUp = false;
    }
    return;
  }
  if (gMdnsUp) {
    return;
  }
  if (MDNS.begin(kHost)) {
    MDNS.addService("http", "tcp", 80);
    gMdnsUp = true;
    Serial.printf("Page: http://%s.local\n", kHost);
  }
}

bool joinWifi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.setAutoReconnect(true);
  WiFi.setHostname(kHost);

  if (strlen(WIFI_SSID) > 0) {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    const uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
      delay(250);
    }
    if (WiFi.status() == WL_CONNECTED) {
      return true;
    }
  }

  WiFiManager portal;
  portal.setHostname(kHost);
  portal.setConfigPortalTimeout(180);
  portal.setTitle("Van Gogh");
  return portal.autoConnect(kSetupAp);
}

void setup() {
  Serial.begin(115200);
#if ARDUINO_USB_CDC_ON_BOOT
  // A blocking serial timeout stalls setup when USB has no host, which
  // happens on battery. Zero keeps Wi-Fi and the page moving.
  Serial.setTxTimeoutMs(0);
#endif
  delay(200);
  Serial.println("\nVan Gogh component test");

  setupOutputs();
  Serial.println("Outputs parked");

  if (!joinWifi()) {
    Serial.println("Wi-Fi setup timed out");
  } else {
    Serial.printf("Wi-Fi %s  %s\n", WiFi.SSID().c_str(),
                  WiFi.localIP().toString().c_str());
    serviceWifi();
  }

  server.on("/", HTTP_GET, handleRoot);
  server.on("/api/status", HTTP_GET, handleStatus);
  server.on("/api/estop", HTTP_POST, handleEstop);
  server.on("/api/flywheel/settings", HTTP_POST, handleFlywheelSettings);
  server.on("/api/flywheel/arm", HTTP_POST, handleFlywheelArm);
  server.on("/api/flywheel/throttle", HTTP_POST, handleFlywheelThrottle);
  server.on("/api/flywheel/stop", HTTP_POST, handleFlywheelStop);
  server.on("/api/dial/run", HTTP_POST, handleDialRun);
  server.on("/api/dial/stop", HTTP_POST, handleDialStop);
  server.on("/api/cone/run", HTTP_POST, handleConeRun);
  server.on("/api/cone/stop", HTTP_POST, handleConeStop);
  server.on("/api/laser", HTTP_POST, handleLaser);
  server.on("/api/solenoid", HTTP_POST, handleSolenoid);
  server.on("/api/leds", HTTP_POST, handleLeds);
  server.onNotFound(handleNotFound);
  server.begin();
  Serial.println("Page ready");
}

void loop() {
  serviceWifi();
  server.handleClient();
  serviceWheel(gWheel[0]);
  serviceWheel(gWheel[1]);
  serviceDshot();
  serviceMotor(gDial);
  serviceMotor(gCone);
  serviceLaser();
  serviceSolenoid();
  serviceChase();
}
