# RTS Main Repo

## 1. Prerequisites

* VSCode (or other editor)
  * VSCode has a PlatformIO extension that works pretty well.
  * Use the "Serial Monitor" extension for an integrated terminal.
* [PlatformIO Core](https://platformio.org/install/cli)

## 2. Usage

```bash
cd test1
pio pkg install
pio run -e esp32dev
# pio run -e esp32dev -t upload (if uploading to esp32)
```

Please note that `pio pkg install` and/or `pio run -e esp32dev` will take a while if you are running them for the first time, as PlatformIO must install the required toolchains.

Also note that you may need additional drivers to connet to your ESP32, if it's not being recognized that can be the issue.
