#pragma once
#include <stdbool.h>
#include <stdint.h>

static inline bool _char_isWhitespace(uint8_t c) {
  return (c == ' ') || (c == '\t') || (c == '\n') || (c == '\r');
}

static inline bool _char_isDigit(uint8_t c) { return (c >= '0' && c <= '9'); }

static inline bool _char_isAlpha(uint8_t c) {
  return ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z'));
}

static inline uint8_t _char_toUpper(uint8_t c) {
  return (c >= 'a' && c <= 'z') ? (c - 'a' + 'A') : c;
}

static inline uint8_t _char_toLower(uint8_t c) {
  return (c >= 'A' && c <= 'Z') ? (c - 'A' + 'a') : c;
}

static inline uint8_t _char_toPrintable(uint8_t c) {
  return (c >= 32 && c <= 126) ? c : '.';
}
