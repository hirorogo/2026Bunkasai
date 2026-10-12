// ir_rx_test.ino - 文化祭IRタグ 受信側 最小被弾テスト
// Board: Arduino Nano Every / Nano V3
// 配線: D2 <- VS1838B OUT(前), VCC->5V GND->GND (VCC-GND間に4.7uF)
//       D6 -> WS2812 DIN (別5V, 330Ω直列推奨), D8 -> ブザー
#include <IRremote.h>

const uint8_t PIN_IR = 2;
const uint8_t PIN_BUZZER = 8;
const uint8_t MY_TEAM = 0; // 自チーム: 同じaddr上位2bitは無視(味方撃ち無効)
int hp = 5;
uint32_t invincibleUntil = 0;

void hitEffect() {
  tone(PIN_BUZZER, 880, 300);
  Serial.println("HIT!");
}

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(115200);
  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK);
  Serial.println("IR RX ready. HP=5");
}

void loop() {
  if (IrReceiver.decode()) {
    uint16_t addr = IrReceiver.decodedIRData.address;
    uint8_t cmd = IrReceiver.decodedIRData.command;
    uint8_t team = (addr >> 6) & 0x03;
    Serial.print("RECV addr=0x"); Serial.print(addr, HEX);
    Serial.print(" team="); Serial.print(team);
    Serial.print(" dmg="); Serial.println(cmd);
    if (team != MY_TEAM && millis() > invincibleUntil && hp > 0) {
      hp -= cmd > 0 ? cmd : 1;
      invincibleUntil = millis() + 3000; // 3秒無敵
      hitEffect();
      Serial.print("HP="); Serial.println(hp);
      if (hp <= 0) Serial.println("DEAD - respawn in 10s (self-array touch)");
    }
    IrReceiver.resume();
  }
}
