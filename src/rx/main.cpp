#include <Arduino.h>

const int PIN_IR_RECEIVER = 3; // 受光モジュール OUT 端子 (アクティブLOW)
const int PIN_BUZZER      = 5; // アクティブブザー (+) 端子 (アクティブHIGH)

void setup() {
  pinMode(PIN_IR_RECEIVER, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
}

void loop() {
  if (digitalRead(PIN_IR_RECEIVER) == LOW) {
    digitalWrite(PIN_BUZZER, HIGH); // 受光中: ブザー発音
  } else {
    digitalWrite(PIN_BUZZER, LOW);  // 非受光: ブザー停止
  }
}
