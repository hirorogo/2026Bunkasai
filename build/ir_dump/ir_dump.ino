// ir_dump.ino - Phase 0: 市販リモコンの信号解析
// Board: Arduino UNO (ATmega328P)
// 用途: リモコンのボタンを押すと Protocol / Address / Command をシリアルに表示。
//       ここで出た値を ir_rx.ino の GUN_ADDRESS / GUN_COMMAND に転記する。
// 配線: 受光モジュール OUT -> D2, VCC -> 5V, GND -> GND (VCC-GND間に 4.7uF)
#include <IRremote.h>

const uint8_t PIN_IR = 2;

void setup() {
  Serial.begin(115200);
  IrReceiver.begin(PIN_IR, ENABLE_LED_FEEDBACK);
  Serial.println("IR dump ready. リモコンのボタンを押してください");
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial); // プロトコル名+addr+cmdを1行で
    Serial.println();
    IrReceiver.resume();
  }
}
