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
    digitalWrite(PIN_BUZZER, HIGH); // 受光検知: ブザー発音開始
    delay(2500);                    // 2.5秒間鳴り続ける
    digitalWrite(PIN_BUZZER, LOW);  // ブザー停止
  }
}
