#include "ch32fun.h"
#include <stdio.h>

// ---------- ピン定義 (UIAPduino Pro Micro CH32V003) ----------
// D4 ピン = PC2 (トリガーボタン: 押すとHIGHになるNC接続)
#define PIN_BUTTON      PC2
// D8 ピン = PC6 (赤外線送信モジュール DAT)
#define PIN_IR_LED      PC6
// D2 ピン = PC0 (UIAPduino オンボード「L」LED)
#define PIN_STATUS_LED  PC0

// 押した時に導通が切れる（NC）スイッチ仕様: 押下時は 1 (HIGH)
#define BUTTON_PRESSED_STATE 1

// 38kHzパルス（周期約26µs: HIGH 13µs / LOW 13µs）を指定マイクロ秒バースト出力する関数
void send38kHzBurst(uint32_t durationUs)
{
    uint32_t cycles = durationUs / 26;
    for (uint32_t i = 0; i < cycles; i++) {
        funDigitalWrite(PIN_IR_LED, FUN_HIGH);
        Delay_Us(13);
        funDigitalWrite(PIN_IR_LED, FUN_LOW);
        Delay_Us(13);
    }
}

int main()
{
    SystemInit();

    // 全GPIOペリフェラルのクロックを有効化
    funGpioInitAll();

    // トリガーボタン (PC2) をプルアップ入力に設定
    funPinMode(PIN_BUTTON, GPIO_CFGLR_IN_PUPD);
    funDigitalWrite(PIN_BUTTON, FUN_HIGH); // プルアップ有効化

    // 赤外線LED (PC6) を出力(プッシュプル)に設定
    funPinMode(PIN_IR_LED, GPIO_CFGLR_OUT_10Mhz_PP);
    funDigitalWrite(PIN_IR_LED, FUN_LOW);

    // オンボード「L」LED (PC0) を出力(プッシュプル)に設定
    funPinMode(PIN_STATUS_LED, GPIO_CFGLR_OUT_10Mhz_PP);
    funDigitalWrite(PIN_STATUS_LED, FUN_LOW);

    while (1) {
        // ボタン状態を取得
        int btn = funDigitalRead(PIN_BUTTON);

        if (btn == BUTTON_PRESSED_STATE) {
            // ボタン押下中: オンボードLED点灯
            funDigitalWrite(PIN_STATUS_LED, FUN_HIGH);

            // 38kHz 赤外線パルスをバースト出力 (1.2ms 送信 + 0.4ms 休止)
            send38kHzBurst(1200);
            funDigitalWrite(PIN_IR_LED, FUN_LOW);
            Delay_Us(400);
        } else {
            // ボタン非押下: 赤外線・LEDともに消灯
            funDigitalWrite(PIN_IR_LED, FUN_LOW);
            funDigitalWrite(PIN_STATUS_LED, FUN_LOW);
            Delay_Ms(5);
        }
    }
}
