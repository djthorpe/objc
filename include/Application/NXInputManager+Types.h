/**
 * @file NXInputManager+Types.h
 * @brief Application framework input manager types
 */
#pragma once
#include <stdint.h>

///////////////////////////////////////////////////////////////////////////////
// TYPES

/**
 * @brief Input state flags used by the input manager and key handling.
 *
 * This set of bitflags describes instantaneous and latent input states for
 * buttons and keys. Flags are combined with bitwise-or to represent
 * simultaneous conditions (for example, a key that is down and also
 * generates a click event). Use bit tests to query states, e.g.:
 */
typedef enum {
  NXInputStateNone = 0,      ///< No input state set.
  NXInputStateOn = (1 << 0), ///< Key/button is currently pressed (logical on).
  NXInputStateOff =
      (1 << 1), ///< Key/button is currently released (logical off).
  NXInputStateRising = NXInputStateOn,   ///< Alias for NXInputStateOn; used to
                                         ///< indicate a rising edge.
  NXInputStateFalling = NXInputStateOff, ///< Alias for NXInputStateOff; used to
                                         ///< indicate a falling edge.
  NXInputStateClick = (1 << 2),       ///< A single click event was generated.
  NXInputStateDoubleClick = (1 << 3), ///< A double-click event was generated.
  NXInputStateTripleClick = (1 << 4), ///< A triple-click event was generated.
  NXInputStateLongClick =
      (1 << 5), ///< A long-press (long click) event was generated.
  NXInputStateRepeat = (1 << 6), ///< A repeated key/button event (auto-repeat).
  NXInputStateFunction = (1 << 7), ///< Function key or mode modifier is active.
  NXInputStateCapsLock = (1 << 8), ///< Caps Lock state is active.
  NXInputStateNumLock = (1 << 9),  ///< Num Lock state is active.
  NXInputStateScrollLock = (1 << 10), ///< Scroll Lock state is active.
  NXInputStateLeftShift = (1 << 11),  ///< Left Shift modifier is active.
  NXInputStateRightShift = (1 << 12), ///< Right Shift modifier is active.
  NXInputStateLeftMeta = (1 << 13), ///< Left Meta (Command) modifier is active.
  NXInputStateRightMeta =
      (1 << 14),                   ///< Right Meta (Command) modifier is active.
  NXInputStateLeftAlt = (1 << 15), ///< Left Option/Alt modifier is active.
  NXInputStateRightAlt = (1 << 16),    ///< Right Option/Alt modifier is active.
  NXInputStateLeftControl = (1 << 17), ///< Left Control modifier is active.
  NXInputStateRightControl = (1 << 18), ///< Right Control modifier is active.
  NXInputStateLeftWindows =
      (1 << 19), ///< Left Windows/Meta modifier is active.
  NXInputStateRightWindows =
      (1 << 20), ///< Right Windows/Meta modifier is active.
  NXInputStateShift = (NXInputStateLeftShift |
                       NXInputStateRightShift), ///< Convenience mask: either
                                                ///< left or right Shift.
  NXInputStateMeta = (NXInputStateLeftMeta |
                      NXInputStateRightMeta), ///< Convenience mask: either left
                                              ///< or right Meta (Command).
  NXInputStateAlt = (NXInputStateLeftAlt |
                     NXInputStateRightAlt), ///< Convenience mask: either left
                                            ///< or right Alt (Option).
  NXInputStateControl =
      (NXInputStateLeftControl |
       NXInputStateRightControl), ///< Convenience mask: either left or right
                                  ///< Control.
  NXInputStateWindows =
      (NXInputStateLeftWindows |
       NXInputStateRightWindows), ///< Convenience mask: either left or right
                                  ///< Windows/Meta.
} NXInputState;

///////////////////////////////////////////////////////////////////////////////
// KEYCODES

