# Home IoT System — Project Brief

## Goal
Self-built home IoT system: firmware for devices, a home server, and a web SPA to CRUD and control devices.
Starts with one device type (smart lamp), designed to scale to many device types.
Also a learning project: Zephyr RTOS + full-stack IoT.

## First device: smart lamp (ESP32-C3)
Features: on/off, dimming (intensity), off-timer. Must work locally without the server.

**Hardware (assumed: 12V single-color LED strip, not yet confirmed)**
- MCU: ESP32-C3 SuperMini (Zephyr board target `esp32c3_supermini`)
- Power: 12V adapter (2A+) → MP1584 buck set to 5V → ESP 5V pin. Set buck voltage BEFORE connecting ESP.
- LED switch: D4184 MOSFET module (low-side), no-optocoupler variant
- Input: KY-040 rotary encoder, powered from 3.3V
- All grounds common

**Pin map**
| GPIO | Function |
|---|---|
| 4 | PWM → MOSFET (20 kHz, gamma-corrected, fades) |
| 5 | Encoder CLK |
| 6 | Encoder DT |
| 7 | Encoder SW (internal pull-up) |

Avoid: GPIO2/8/9 (strapping), 18/19 (USB), 20/21 (UART).

**Local UX:** rotate = brightness, short press = on/off, long press = cycle off-timer (15/30/60 min).

## System architecture
- Devices ↔ **MQTT broker (Mosquitto)** ↔ **Backend** (device registry, rules, Postgres) ↔ **Web SPA** (REST for CRUD, WebSocket for live state)
- Server runs on a home box (Raspberry Pi / mini PC) via Docker Compose: Mosquitto, backend, Postgres, Nginx serving SPA
- Devices never talk to the backend directly — only to the broker
- Remote access via Tailscale/WireGuard; broker never exposed to internet; per-device MQTT credentials
- Later: Zigbee2MQTT bridge for off-the-shelf devices. Matter: out of scope.

## Core design principle: capabilities
A device = a list of capabilities (`switch`, `brightness`, `timer`, later `temperature`, `relay`…).
Every layer is generic over capabilities:
- Firmware: capability implementations, instantiated from devicetree
- Protocol: one JSON Schema per capability type
- Backend: registry stores capabilities; adapter interface for external sources (own MQTT devices, Zigbee2MQTT later)
- SPA: capability → widget map (brightness → slider, switch → toggle)

Rule: new device built from existing capability types = new firmware app + overlay, zero server/web changes.
New capability type = schema + firmware cap + widget.

## MQTT contract (draft)
- `home/{id}/config` — capabilities + metadata (device type, fw version), retained
- `home/{id}/state` — current state, retained
- `home/{id}/set` — commands
- `home/{id}/availability` — online/offline via MQTT Last Will
- Device ID from `hwinfo_get_device_id()`, never hardcoded
- Device type (`CONFIG_HIOT_DEVICE_TYPE`) is for UI/grouping/OTA image selection only; behavior comes from capabilities

## Firmware: Zephyr RTOS
- Upstream `esp32c3_supermini` board; enable `&wifi`, `&ledc0` in overlay
- Wi-Fi, MQTT client, SNTP, settings/NVS, MCUboot via sysbuild (OTA)
- Encoder: `gpio-qdec` input driver; button: `gpio-keys` + `zephyr,input-longpress`
- Known gap: no ready-made Wi-Fi provisioning → build own (BLE GATT or captive portal); no hardcoded credentials
- Decoupling via **zbus**: capabilities publish state to zbus, MQTT bridge subscribes. Local input and remote commands use the same capability `set` path.
- Capability interface (registered via iterable sections):
  - `describe()`, `get_state()`, `set()`, plus `HIOT_CAP_DEFINE` macro
- Capabilities declared in devicetree with custom bindings (`hiot,brightness`, `hiot,switch`, `hiot,timer`), one instance per DT node via `DT_FOREACH_STATUS_OKAY`
- Features toggled per app via Kconfig (`CONFIG_HIOT_CAP_BRIGHTNESS=y`)
- New app = different behavior/inputs; variant = same behavior, different hardware (overlay + conf fragment)
- Don't abstract input bindings until a second device needs them

## Monorepo structure
Repo is a **west manifest repository**; `firmware/` is a Zephyr module.
Zephyr lands outside the repo as a sibling (`west init -l ola-home && west update`).
Use `name-allowlist` in west.yml (hal_espressif, mcuboot, mbedtls…).

```
ola-home/
├── west.yml
├── protocol/            # single source of truth: topics.md, JSON schemas, codegen → TS types
├── firmware/
│   ├── zephyr/module.yml
│   ├── CMakeLists.txt, Kconfig
│   ├── include/hiot/    # core.h, cap.h, caps/*.h
│   ├── dts/bindings/    # hiot,brightness.yaml, hiot,switch.yaml, hiot,timer.yaml
│   ├── boards/          # custom boards later
│   ├── lib/
│   │   ├── hiot_core/   # registry, config/state publishing, command routing
│   │   ├── hiot_net/    # Wi-Fi, MQTT, LWT, SNTP, reconnect
│   │   ├── hiot_ota/
│   │   ├── hiot_provision/
│   │   └── caps/        # brightness/, switch/, timer/
│   └── apps/
│       └── lamp/
│           ├── CMakeLists.txt, prj.conf, sysbuild.conf, sample.yaml
│           ├── boards/esp32c3_supermini.overlay (+ .conf)
│           ├── variants/cct.overlay
│           └── src/main.c, src/lamp_input.c
├── server/
├── web/
├── deploy/              # docker-compose, mosquitto.conf, nginx
├── tools/               # flashing, provisioning, MQTT test scripts
└── .github/workflows/   # west build per app (Zephyr Docker image), server tests, web build
```

Note: since `firmware/` is not the repo root, west doesn't auto-discover it as a
module. Each app adds it with `list(APPEND ZEPHYR_EXTRA_MODULES ...)` in its `CMakeLists.txt`.

Build: `west build -b esp32c3_supermini --sysbuild firmware/apps/lamp`
Variant: add `-- -DEXTRA_DTC_OVERLAY_FILE=variants/cct.overlay`

## Tooling (recommended)
- VS Code; commit `.vscode/` (extensions.json, settings, tasks for build/flash/monitor per app)
- clangd with Zephyr's `compile_commands.json` (not MS IntelliSense)
- nRF DeviceTree + nRF Kconfig extensions
- Debug over C3 built-in USB-JTAG with Espressif's OpenOCD fork
- Optional dev container (Zephyr CI image); flash from host
- Alternative: CLion (free non-commercial)

## Open decisions
- Light source confirmation (12V strip length/wattage → power budget)
- Backend language/framework
- SPA framework (React or Vue)
- Provisioning method (BLE vs captive portal)
- OTA transport (MQTT vs HTTP)

## Implementation order
1. Repo scaffold: west.yml, module, bindings, hiot_core skeleton, lamp app building for esp32c3_supermini
2. Lamp local function: PWM + gamma + fades, encoder, button, timer (no network)
3. Protocol: topic contract + capability JSON schemas
4. hiot_net: Wi-Fi + MQTT + availability; test with mosquitto_pub/sub
5. deploy/: Mosquitto + Postgres via Docker Compose
6. Backend: device registry from config messages, REST + WebSocket
7. SPA: device list, capability widgets
8. OTA (MCUboot) and provisioning
