// ==============================================================================
// UIAPduino / Arduino IRレーザータグ 送信機（送信テスト・トリガー発光）
// ==============================================================================
// ボタンを押している間、38kHz変調の赤外線パルスを送信します。
// 受信側（docs/DESIGN.md の受光モジュール D3）に向けるとブザーが鳴ります。
//
// 【使用パーツ & 配線】
//  1. プッシュボタンスイッチ (ミヤマ ADS-299 / 押すと導通が切れる2線):
//      - 2本の線をそれぞれ「UIAPduino D4」と「UIAPduino GND」に接続
//      - 普段: 導通(LOW) / ボタン押下時: 切れる(HIGH)
//  2. 赤外線送信モジュール (IR Transmitter V1221 / 3端子):
//      - DAT <---> UIAPduino D8
//      - VCC <---> UIAPduino 5V
//      - GND <---> UIAPduino GND
//  3. 動作確認LED:
//      - LED_BUILTIN (オンボードLED: UIAPduinoはD2、Arduino Uno/NanoはD13)
// ==============================================================================

#include <Arduino.h>

// ---------- ピン定義 ----------
const int PIN_BUTTON     = 4; // トリガーボタン (左列 6番目「4」ピン)
const int PIN_IR_LED     = 8; // 赤外線LED出力ピン (左列 11番目「8」ピン / モジュールの DAT)
const int PIN_STATUS_LED = 2; // UIAPduino オンボードLED (基板上の「L」LED / D2)

// ---------- スイッチ極性設定 ----------
// 通常のスイッチ仕様（プルアップ入力）: 押下時は LOW になります
const int BUTTON_PRESSED_STATE = LOW;

// ---------- 38kHzキャリア生成用定数 ----------
// 38kHzの周期は約 26.3 microseconds (HIGH: 13us, LOW: 13us)
const int CARRIER_HIGH_US = 13;
const int CARRIER_LOW_US  = 13;

// 38kHzパルスを指定マイクロ秒バースト出力する関数
void send38kHzBurst(unsigned long durationUs) {
  unsigned long start = micros();
  while (micros() - start < durationUs) {
    digitalWrite(PIN_IR_LED, HIGH);
    delayMicroseconds(CARRIER_HIGH_US);
    digitalWrite(PIN_IR_LED, LOW);
    delayMicroseconds(CARRIER_LOW_US);
  }
}

void setup() {
  // トリガーボタンを入力プルアップに設定
  pinMode(PIN_BUTTON, INPUT_PULLUP);

  // 赤外線LEDおよびインジケータLEDを出力に設定
  pinMode(PIN_IR_LED, OUTPUT);
  digitalWrite(PIN_IR_LED, LOW);

  pinMode(PIN_STATUS_LED, OUTPUT);
  digitalWrite(PIN_STATUS_LED, LOW);
}

void loop() {
  // ボタンが押されているか判定 (押下時に HIGH)
  if (digitalRead(PIN_BUTTON) == BUTTON_PRESSED_STATE) {
    // 送信中インジケータ（可視光LED）を点灯
    digitalWrite(PIN_STATUS_LED, HIGH);

    // 受光モジュールが安定検知できるよう、バースト（送信）と休止を繰り返す
    // 38kHzを 1.2ms 送信
    send38kHzBurst(1200);
    // 0.4ms 休止 (LOW)
    digitalWrite(PIN_IR_LED, LOW);
    delayMicroseconds(400);

  } else {
    // ボタンが離されている時は消灯
    digitalWrite(PIN_IR_LED, LOW);
    digitalWrite(PIN_STATUS_LED, LOW);
    delay(5); // チャタリング・CPU負荷軽減
  }
}
