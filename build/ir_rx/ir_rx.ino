// ir_rx.ino - 受信機(的)側: 被弾判定 + HP管理 + 演出
// Board: Arduino UNO (ATmega328P)
// Lib:  Arduino-IRremote v4.x (Library Managerで "IRremote" を検索して導入)
//
// 弾の判定は2系統:
//  - 自作銃モード: addr上位8bit==MAGIC かつ 自チームでない -> 被弾
//  - リモコンMVPモード: addr/cmd が GUN_ADDRESS / GUN_COMMAND に一致 -> 被弾
//    (ir_dump.ino で実測した値を下に書き込む)
//
// 注意: tone() は使わない。UNOではIRremoteと同じTimer2を奪い受信が壊れる。
//       ブザーは「アクティブ式(5V)」を digitalWrite で鳴らす。
#include <IRremote.h>

// ---------- ピン割り当て ----------
const uint8_t PIN_IR      = 2; // 受光モジュール OUT (複数個は直接束ねてOK)
const uint8_t PIN_HIT_LED = 5; // 被弾/死亡表示LED -> 330Ω -> GND
const uint8_t PIN_VIBE    = 6; // 振動モーター(2N2222経由・省略可)
const uint8_t PIN_BUZZER  = 8; // アクティブブザー(+)

// ---------- ゲーム設定 ----------
const uint8_t  MAGIC = 0xA5;      // 自作弾の識別子 (ir_tx.ino と同じ値)
const uint8_t  MY_TEAM = 0;       // 自チームID 0-3 (味方撃ち無効)
const int      MAX_HP = 5;
const uint32_t INVINCIBLE_MS = 3000; // 被弾後の無敵時間
const uint32_t HIT_FX_MS = 300;      // 被弾演出の長さ

// ---------- リモコンMVP用 (ir_dumpで実測して書き換える) ----------
const uint16_t GUN_ADDRESS = 0x0000; // TODO: 実測値
const uint8_t  GUN_COMMAND = 0x00;   // TODO: 実測値

int hp = MAX_HP;
uint32_t invincibleUntil = 0;
uint32_t fxOffAt = 0;

// この addr/cmd は「自分を倒せる弾」か?
bool isBullet(uint16_t addr, uint8_t cmd) {
  if ((addr >> 8) == MAGIC) {
    uint8_t team = (addr >> 6) & 0x03;
    return team != MY_TEAM; // 自作弾: 敵チームのみ有効
  }
  return addr == GUN_ADDRESS && cmd == GUN_COMMAND; // リモコン弾
}

void fxOn() {
  digitalWrite(PIN_HIT_LED, HIGH);
  digitalWrite(PIN_VIBE, HIGH);
  digitalWrite(PIN_BUZZER, HIGH);
  fxOffAt = millis() + HIT_FX_MS;
}

void fxOff() {
  digitalWrite(PIN_VIBE, LOW);
  digitalWrite(PIN_BUZZER, LOW);
  if (hp > 0) digitalWrite(PIN_HIT_LED, LOW); // 死亡時は点灯しっぱなし
}

void setup() {
  pinMode(PIN_HIT_LED, OUTPUT);
  pinMode(PIN_VIBE, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(115200);
  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK);
  Serial.print("IR RX ready. HP="); Serial.println(hp);
}

void loop() {
  // 演出の消灯 (delay()を使わない)
  if (fxOffAt && millis() >= fxOffAt) {
    fxOff();
    fxOffAt = 0;
  }

  if (IrReceiver.decode()) {
    uint16_t addr = IrReceiver.decodedIRData.address;
    uint8_t cmd = IrReceiver.decodedIRData.command;
    Serial.print("RECV addr=0x"); Serial.print(addr, HEX);
    Serial.print(" cmd=0x"); Serial.println(cmd, HEX);

    if (hp > 0 && isBullet(addr, cmd) && millis() > invincibleUntil) {
      bool selfMade = (addr >> 8) == MAGIC;
      uint8_t dmg = selfMade ? ((cmd >= 1 && cmd <= 5) ? cmd : 1) : 1;
      hp -= dmg;
      invincibleUntil = millis() + INVINCIBLE_MS;
      fxOn();
      Serial.print("HIT! dmg="); Serial.print(dmg);
      Serial.print(" HP="); Serial.println(hp);
      if (hp <= 0) {
        Serial.println("DEAD - 電源入れ直しでリスポーン");
        digitalWrite(PIN_HIT_LED, HIGH); // 死亡表示: LED点灯維持
      }
    }
    IrReceiver.resume();
  }
}
