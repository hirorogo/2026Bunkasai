// ir_tx_test.ino - 文化祭IRタグ 銃側 最小送信テスト
// Board: Arduino Nano Every / Nano V3 (ATmega328P 5V)
// Libs: Arduino-IRremote (z3t0版, v4.x), NewTone(任意)
// 配線: D3 -> [1k] -> 2N2222 B, 5V->[10Ω]->IR LED1->IR LED2->2N2222 C, E->GND
//       D2 <- トリガーSW <- GND (INPUT_PULLUP), D8 -> ブザー, D5 -> 発射LED
#include <IRremote.h>

const uint8_t PIN_TRIGGER = 2;
const uint8_t PIN_BUZZER = 8;
const uint8_t PIN_SHOT_LED = 5;

// チーム2bit + プレイヤー6bitをアドレスに格納 (NEC 16bit addr)
const uint8_t TEAM_ID = 0;   // 0-3 DIPで切替想定
const uint8_t PLAYER_ID = 1; // 1-63
const uint8_t DAMAGE = 1;

uint16_t makeAddr() { return ((TEAM_ID & 0x03) << 6) | (PLAYER_ID & 0x3F); }

void setup() {
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_SHOT_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(115200);
  IrSender.begin(3); // D3 PWM送信
  Serial.println("IR TX ready");
}

void loop() {
  static uint32_t lastShot = 0;
  if (digitalRead(PIN_TRIGGER) == LOW && millis() - lastShot > 300) {
    lastShot = millis();
    uint16_t addr = makeAddr();
    // 1発70ms間隔で3連射: 教室蛍光灯下でも当たりやすく
    for (int i = 0; i < 3; i++) {
      IrSender.sendNEC(addr, DAMAGE, 0);
      digitalWrite(PIN_SHOT_LED, HIGH);
      tone(PIN_BUZZER, 2000, 60);
      delay(70);
      digitalWrite(PIN_SHOT_LED, LOW);
    }
    Serial.print("SHOT addr=0x"); Serial.println(addr, HEX);
  }
}
