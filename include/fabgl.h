#pragma once

#include <array>
#include <cstdint>
#include <deque>
#include <cstring>

// Global mouse delta type for legacy code compatibility
struct MouseButtons { bool left; bool right; };
struct MouseDelta { int16_t deltaX; int16_t deltaY; MouseButtons buttons; };

namespace fabgl {

enum VirtualKey {
  VK_NONE,

  VK_SPACE,

  VK_0,
  VK_1,
  VK_2,
  VK_3,
  VK_4,
  VK_5,
  VK_6,
  VK_7,
  VK_8,
  VK_9,
  VK_KP_0,
  VK_KP_1,
  VK_KP_2,
  VK_KP_3,
  VK_KP_4,
  VK_KP_5,
  VK_KP_6,
  VK_KP_7,
  VK_KP_8,
  VK_KP_9,

  VK_a,
  VK_b,
  VK_c,
  VK_d,
  VK_e,
  VK_f,
  VK_g,
  VK_h,
  VK_i,
  VK_j,
  VK_k,
  VK_l,
  VK_m,
  VK_n,
  VK_o,
  VK_p,
  VK_q,
  VK_r,
  VK_s,
  VK_t,
  VK_u,
  VK_v,
  VK_w,
  VK_x,
  VK_y,
  VK_z,
  VK_A,
  VK_B,
  VK_C,
  VK_D,
  VK_E,
  VK_F,
  VK_G,
  VK_H,
  VK_I,
  VK_J,
  VK_K,
  VK_L,
  VK_M,
  VK_N,
  VK_O,
  VK_P,
  VK_Q,
  VK_R,
  VK_S,
  VK_T,
  VK_U,
  VK_V,
  VK_W,
  VK_X,
  VK_Y,
  VK_Z,

  VK_GRAVEACCENT,
  VK_ACUTEACCENT,
  VK_QUOTE,
  VK_QUOTEDBL,
  VK_EQUALS,
  VK_MINUS,
  VK_KP_MINUS,
  VK_PLUS,
  VK_KP_PLUS,
  VK_KP_MULTIPLY,
  VK_ASTERISK,
  VK_BACKSLASH,
  VK_KP_DIVIDE,
  VK_SLASH,
  VK_KP_PERIOD,
  VK_PERIOD,
  VK_COLON,
  VK_COMMA,
  VK_SEMICOLON,
  VK_AMPERSAND,
  VK_VERTICALBAR,
  VK_HASH,
  VK_AT,
  VK_CARET,
  VK_DOLLAR,
  VK_POUND,
  VK_EURO,
  VK_PERCENT,
  VK_EXCLAIM,
  VK_QUESTION,
  VK_LEFTBRACE,
  VK_RIGHTBRACE,
  VK_LEFTBRACKET,
  VK_RIGHTBRACKET,
  VK_LEFTPAREN,
  VK_RIGHTPAREN,
  VK_LESS,
  VK_GREATER,
  VK_UNDERSCORE,
  VK_DEGREE,
  VK_SECTION,
  VK_TILDE,
  VK_NEGATION,

  VK_LSHIFT,
  VK_RSHIFT,
  VK_LALT,
  VK_RALT,
  VK_LCTRL,
  VK_RCTRL,
  VK_LGUI,
  VK_RGUI,

  VK_ESCAPE,

  VK_PRINTSCREEN,
  VK_SYSREQ,

  VK_INSERT,
  VK_KP_INSERT,
  VK_DELETE,
  VK_KP_DELETE,
  VK_BACKSPACE,
  VK_HOME,
  VK_KP_HOME,
  VK_END,
  VK_KP_END,
  VK_PAUSE,
  VK_BREAK,
  VK_SCROLLLOCK,
  VK_NUMLOCK,
  VK_CAPSLOCK,
  VK_TAB,
  VK_RETURN,
  VK_KP_ENTER,
  VK_APPLICATION,
  VK_PAGEUP,
  VK_KP_PAGEUP,
  VK_PAGEDOWN,
  VK_KP_PAGEDOWN,
  VK_UP,
  VK_KP_UP,
  VK_DOWN,
  VK_KP_DOWN,
  VK_LEFT,
  VK_KP_LEFT,
  VK_RIGHT,
  VK_KP_RIGHT,
  VK_KP_CENTER,

