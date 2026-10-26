# UIAPduino Pro Micro (CH32V003 V1.4) 仕様・ピン配置資料

公式ドキュメントおよびリポジトリより取得した、UIAPduino Pro Micro (CH32V003 V1.4) の回路図・ピン配置図・仕様まとめです。

---

## 1. 取得済みファイル一覧

- **ピン配置図**: [Pinout-Diagram.png](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/Pinout-Diagram.png) （オリジナル: [Pinout-Diagram.avif](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/Pinout-Diagram.avif)）
- **基板外形寸法図**: [Dimensions.png](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/Dimensions.png)
- **回路図 (PDF)**: [UIAPduino-Pro-Micro-CH32V003-V1dot4-sch.pdf](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/UIAPduino-Pro-Micro-CH32V003-V1dot4-sch.pdf)
- **EAGLE設計ファイル**:
  - 回路図: [UIAPduino_Pro_Micro.sch](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/UIAPduino_Pro_Micro.sch)
  - 基板レイアウト: [UIAPduino_Pro_Micro.brd](file:///Users/hiro/Documents/2026Bunkasai/docs/uiapduino/UIAPduino_Pro_Micro.brd)

---

## 2. ピン配置（ピンアサイン表）

Arduino IDE環境におけるデジタルピン番号とCH32V003 MCUのポート対応です。

| Arduino ピン番号 | MCU ポート (CH32V003) | 主な機能 / ペリフェラル | 備考 |
|:---:|:---:|:---|:---|
| **D0** | `PA1` | A1 | アナログ入力対応 |
| **D1** | `PA2` | A0 | アナログ入力対応 |
| **D2** | `PC0` | PWM / **LED_BUILTIN** | オンボードLED接続 |
| **D3** | `PC1` | I2C SDA | 受光部OUTピン等に推奨 |
| **D4** | `PC2` | I2C SCL | |
| **D5** | `PC3` | PWM | ブザー接続等に推奨 |
| **D6** | `PC4` | A2 / SPI SS | |
| **D7** | `PC5` | SPI SCK | ※左右の7番ピンは内部で導通 |
| **D8** | `PC6` | PWM / SPI MOSI | ※左右の8番ピンは内部で導通 |
| **D9** | `PC7` | PWM / SPI MISO | ※左右の9番ピンは内部で導通 |
| **D10** | `PD0` | デジタルI/O | |
| **D11** | `PD1` | デバッグ兼用 (SWIO) | 使用時は `pinV32_DisconnectDebug(PD_1)` が必要 |
| **D12** | `PD2` | A3 | |
| **D13** | `PD3` | A4 | |
| **D14** | `PD4` | A7 | |
| **D15** | `PD5` | A5 / UART TX | シリアル通信送信 |
| **D16** | `PD6` | A6 / UART RX | シリアル通信受信 |
| **D17** | `PD7` | デジタルI/O | |

---

## 3. 回路・電源の留意事項

1. **電源仕様**:
   - 初期状態ではボードの電源電圧は5V動作です。
   - 38kHz受光モジュール（OSRB38C9AAやVS1838B等）は 2.7V〜5.5V で動作するため、UIAPduinoの5Vピンまたは3.3Vピンから直接給電可能です。
2. **ピンの内部導通**:
   - 基板左右にある「7・8・9番ピン」は基板内部で接続されています。
3. **D11ピン（PD1）の注意**:
   - 書込み・デバッグ用（SWIO）端子と兼用になっているため、通常のGPIOとして使う場合はSWIO切断コードが必要です。受光部やブザーにはD3やD5など通常のピンを使用することを推奨します。
4. **本プロジェクト（IRレーザータグ）での推奨ピンアサイン**:
   - **受光モジュールOUT**: `D3` (PC1)
   - **アクティブブザー(+)**: `D5` (PC3)
   - **GND / VCC**: 基板のGNDピンおよびVCC(5V)ピン

---

## 4. 参照リンク

- **公式ドキュメント**: https://www.uiap.jp/uiapduino/pro-micro/ch32v003/v1dot4
- **公式GitHub**: https://github.com/YuukiUmeta-UIAP/UIAP-Devices
