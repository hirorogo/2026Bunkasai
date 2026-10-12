// ir_beep.ino - 最小構成: 何かIRが来たらブザーが鳴る
// Board: Arduino UNO。ライブラリ不要。
// 用途: 配線と受光モジュールの生死確認(Phase 0の最初の1歩)。
//       プロトコル判別はしないので、家電リモコンでもエアコンでも鳴る。
//       ゲーム用の被弾判定は ir_rx.ino を使うこと。
// 配線: 受光モジュール OUT -> D2, VCC -> 5V, GND -> GND
//       アクティブブザー(+) -> D8, (-) -> GND

const int SENSOR_PIN = 2; // 受光モジュール OUT (アクティブLow)
const int BUZZER_PIN = 8; // アクティブブザー(+)

bool isBuzzerActive = false;
unsigned long buzzerStartTime = 0;
const unsigned long BUZZER_DURATION = 500; // 0.5秒鳴らす

void setup() {
  pinMode(SENSOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
}

void loop() {
  unsigned long now = millis();

  if (digitalRead(SENSOR_PIN) == LOW && !isBuzzerActive) {
    digitalWrite(BUZZER_PIN, HIGH);
    isBuzzerActive = true;
    buzzerStartTime = now;
  }
  if (isBuzzerActive && now - buzzerStartTime >= BUZZER_DURATION) {
    digitalWrite(BUZZER_PIN, LOW);
    isBuzzerActive = false;
  }
}
