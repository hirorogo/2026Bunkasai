// ir_tx.ino - 送信機(銃)側: NECフォーマットでID付きの弾を撃つ
// Board: Arduino UNO (ATmega328P)
// Lib:  Arduino-IRremote v4.x
//
// 注意: tone() は使わない(IRremoteと同じTimer2を奪い送信が壊れる)。
//       ブザーは「アクティブ式(5V)」を digitalWrite で鳴らす。
#include <IRremote.h>

// ---------- ピン割り当て ----------
const uint8_t PIN_TRIGGER  = 2; // トリガーSW -> GND (INPUT_PULLUP)
const uint8_t PIN_IR_SEND  = 3; // -> 1kΩ -> 2N2222ベース (IRremote規定ピン)
const uint8_t PIN_RELOAD   = 4; // リロードSW -> GND (INPUT_PULLUP)
const uint8_t PIN_SHOT_LED = 5; // 発射フラッシュ(赤) -> 330Ω -> GND
const uint8_t PIN_AMMO_LED = 6; // 残弾あり表示(緑) -> 330Ω -> GND
const uint8_t PIN_BUZZER   = 8; // アクティブブザー(+)

// ---------- ID設定 ----------
// NEC 16bitアドレス: [magic 8bit][team 2bit][player 6bit]
const uint8_t MAGIC     = 0xA5; // ir_rx.ino と同じ値
const uint8_t TEAM_ID   = 0;    // 0-3
const uint8_t PLAYER_ID = 1;    // 0-63
const uint8_t DAMAGE    = 1;    // 1-5

// ---------- ゲーム設定 ----------
const int      MAGAZINE    = 5;    // 残弾
const uint32_t FIRE_MIN_MS = 300;  // 発射間隔
const uint32_t RELOAD_MS   = 3000; // リロード所要時間

int ammo = MAGAZINE;
uint32_t lastShot = 0;
uint32_t reloadStart = 0; // 0=リロード中でない

uint16_t makeAddr() {
  return ((uint16_t)MAGIC << 8) | ((TEAM_ID & 0x03) << 6) | (PLAYER_ID & 0x3F);
}

void fire() {
  lastShot = millis();
  ammo--;
  digitalWrite(PIN_BUZZER, HIGH); // 発射音
  for (int i = 0; i < 3; i++) {   // 70ms間隔3連射で当たりやすく
    IrSender.sendNEC(makeAddr(), DAMAGE, 0);
    digitalWrite(PIN_SHOT_LED, HIGH);
    delay(70);
    digitalWrite(PIN_SHOT_LED, LOW);
  }
  digitalWrite(PIN_BUZZER, LOW);
  Serial.print("SHOT ammo="); Serial.println(ammo);
}

void setup() {
  pinMode(PIN_TRIGGER, INPUT_PULLUP);
  pinMode(PIN_RELOAD, INPUT_PULLUP);
  pinMode(PIN_SHOT_LED, OUTPUT);
  pinMode(PIN_AMMO_LED, OUTPUT);
  pinMode(PIN_BUZZER, OUTPUT);
  Serial.begin(115200);
  IrSender.begin(PIN_IR_SEND);
  Serial.println("IR TX ready");
}

void loop() {
  // リロード完了判定
  if (reloadStart && millis() - reloadStart >= RELOAD_MS) {
    reloadStart = 0;
    ammo = MAGAZINE;
    Serial.println("RELOADED");
  }

  bool reloading = reloadStart != 0;
  digitalWrite(PIN_AMMO_LED, ammo > 0 && !reloading ? HIGH : LOW);

  if (digitalRead(PIN_RELOAD) == LOW && !reloading && ammo < MAGAZINE) {
    reloadStart = millis();
    Serial.println("reloading...");
  }

  if (digitalRead(PIN_TRIGGER) == LOW && !reloading &&
      millis() - lastShot > FIRE_MIN_MS) {
    if (ammo > 0) {
      fire();
    } else {
      Serial.println("empty! reload");
    }
  }
}