  VK_F1,
  VK_F2,
  VK_F3,
  VK_F4,
  VK_F5,
  VK_F6,
  VK_F7,
  VK_F8,
  VK_F9,
  VK_F10,
  VK_F11,
  VK_F12,

  VK_GRAVE_a,
  VK_GRAVE_e,
  VK_GRAVE_i,
  VK_GRAVE_o,
  VK_GRAVE_u,
  VK_GRAVE_y,

  VK_ACUTE_a,
  VK_ACUTE_e,
  VK_ACUTE_i,
  VK_ACUTE_o,
  VK_ACUTE_u,
  VK_ACUTE_y,

  VK_GRAVE_A,
  VK_GRAVE_E,
  VK_GRAVE_I,
  VK_GRAVE_O,
  VK_GRAVE_U,
  VK_GRAVE_Y,

  VK_ACUTE_A,
  VK_ACUTE_E,
  VK_ACUTE_I,
  VK_ACUTE_O,
  VK_ACUTE_U,
  VK_ACUTE_Y,

  VK_UMLAUT_a,
  VK_UMLAUT_e,
  VK_UMLAUT_i,
  VK_UMLAUT_o,
  VK_UMLAUT_u,
  VK_UMLAUT_y,

  VK_UMLAUT_A,
  VK_UMLAUT_E,
  VK_UMLAUT_I,
  VK_UMLAUT_O,
  VK_UMLAUT_U,
  VK_UMLAUT_Y,

  VK_CARET_a,
  VK_CARET_e,
  VK_CARET_i,
  VK_CARET_o,
  VK_CARET_u,
  VK_CARET_y,

  VK_CARET_A,
  VK_CARET_E,
  VK_CARET_I,
  VK_CARET_O,
  VK_CARET_U,
  VK_CARET_Y,

  VK_CEDILLA_c,
  VK_CEDILLA_C,

  VK_TILDE_a,
  VK_TILDE_o,
  VK_TILDE_n,

  VK_TILDE_A,
  VK_TILDE_O,
  VK_TILDE_N,

  VK_UPPER_a,
  VK_ESZETT,
  VK_EXCLAIM_INV,
  VK_QUESTION_INV,
  VK_INTERPUNCT,
  VK_DIAERESIS,
  VK_SQUARE,
  VK_CURRENCY,
  VK_MU,

  VK_aelig,
  VK_oslash,
  VK_aring,

  VK_AELIG,
  VK_OSLASH,
  VK_ARING,

  VK_YEN,
  VK_MUHENKAN,
  VK_HENKAN,
  VK_KATAKANA_HIRAGANA_ROMAJI,
  VK_HANKAKU_ZENKAKU_KANJI,
  VK_SHIFT_0,

  VK_JOY1LEFT,
  VK_JOY1RIGHT,
  VK_JOY1UP,
  VK_JOY1DOWN,
  VK_JOY1START,
  VK_JOY1MODE,
  VK_JOY1A,
  VK_JOY1B,
  VK_JOY1C,
  VK_JOY1X,
  VK_JOY1Y,
  VK_JOY1Z,
  VK_JOY2LEFT,
  VK_JOY2RIGHT,
  VK_JOY2UP,
  VK_JOY2DOWN,
  VK_JOY2START,
  VK_JOY2MODE,
  VK_JOY2A,
  VK_JOY2B,
  VK_JOY2C,
  VK_JOY2X,
  VK_JOY2Y,
  VK_JOY2Z,

  VK_KEMPSTON_RIGHT,
  VK_KEMPSTON_LEFT,
  VK_KEMPSTON_DOWN,
  VK_KEMPSTON_UP,
  VK_KEMPSTON_FIRE,
  VK_KEMPSTON_ALTFIRE,

  VK_FULLER_RIGHT,
  VK_FULLER_LEFT,
  VK_FULLER_DOWN,
  VK_FULLER_UP,
  VK_FULLER_FIRE,

  VK_VOLUMEUP,
  VK_VOLUMEDOWN,
  VK_VOLUMEMUTE,