#define KEYCODE_NONE 0x0000
#define KEYCODE_ESC 0x0001
#define KEYCODE_1 0x0002
#define KEYCODE_2 0x0003
#define KEYCODE_3 0x0004
#define KEYCODE_4 0x0005
#define KEYCODE_5 0x0006
#define KEYCODE_6 0x0007
#define KEYCODE_7 0x0008
#define KEYCODE_8 0x0009
#define KEYCODE_9 0x000A
#define KEYCODE_0 0x000B
#define KEYCODE_MINUS 0x000C
#define KEYCODE_EQUAL 0x000D
#define KEYCODE_BACKSPACE 0x000E
#define KEYCODE_TAB 0x000F
#define KEYCODE_Q 0x0010
#define KEYCODE_W 0x0011
#define KEYCODE_E 0x0012
#define KEYCODE_R 0x0013
#define KEYCODE_T 0x0014
#define KEYCODE_Y 0x0015
#define KEYCODE_U 0x0016
#define KEYCODE_I 0x0017
#define KEYCODE_O 0x0018
#define KEYCODE_P 0x0019
#define KEYCODE_LEFTBRACE 0x001A
#define KEYCODE_RIGHTBRACE 0x001B
#define KEYCODE_ENTER 0x001C
#define KEYCODE_LEFTCTRL 0x001D
#define KEYCODE_A 0x001E
#define KEYCODE_S 0x001F
#define KEYCODE_D 0x0020
#define KEYCODE_F 0x0021
#define KEYCODE_G 0x0022
#define KEYCODE_H 0x0023
#define KEYCODE_J 0x0024
#define KEYCODE_K 0x0025
#define KEYCODE_L 0x0026
#define KEYCODE_SEMICOLON 0x0027
#define KEYCODE_APOSTROPHE 0x0028
#define KEYCODE_GRAVE 0x0029
#define KEYCODE_LEFTSHIFT 0x002A
#define KEYCODE_BACKSLASH 0x002B
#define KEYCODE_Z 0x002C
#define KEYCODE_X 0x002D
#define KEYCODE_C 0x002E
#define KEYCODE_V 0x002F
#define KEYCODE_B 0x0030
#define KEYCODE_N 0x0031
#define KEYCODE_M 0x0032
#define KEYCODE_COMMA 0x0033
#define KEYCODE_DOT 0x0034
#define KEYCODE_SLASH 0x0035
#define KEYCODE_RIGHTSHIFT 0x0036
#define KEYCODE_KPASTERISK 0x0037
#define KEYCODE_LEFTALT 0x0038
#define KEYCODE_SPACE 0x0039
#define KEYCODE_CAPSLOCK 0x003A
#define KEYCODE_F1 0x003B
#define KEYCODE_F2 0x003C
#define KEYCODE_F3 0x003D
#define KEYCODE_F4 0x003E
#define KEYCODE_F5 0x003F
#define KEYCODE_F6 0x0040
#define KEYCODE_F7 0x0041
#define KEYCODE_F8 0x0042
#define KEYCODE_F9 0x0043
#define KEYCODE_F10 0x0044
#define KEYCODE_NUMLOCK 0x0045
#define KEYCODE_SCROLLLOCK 0x0046
#define KEYCODE_KP7 0x0047
#define KEYCODE_KP8 0x0048
#define KEYCODE_KP9 0x0049
#define KEYCODE_KPMINUS 0x004A
#define KEYCODE_KP4 0x004B
#define KEYCODE_KP5 0x004C
#define KEYCODE_KP6 0x004D
#define KEYCODE_KPPLUS 0x004E
#define KEYCODE_KP1 0x004F
#define KEYCODE_KP2 0x0050
#define KEYCODE_KP3 0x0051
#define KEYCODE_KP0 0x0052
#define KEYCODE_KPDOT 0x0053
#define KEYCODE_F11 0x0057
#define KEYCODE_F12 0x0058
#define KEYCODE_KPENTER 0x0060
#define KEYCODE_RIGHTCTRL 0x0061
#define KEYCODE_KPSLASH 0x0062
#define KEYCODE_SYSRQ 0x0063
#define KEYCODE_RIGHTALT 0x0064
#define KEYCODE_LINEFEED 0x0065
#define KEYCODE_HOME 0x0066
#define KEYCODE_UP 0x0067
#define KEYCODE_PAGEUP 0x0068
#define KEYCODE_LEFT 0x0069
#define KEYCODE_RIGHT 0x006A
#define KEYCODE_END 0x006B
#define KEYCODE_DOWN 0x006C
#define KEYCODE_PAGEDOWN 0x006D
#define KEYCODE_INSERT 0x006E
#define KEYCODE_DELETE 0x006F
#define KEYCODE_MACRO 0x0070
#define KEYCODE_MUTE 0x0071
#define KEYCODE_VOLUMEDOWN 0x0072
#define KEYCODE_VOLUMEUP 0x0073
#define KEYCODE_POWER 0x0074
#define KEYCODE_KPEQUAL 0x0075
#define KEYCODE_KPPLUSMINUS 0x0076
#define KEYCODE_KPCOMMA 0x0079
#define KEYCODE_LEFTMETA 0x007D
#define KEYCODE_RIGHTMETA 0x007E
#define KEYCODE_SLEEP 0x008E
#define KEYCODE_WAKEUP 0x008F
#define KEYCODE_KPLEFTPAREN 0x00B3
#define KEYCODE_KPRIGHTPAREN 0x00B4
#define KEYCODE_F13 0x00B7
#define KEYCODE_F14 0x00B8
#define KEYCODE_F15 0x00B9
#define KEYCODE_F16 0x00BA
#define KEYCODE_F17 0x00BB
#define KEYCODE_F18 0x00BC
#define KEYCODE_F19 0x00BD
#define KEYCODE_F20 0x00BE
#define KEYCODE_F21 0x00BF
#define KEYCODE_F22 0x00C0
#define KEYCODE_F23 0x00C1
#define KEYCODE_F24 0x00C2
#define KEYCODE_CLOSE 0x00CE
#define KEYCODE_PLAY 0x00CF
#define KEYCODE_PRINT 0x00D2
#define KEYCODE_SEARCH 0x00D9
#define KEYCODE_CANCEL 0x00DF
#define KEYCODE_BRIGHTNESS_DOWN 0x00E0
#define KEYCODE_BRIGHTNESS_UP 0x00E1
#define KEYCODE_BRIGHTNESS_CYCLE 0x00F3
#define KEYCODE_BRIGHTNESS_AUTO 0x00F4
#define KEYCODE_BTN0 0x0100
#define KEYCODE_BTN1 0x0101
#define KEYCODE_BTN2 0x0102
#define KEYCODE_BTN3 0x0103
#define KEYCODE_BTN4 0x0104
#define KEYCODE_BTN5 0x0105
#define KEYCODE_BTN6 0x0106
#define KEYCODE_BTN7 0x0107
#define KEYCODE_BTN8 0x0108
#define KEYCODE_BTN9 0x0109
#define KEYCODE_BTNLEFT 0x0110
#define KEYCODE_BTNRIGHT 0x0111
#define KEYCODE_BTNMIDDLE 0x0112
#define KEYCODE_BTNSIDE 0x0113
#define KEYCODE_BTNEXTRA 0x0114
#define KEYCODE_BTNTOUCH 0x014A
#define KEYCODE_EJECT 0x0200
#define KEYCODE_POWER_OFF 0x0201
#define KEYCODE_POWER_ON 0x0202
#define KEYCODE_INPUT_SELECT 0x0203
#define KEYCODE_INPUT_PC 0x0204
#define KEYCODE_INPUT_VIDEO1 0x0205
#define KEYCODE_INPUT_VIDEO2 0x0206
#define KEYCODE_INPUT_VIDEO3 0x0207
#define KEYCODE_INPUT_VIDEO4 0x0208
#define KEYCODE_INPUT_VIDEO5 0x0209
#define KEYCODE_INPUT_HDMI1 0x020A
#define KEYCODE_INPUT_HDMI2 0x020B
#define KEYCODE_INPUT_HDMI3 0x020C
#define KEYCODE_INPUT_HDMI4 0x020D
#define KEYCODE_INPUT_HDMI5 0x020E
#define KEYCODE_INPUT_AUX1 0x020F
#define KEYCODE_INPUT_AUX2 0x0210
#define KEYCODE_INPUT_AUX3 0x0211
#define KEYCODE_INPUT_AUX4 0x0212
#define KEYCODE_INPUT_AUX5 0x0213
#define KEYCODE_INPUT_CD 0x0214
#define KEYCODE_INPUT_DVD 0x0215
#define KEYCODE_INPUT_PHONO 0x0216
#define KEYCODE_INPUT_TAPE1 0x0217
#define KEYCODE_INPUT_TAPE2 0x0218
#define KEYCODE_INPUT_TUNER 0x0219
#define KEYCODE_INPUT_ANALOG 0x021A
#define KEYCODE_INPUT_DIGITAL 0x021B
#define KEYCODE_INPUT_INTERNET 0x021C
#define KEYCODE_INPUT_TEXT 0x021D
#define KEYCODE_INPUT_NEXT 0x021E
#define KEYCODE_INPUT_PREV 0x021F
#define KEYCODE_VIDEO_ASPECT 0x0220
#define KEYCODE_VIDEO_PIP 0x0221
#define KEYCODE_AUDIO_MONITOR 0x0222
#define KEYCODE_CLEAR 0x0223
#define KEYCODE_TIMER 0x0224
#define KEYCODE_CHANNEL_PREV 0x0225
#define KEYCODE_CHANNEL_GUIDE 0x0226
#define KEYCODE_RECORD 0x0227
#define KEYCODE_RECORD_SPEED 0x0228
#define KEYCODE_PLAY_SPEED 0x0229
#define KEYCODE_PLAY_MODE 0x022A
#define KEYCODE_REPLAY 0x022B
#define KEYCODE_DISPLAY 0x022C
#define KEYCODE_MENU 0x022D
#define KEYCODE_INFO 0x022E
#define KEYCODE_THUMBS_UP 0x022F
#define KEYCODE_THUMBS_DOWN 0x0230
#define KEYCODE_FAVOURITE 0x0231
#define KEYCODE_BUTTON_RED 0x0232
#define KEYCODE_BUTTON_GREEN 0x0233
#define KEYCODE_BUTTON_YELLOW 0x0234
#define KEYCODE_BUTTON_BLUE 0x0235
#define KEYCODE_SEARCH_LEFT 0x0236
#define KEYCODE_SEARCH_RIGHT 0x0237
#define KEYCODE_CHAPTER_NEXT 0x0238
#define KEYCODE_CHAPTER_PREV 0x0239
#define KEYCODE_NAV_SELECT 0x023A
#define KEYCODE_SUBTITLE_TOGGLE 0x023B
#define KEYCODE_SUBTITLE_ON 0x023C
#define KEYCODE_SUBTITLE_OFF 0x023D
#define KEYCODE_STOP 0x023E
#define KEYCODE_PAUSE 0x023F
#define KEYCODE_BROWSE 0x0240
#define KEYCODE_SHUFFLE 0x0241
#define KEYCODE_REPEAT 0x0242
#define KEYCODE_KEYPAD_10PLUS 0x0243

