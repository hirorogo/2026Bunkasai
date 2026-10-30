// UIAPduino IRレーザータグ 受光部（的）パッシブブザー（他励式）対応版スケッチ

const int PIN_IR_RECEIVER = 3; // 受光モジュール OUT 端子 (PC1 / アクティブLOW)
const int PIN_BUZZER      = 5; // パッシブブザー端子 (PC3)

// 発音周波数: 2700Hz
const unsigned int BUZZER_FREQ_HZ = 2700;

void setup() {
  pinMode(PIN_IR_RECEIVER, INPUT_PULLUP);
  pinMode(PIN_BUZZER, OUTPUT);
  noTone(PIN_BUZZER);
}

void loop() {
  if (digitalRead(PIN_IR_RECEIVER) == LOW) {
    // 受光検知: パッシブブザーを2.5秒間鳴らす
    tone(PIN_BUZZER, BUZZER_FREQ_HZ, 2500);
    delay(2500);
    noTone(PIN_BUZZER);
  }
}
