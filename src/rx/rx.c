#include "ch32fun.h"
#include <stdio.h>

// ---------- ピン定義 (UIAPduino Pro Micro CH32V003) ----------
// D3 ピン = PC1 (受光モジュール OUT 端子: 通常HIGH, 赤外線受光時LOW)
#define PIN_IR_RECEIVER PC1
// D5 ピン = PC3 (アクティブブザー (+) 端子: アクティブHIGH)
#define PIN_BUZZER      PC3
// D2 ピン = PC0 (オンボードLED: リセット・受光確認用)
#define PIN_LED         PC0

// 指定論理値(state: 0または1)が継続する時間をマイクロ秒で計測（タイムアウト時は0）
uint32_t measurePulseUs(int state, uint32_t timeoutUs)
{
    uint32_t start = SysTick->CNT;
    uint32_t maxTicks = timeoutUs * 6; // SysTick: 6MHz (1µs = 6ticks)
    while (funDigitalRead(PIN_IR_RECEIVER) == state) {
        if ((uint32_t)(SysTick->CNT - start) >= maxTicks) {
            return 0; // タイムアウト
        }
    }
    return (uint32_t)(SysTick->CNT - start) / 6;
}

// 有効な赤外線リモコン信号（エアコン/テレビ/自作銃等）か判定する復号関数
// 蛍光灯ノイズ（数十〜数百µs）を完全に排除し、本物のIR信号のみ確実に通す
int checkValidIRSignal()
{
    // 1. リーダーパルス（LOW時間）の長さを計測
    // 蛍光灯ノイズは0.5ms未満で消える。本物のリモコンは1.5ms〜12ms継続
    // （エアコン/家電協:約3.2ms, SONY:約2.4ms, テレビ/NEC:約9.0ms）
    uint32_t leaderLow = measurePulseUs(0, 15000);
    if (leaderLow < 1500 || leaderLow > 13000) {
        return 0; // 蛍光灯ノイズ（短すぎる）または異常パルスは破棄
    }

    // 2. リーダースペース（HIGH時間）の長さを計測
    // SONY:約0.6ms, エアコン:約1.6ms, NEC:約4.5ms
    uint32_t leaderHigh = measurePulseUs(1, 8000);
    if (leaderHigh < 300 || leaderHigh > 7000) {
        return 0; // 規格外スペースは破棄
    }

    // 3. 後続のデータパルス列（最低1ビット以上）が届くか確認
    // 単発の外乱光ではなく、変調パルス信号であることを確定
    int pulseCount = 0;
    for (int i = 0; i < 4; i++) {
        uint32_t bitLow = measurePulseUs(0, 3000);
        if (bitLow < 150 || bitLow > 2000) break;

        uint32_t bitHigh = measurePulseUs(1, 3000);
        if (bitHigh < 150 || bitHigh > 2500) break;

        pulseCount++;
    }

    // リーダー合格 ＋ 後続パルス確認で有効なリモコン信号（復号成功）と判定
    return (pulseCount >= 1);
}

int main()
{
    SystemInit();

    // 全GPIOペリフェラルのクロックを有効化
    funGpioInitAll();

    // オンボードLED (PC0 / D2) を出力に設定
    funPinMode(PIN_LED, GPIO_CFGLR_OUT_10Mhz_PP);

    // 【リセット可視化】起動時・再起動時にLEDを2回チカチカ点滅
    for (int i = 0; i < 2; i++) {
        funDigitalWrite(PIN_LED, FUN_HIGH);
        Delay_Ms(80);
        funDigitalWrite(PIN_LED, FUN_LOW);
        Delay_Ms(80);
    }

    // 受光信号ピン (PC1 / D3) をプルアップ入力に設定
    funPinMode(PIN_IR_RECEIVER, GPIO_CFGLR_IN_PUPD);
    funDigitalWrite(PIN_IR_RECEIVER, FUN_HIGH); // プルアップ有効化

    // ブザーピン (PC3 / D5) を出力(プッシュプル)に設定
    funPinMode(PIN_BUZZER, GPIO_CFGLR_OUT_10Mhz_PP);
    funDigitalWrite(PIN_BUZZER, FUN_LOW); // 初期状態は停止

    // 電源投入直後の受光素子の不安定状態を待機 (200ms)
    Delay_Ms(200);

    while (1) {
        // 通常はHIGH。赤外線の立ち下がり（LOW）を検出したときのみ復号判定へ
        if (funDigitalRead(PIN_IR_RECEIVER) == 0) {
            if (checkValidIRSignal()) {
                // 正当なリモコン/銃の信号を受信（復号成功！）
                funDigitalWrite(PIN_LED, FUN_HIGH);    // LED点灯
                funDigitalWrite(PIN_BUZZER, FUN_HIGH); // ブザー発音開始
                Delay_Ms(2500);                        // 2.5秒間鳴り続ける
                funDigitalWrite(PIN_BUZZER, FUN_LOW);  // ブザー停止
                funDigitalWrite(PIN_LED, FUN_LOW);     // LED消灯

                // 連続ヒット防止のクールダウン (300ms)
                Delay_Ms(300);
            }
        }
    }
}