///////////////////////////////////////////////////////////////////////////////
// FUNCTION DECLARATIONS

/**
 * Convert a keycode to a control character.
 */
static inline NXInputState keycode_to_state(uint16_t keycode) {
  switch (keycode) {
  case KEYCODE_LEFTCTRL:
    return NXInputStateLeftControl;
  case KEYCODE_RIGHTCTRL:
    return NXInputStateRightControl;
  case KEYCODE_LEFTSHIFT:
    return NXInputStateLeftShift;
  case KEYCODE_RIGHTSHIFT:
    return NXInputStateRightShift;
  case KEYCODE_LEFTALT:
    return NXInputStateLeftAlt;
  case KEYCODE_RIGHTALT:
    return NXInputStateRightAlt;
  case KEYCODE_LEFTMETA:
    return NXInputStateLeftMeta;
  case KEYCODE_RIGHTMETA:
    return NXInputStateRightMeta;
  case KEYCODE_CAPSLOCK:
    return NXInputStateCapsLock;
  case KEYCODE_NUMLOCK:
    return NXInputStateNumLock;
  case KEYCODE_SCROLLLOCK:
    return NXInputStateScrollLock;
  default:
    return NXInputStateNone;
  }
}

/**
 * Convert a keycode to its one-byte character representation.
 */
