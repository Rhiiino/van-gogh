#pragma once

#include <Arduino.h>

inline String jsonEscape(const String &value) {
  String out;
  out.reserve(value.length() + 8);
  for (unsigned int i = 0; i < value.length(); ++i) {
    const char c = value[i];
    if (c == '\\' || c == '"') {
      out += '\\';
      out += c;
    } else if (c == '\n') {
      out += "\\n";
    } else {
      out += c;
    }
  }
  return out;
}

inline bool jsonHas(const String &body, const char *key) {
  return body.indexOf(String("\"") + key + "\"") >= 0;
}

inline int jsonInt(const String &body, const char *key, int fallback) {
  const int keyAt = body.indexOf(String("\"") + key + "\"");
  if (keyAt < 0) {
    return fallback;
  }
  const int colon = body.indexOf(':', keyAt);
  if (colon < 0) {
    return fallback;
  }
  return body.substring(colon + 1).toInt();
}

inline bool jsonBool(const String &body, const char *key, bool fallback) {
  const int keyAt = body.indexOf(String("\"") + key + "\"");
  if (keyAt < 0) {
    return fallback;
  }
  const int colon = body.indexOf(':', keyAt);
  if (colon < 0) {
    return fallback;
  }
  String tail = body.substring(colon + 1);
  tail.trim();
  if (tail.startsWith("true")) {
    return true;
  }
  if (tail.startsWith("false")) {
    return false;
  }
  return fallback;
}

inline String jsonText(const String &body, const char *key, const String &fallback) {
  const int keyAt = body.indexOf(String("\"") + key + "\"");
  if (keyAt < 0) {
    return fallback;
  }
  const int colon = body.indexOf(':', keyAt);
  if (colon < 0) {
    return fallback;
  }
  const int open = body.indexOf('"', colon + 1);
  if (open < 0) {
    return fallback;
  }
  const int close = body.indexOf('"', open + 1);
  if (close < 0) {
    return fallback;
  }
  return body.substring(open + 1, close);
}
