# RTS Main Repo

## 1. Prerequisites

* VSCode (or other editor)
  * VSCode has a PlatformIO extension that works pretty well.
* [PlatformIO Core](https://platformio.org/install/cli)

## 2. Usage

```bash
cd test1
pio pkg install
pio run -e esp32dev
# pio run -e esp32dev -t upload (if uploading to esp32)
```