static inline char keycode_to_char(uint16_t keycode) {
  switch (keycode) {
  case KEYCODE_1:
    return '1';
  case KEYCODE_2:
    return '2';
  case KEYCODE_3:
    return '3';
  case KEYCODE_4:
    return '4';
  case KEYCODE_5:
    return '5';
  case KEYCODE_6:
    return '6';
  case KEYCODE_7:
    return '7';
  case KEYCODE_8:
    return '8';
  case KEYCODE_9:
    return '9';
  case KEYCODE_0:
    return '0';
  case KEYCODE_MINUS:
    return '-';
  case KEYCODE_EQUAL:
    return '=';
  case KEYCODE_TAB:
    return '\t';
  case KEYCODE_Q:
    return 'Q';
  case KEYCODE_W:
    return 'W';
  case KEYCODE_E:
    return 'E';
  case KEYCODE_R:
    return 'R';
  case KEYCODE_T:
    return 'T';
  case KEYCODE_Y:
    return 'Y';
  case KEYCODE_U:
    return 'U';
  case KEYCODE_I:
    return 'I';
  case KEYCODE_O:
    return 'O';
  case KEYCODE_P:
    return 'P';
  case KEYCODE_LEFTBRACE:
    return '[';
  case KEYCODE_RIGHTBRACE:
    return ']';
  case KEYCODE_ENTER:
    return '\n';
  case KEYCODE_A:
    return 'A';
  case KEYCODE_S:
    return 'S';
  case KEYCODE_D:
    return 'D';
  case KEYCODE_F:
    return 'F';
  case KEYCODE_G:
    return 'G';
  case KEYCODE_H:
    return 'H';
  case KEYCODE_J:
    return 'J';
  case KEYCODE_K:
    return 'K';
  case KEYCODE_L:
    return 'L';
  case KEYCODE_SEMICOLON:
    return ';';
  case KEYCODE_APOSTROPHE:
    return '\'';
  case KEYCODE_GRAVE:
    return '`';
  case KEYCODE_BACKSLASH:
    return '\\';
  case KEYCODE_Z:
    return 'Z';
  case KEYCODE_X:
    return 'X';
  case KEYCODE_C:
    return 'C';
  case KEYCODE_V:
    return 'V';
  case KEYCODE_B:
    return 'B';
  case KEYCODE_N:
    return 'N';
  case KEYCODE_M:
    return 'M';
  case KEYCODE_COMMA:
    return ',';
  case KEYCODE_DOT:
    return '.';
  case KEYCODE_SLASH:
    return '/';
  case KEYCODE_KPASTERISK:
    return '*';
  case KEYCODE_SPACE:
    return ' ';
  case KEYCODE_KP7:
    return '7';
  case KEYCODE_KP8:
    return '8';
  case KEYCODE_KP9:
    return '9';
  case KEYCODE_KPMINUS:
    return '-';
  case KEYCODE_KP4:
    return '4';
  case KEYCODE_KP5:
    return '5';
  case KEYCODE_KP6:
    return '6';
  case KEYCODE_KPPLUS:
    return '+';
  case KEYCODE_KP1:
    return '1';
  case KEYCODE_KP2:
    return '2';
  case KEYCODE_KP3:
    return '3';
  case KEYCODE_KP0:
    return '0';
  case KEYCODE_KPDOT:
    return '.';
  case KEYCODE_KPENTER:
    return '\n';
  case KEYCODE_KPSLASH:
    return '/';
  case KEYCODE_LINEFEED:
    return '\r';
  case KEYCODE_KPCOMMA:
    return ',';
  case KEYCODE_KPLEFTPAREN:
    return '(';
  case KEYCODE_KPRIGHTPAREN:
    return ')';
  default:
    return 0;
  }
}