  VK_ASCII,
  VK_LAST,
};

struct VirtualKeyItem {
  VirtualKey vk;
  uint8_t down;
  uint8_t scancode[8];
  uint8_t ASCII;
  uint8_t CTRL : 1;
  uint8_t LALT : 1;
  uint8_t RALT : 1;
  uint8_t SHIFT : 1;
  uint8_t GUI : 1;
  uint8_t CAPSLOCK : 1;
  uint8_t NUMLOCK : 1;
  uint8_t SCROLLLOCK : 1;

  VirtualKeyItem()
    : vk(VK_NONE), down(0), ASCII(0), CTRL(0), LALT(0), RALT(0), SHIFT(0), GUI(0), CAPSLOCK(0), NUMLOCK(0), SCROLLLOCK(0)
  {
    std::memset(scancode, 0, sizeof(scancode));
  }
};

enum PS2Preset {
  zxKeyb,
  KeyboardPort0,
  MousePort0,
  KeyboardPort0_KeybJoystickPort1,
  KeyboardPort0_MousePort1,
};

enum KbdMode {
  CreateVirtualKeysQueue,
};

class Keyboard {
public:
  Keyboard() {
    m_keyState.fill(false);
  }

  // No real keyboard available in this build (stubbed)
  bool isKeyboardAvailable() const { return false; }
  bool virtualKeyAvailable() const { return !m_queue.empty(); }
  bool getNextVirtualKey(VirtualKeyItem * item) {
    if (m_queue.empty()) {
      return false;
    }
    *item = m_queue.front();
    m_queue.pop_front();
    return true;
  }
  bool isVKDown(VirtualKey vk) const {
    auto index = static_cast<size_t>(vk);
    if (index >= m_keyState.size()) {
      return false;
    }
    return m_keyState[index];
  }
  void injectVirtualKey(VirtualKey vk, bool down, bool ctrl = false, bool alt = false, bool shift = false, bool gui = false, bool caps = false) {
    VirtualKeyItem item;
    item.vk = vk;
    item.down = down ? 1 : 0;
    item.ASCII = 0;
    item.CTRL = ctrl ? 1 : 0;
    item.LALT = alt ? 1 : 0;
    item.RALT = 0;
    item.SHIFT = shift ? 1 : 0;
    item.GUI = gui ? 1 : 0;
    item.CAPSLOCK = caps ? 1 : 0;
    item.NUMLOCK = 0;
    item.SCROLLLOCK = 0;
    std::memset(item.scancode, 0, sizeof(item.scancode));
    m_queue.push_back(item);
    if (static_cast<size_t>(vk) < m_keyState.size()) {
      m_keyState[static_cast<size_t>(vk)] = down;
    }
  }
  void emptyVirtualKeyQueue() { m_queue.clear(); }
  void setLEDs(bool, bool, bool) {}
  void setScancodeSet(int) {}

private:
  std::deque<VirtualKeyItem> m_queue;
  std::array<bool, 256> m_keyState;
};

using KeybJoystick = Keyboard;

class Mouse {
public:
  bool isMouseAvailable() const { return false; }
  void setResolution(int) {}
  void setSampleRate(int) {}
  void setScaling(int) {}
  // Mouse delta access used by Ports.cpp
  struct MouseButtons { bool left; bool right; };
  struct MouseDelta { int16_t deltaX; int16_t deltaY; MouseButtons buttons; };
  bool deltaAvailable() const { return false; }
  bool getNextDelta(::MouseDelta* d) { if (d) { d->deltaX = 0; d->deltaY = 0; d->buttons.left = false; d->buttons.right = false; } return false; }
};

class PS2Controller {
public:
  PS2Controller() = default;
  void begin(PS2Preset, KbdMode) {}
  Keyboard * keyboard() { return &m_keyboard; }
  KeybJoystick * keybjoystick() { return &m_keybjoystick; }
  Mouse * mouse() { return &m_mouse; }
  void end() {}
  void disableRX(int) {}
  void enableRX(int) {}

private:
  Keyboard m_keyboard;
  KeybJoystick m_keybjoystick;
  Mouse m_mouse;
};

} // namespace fabgl

// Provide unqualified aliases in the global namespace for code that expects them
using PS2Preset = fabgl::PS2Preset;
using KbdMode = fabgl::KbdMode;
