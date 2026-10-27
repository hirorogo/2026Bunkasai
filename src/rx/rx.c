#include "ch32fun.h"
#include <stdio.h>

// ---------- ピン定義 (UIAPduino Pro Micro CH32V003) ----------
// D3 ピン = PC1 (受光モジュール OUT 端子: アクティブLOW)
#define PIN_IR_RECEIVER PC1
// D5 ピン = PC3 (アクティブブザー (+) 端子: アクティブHIGH)
#define PIN_BUZZER      PC3

int main()
{
    SystemInit();

    // 全GPIOペリフェラルのクロックを有効化
    funGpioInitAll();

    // 受光信号ピン (PC1 / D3) をプルアップ入力に設定
    funPinMode(PIN_IR_RECEIVER, GPIO_CFGLR_IN_PUPD);
    funDigitalWrite(PIN_IR_RECEIVER, FUN_HIGH); // プルアップ有効化

    // ブザーピン (PC3 / D5) を出力(プッシュプル)に設定
    funPinMode(PIN_BUZZER, GPIO_CFGLR_OUT_10Mhz_PP);
    funDigitalWrite(PIN_BUZZER, FUN_LOW); // 初期状態は停止

    while (1) {
        // 赤外線を受光するとOUT端子がLOW(0)になる
        if (funDigitalRead(PIN_IR_RECEIVER) == 0) {
            funDigitalWrite(PIN_BUZZER, FUN_HIGH); // ブザー発音
        } else {
            funDigitalWrite(PIN_BUZZER, FUN_LOW);  // ブザー停止
        }
    }
}
