#!/usr/bin/env python3
"""serial_mon.py - COM5のシリアルを読み続ける簡易モニタ
使い方: python serial_mon.py [COM番号] [秒数]
  例: python serial_mon.py COM5 30   (30秒読んで終了)
      python serial_mon.py COM5      (60秒)
Ctrl+Cで中断。要 pyserial。
"""
import sys, time
import serial

port = sys.argv[1] if len(sys.argv) > 1 else 'COM5'
duration = int(sys.argv[2]) if len(sys.argv) > 2 else 60

s = serial.Serial(port, 115200, timeout=1)
s.dtr = False
time.sleep(0.3)
s.dtr = True  # UNOをリセットして起動メッセージを拾う
time.sleep(2.0)

end = time.time() + duration
try:
    while time.time() < end:
        data = s.read(256)
        if data:
            sys.stdout.buffer.write(data)
            sys.stdout.buffer.flush()
except KeyboardInterrupt:
    pass
finally:
    s.close()
