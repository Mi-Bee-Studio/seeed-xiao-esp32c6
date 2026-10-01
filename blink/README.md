# blink — Baseline (Test Firmware)

> Isomorphic copy of [`esp32-s3-zero/blink`](../../esp32-s3-zero/blink) (copy-first convention).

## What it does

- Onboard USER LED (GPIO15) toggles every second;
- BOOT button (GPIO9) held keeps the LED on (interaction self-check);
- One heartbeat log line every 10 s (`uptime` / `heap`) for serialtap capture;
- Onboard **web maintenance page :80**: WiFi provisioning / firmware OTA / reboot
  (rescue SoftAP: see `AP_SSID` in the source / `12345678` → `192.168.4.1` when unprovisioned);
- **Watchdog**: ESP-IDF TWDT, 5 s timeout with panic (main loop feeds every 1 s);
- **OTA**: dual OTA slots (app at `0x20000`); the web page streams to the
  alternate slot → verify → switch → reboot.

## Build & Flash

```bash
cd blink
idf.py set-target <target>   # see sdkconfig.defaults
idf.py build
idf.py -p COMx flash monitor
```

Console runs on the native USB-Serial-JTAG (no USB-UART bridge); release assets include `flash_blink.sh/.bat`.

## Firmware baseline norms

| Baseline | Status |
|----------|--------|
| Watchdog | ✅ TWDT 5 s panic, main loop feeds |
| Web/API OTA | ✅ dual OTA slots + streaming `POST /ota` |
