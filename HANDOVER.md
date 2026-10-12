# 引き継ぎメモ (2026-10-13)

文化祭IRレーザータグ(電子サバゲー)。Arduino UNO + 38kHz赤外線モジュール。
詳細設計は `docs/DESIGN.md`。

## フォルダ構成

| 場所 | 役割 | 扱い |
|------|------|------|
| `arc/` | 過去の計画書・STL生成・旧スケッチの**アーカイブ** | **編集禁止** |
| `docs/` | 生きている設計書 (`DESIGN.md`) | ここを改定していく |
| `build/` | 実機に書き込むスケッチ+手順 | 作業はここ |

## 環境

- ボード: **Arduino UNO @ COM5** (変わったら `arduino-cli board list` で確認)
- arduino-cli: `~/tools/arduino-cli/arduino-cli.exe` (ZIP版。PATH未登録)
  - arduino:avr 1.8.8, IRremote 4.7.1 インストール済
- VSCode: `.vscode/c_cpp_properties.json` でIntelliSense設定済(赤波線が出たらウィンドウ再読み込み)
- PlatformIO用 `build/platformio.ini` もある(どちらでもビルド可)

## スケッチ (build/)

| スケッチ | 内容 | 状態 |
|----------|------|------|
| `ir_beep` | リモコン信号でブザーが鳴る(最小・エアコン等長い信号も対応) | **書き込み済・最新** |
| `ir_dump` | 受信したIRのProtocol/Address/Commandをシリアル表示 | 動作確認済 |
| `ir_rx` | 的: 特定ボタンでHP減算・ブザー/LED/振動 | `GUN_ADDRESS`/`GUN_COMMAND`にリモコン実測値の転記が必要 |
| `ir_tx` | 銃: NECでmagic+team+player付き発射・残弾/リロード | 未実機確認 |

## CLI早見表

```powershell
$CLI="$HOME\tools\arduino-cli\arduino-cli.exe"
& $CLI board list                                  # ボード確認
& $CLI compile --fqbn arduino:avr:uno ir_beep      # コンパイル
& $CLI upload -p COM5 --fqbn arduino:avr:uno ir_beep  # 書き込み
python serial_mon.py COM5 30                       # シリアル(115200, pyserial使用)
```

※ `arduino-cli monitor` は非対話環境では即終了するので `build/serial_mon.py` を使う。

## 配線 (受信側)

- 受光モジュール: `OUT→D2, VCC→5V, GND→GND` (複数個はOUTを直接束ねてOK)
- アクティブブザー: `+→D8, -→GND` (tone()は使わない。Timer2がIRremoteと競合)
- LED→D5(330Ω), 振動モータ→D6(2N2222必須・フライホイールダイオード並列)
- ブザーがうるさい → 穴にテープ or 直列に100Ω〜1kΩ

## 次にやること

1. `ir_dump` で使うリモコンのボタンの `Address/Command` を実測 → `ir_rx.ino` の `GUN_ADDRESS`/`GUN_COMMAND` に転記
2. `ir_rx` 動作確認 → HP減算・死亡演出の確認
3. 受光部を複数個(前後+銃)に増設・体への装着(ヘアバンド等)
4. 銃側: `ir_tx` + IR LED×2 + 2N2222 + レンズ(100均ルーペ)でビーム絞り
5. 筐体: `arc/2026Bunkasai_gun.stl` は外形モック。電装座は `gen_stl4.py` (未作成)
6. 当日: 3丁・室内・HP5・3秒無敵・リスポーン10秒 (運用は `arc/operation_manual.md`)

## 注意

- **Nano EveryはATmega4809で別チップ**。買い足しはATmega328P版NanoかUNO互換
- 受光モジュールのピン順は品番ごとに違う。データシート確認必須(逆挿しで壊れる)
- ノイズでUNKNOWNが連発することがある → 受光部に黒筒/4.7µFコンデンサで軽減
