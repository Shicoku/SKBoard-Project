#include <BleKeyboard.h>

BleKeyboard bleKeyboard("SKBorad", "Shicoku");

// ==== Rotary Encoder ====

const int pinA1 = 8;
const int pinB1 = 20;

volatile long pos1 = 0;

void IRAM_ATTR handle1A() {
  int A = digitalRead(pinA1);
  int B = digitalRead(pinB1);
  if (A == B) pos1++;
  else pos1--;
}

const unit8_t encoderMap[2] = {"'", "-"} // left, right

// ==== Key Matrix ====

#define MOD_CTRL KEY_LEFT_CTRL
#define MOD_SHIFT KEY_LEFT_SHIFT
#define MOD_ALT KEY_LEFT_ALT
#define MOD_GUI KEY_LEFT_GUI

struct Shortcut {
  uint8_t modifiers;
  uint8_t key;
};

const int rowNum = 4;
const int colNum = 3;

const int rowPin[rowNum] = { 2, 3, 4, 5 };
const int colPin[colNum] = { 6, 7, 21 };

// const byte keyMap[rowNum][colNum] = {
//   { 0x61, 0x62, 0x63 }, // 1 5 9
//   { 0x64, 0x65, 0x00 }, // 2 6 10
//   { 0x68, 0x69, 0x00 }, // 3 7 11
//   { 0x71, 0x72, 0x00 }, // 4 8 12
// };

const Shortcut keyMap[rowNum][colNum] = {
  { { MOD_GUI, 0 }, { MOD_GUI, 'z' }, { MOD_CTRL, 'c' } },  // 1 5 9
  { { 0, 'a' }, { MOD_GUI, 'y' }, { 0, 0 } },               // 2 6 10
  { { 0, 'e' }, { MOD_GUI, 's' }, { 0, 0 } },               // 3 7 11
  { { 0, 'm' }, { MOD_CTRL, 'r' }, { 0, 0 } }               // 4 8 12
};


bool currentState[rowNum][colNum];
bool beforeState[rowNum][colNum];

int i, j;

void setup() {
  // put your setup code here, to run once:

  bleKeyboard.begin();

  pinMode(pinA1, INPUT);
  pinMode(pinB1, INPUT);

  attachInterrupt(digitalPinToInterrupt(pinA1), handle1A, CHANGE);

  for (auto &&pin : rowPin)
    pinMode(pin, OUTPUT);
  for (auto &&pin : colPin)
    pinMode(pin, INPUT_PULLUP);

  for (i = 0; i < rowNum; i++) {
    for (j = 0; j < colNum; j++) {
      currentState[i][j] = HIGH;
      beforeState[i][j] = HIGH;
    }
    digitalWrite(rowPin[i], HIGH);
  }
}

long old1 = 0;
long old2 = 0;

void loop() {
  // put your main code here, to run repeatedly:

  // ==== Rotary Encoder ====
  if (pos1 != old1) {
    if (pos1 > old1) {
      bleKeyboard.print(encoderMap[1]);  // Right
    } else {
      bleKeyboard.print(encoderMap[0]);  // Left
    }
    old1 = pos1;
    bleKeyboard.releaseAll();
  }

  // ==== KeyBoard ====
  for (i = 0; i < rowNum; i++) {
    digitalWrite(rowPin[i], LOW);

    for (j = 0; j < colNum; j++) {
      currentState[i][j] = digitalRead(colPin[j]);

      if (currentState[i][j] != beforeState[i][j]) {

        if (currentState[i][j] == LOW) {
          // bleKeyboard.press(keyMap[i][j]);
          if (keyMap[i][j].modifiers != 0)
            bleKeyboard.press(keyMap[i][j].modifiers);

          if (keyMap[i][j].key != 0)
            bleKeyboard.press(keyMap[i][j].key);
        } else {
          // bleKeyboard.release(keyMap[i][j]);
          if (keyMap[i][j].key != 0)
            bleKeyboard.release(keyMap[i][j].key);

          if (keyMap[i][j].modifiers != 0)
            bleKeyboard.release(keyMap[i][j].modifiers);
        }
        beforeState[i][j] = currentState[i][j];
      }
    }
    digitalWrite(rowPin[i], HIGH);
  }

  delay(10);
}
