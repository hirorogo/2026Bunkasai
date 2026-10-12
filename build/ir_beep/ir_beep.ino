// ir_beep.ino - シンプル版: リモコンの信号が来たらブザーが鳴る
// Board: Arduino UNO / Lib: Arduino-IRremote v4.x
// ノイズや長押しリピートでは鳴らない。エアコン等の独自フォーマット
// (UNKNOWNになる)も、ビット数が十分あれば本物とみなして鳴らす。
// 「どのボタンでも鳴る」確認用。特定ボタン限定のゲーム版は ir_rx.ino。
// 配線: 受光モジュール OUT -> D2, VCC -> 5V, GND -> GND
//       アクティブブザー(+) -> D8, (-) -> GND
#include <IRremote.h>

const uint8_t PIN_IR = 2;
const uint8_t PIN_BUZZER = 8;
const uint32_t BUZZER_MS = 300;    // 鳴る長さ
const uint8_t MIN_RAW_LEN = 40;    // ノイズは1-7bit程度。本物は40以上

uint32_t buzzerOffAt = 0;

void setup() {
  pinMode(PIN_BUZZER, OUTPUT);
  digitalWrite(PIN_BUZZER, LOW);
  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK);
}

void loop() {
  if (buzzerOffAt && millis() >= buzzerOffAt) {
    digitalWrite(PIN_BUZZER, LOW);
    buzzerOffAt = 0;
  }

  if (IrReceiver.decode()) {
    bool decoded = IrReceiver.decodedIRData.protocol != UNKNOWN;
    bool longEnough = IrReceiver.decodedIRData.rawlen >= MIN_RAW_LEN;
    bool isRepeat = IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT;
    // 解読できた信号 or 十分な長さの信号(エアコン等)のみ反応
    if ((decoded || longEnough) && !isRepeat) {
      digitalWrite(PIN_BUZZER, HIGH);
      buzzerOffAt = millis() + BUZZER_MS;
    }
    IrReceiver.resume();
  }
}
