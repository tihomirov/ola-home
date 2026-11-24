# ola-home

Self-built home IoT system: Zephyr firmware for devices, a home server, and a
web SPA to manage and control them. Starts with one device (a smart lamp on
ESP32-C3) and is designed to grow to many device types.

Full design brief: [docs/PROJECT_BRIEF.md](docs/PROJECT_BRIEF.md).

## Architecture

```
devices ──MQTT──▶ Mosquitto ◀──▶ backend (registry, rules, Postgres) ◀──REST/WS──▶ web SPA
```

Everything is generic over **capabilities** (`switch`, `brightness`, `timer`, …).
A new device built from existing capability types = new firmware app + overlay,
with zero server/web changes.

## Repository layout

| Path | What |
|---|---|
| `west.yml` | West manifest (this repo is the manifest repo) |
| `firmware/` | Zephyr module: `hiot` core, capabilities, DT bindings, apps |
| `firmware/apps/lamp/` | Smart lamp app for `esp32c3_supermini` |
| `protocol/` | MQTT topic contract + JSON schemas (single source of truth) |
| `server/` | Backend (not started) |
| `web/` | SPA (not started) |
| `deploy/` | Docker Compose: Mosquitto, Postgres (backend + nginx later) |
| `tools/` | Flashing / provisioning / MQTT test helpers |

## Firmware quick start

Prerequisites: [Zephyr getting started](https://docs.zephyrproject.org/latest/develop/getting_started/index.html)
(Python venv with `west`, Zephyr SDK with the `riscv64-zephyr-elf` toolchain).

```sh
mkdir ola-workspace && cd ola-workspace
git clone https://github.com/tihomirov/ola-home.git
west init -l ola-home
west update
west zephyr-export
pip install -r zephyr/scripts/requirements.txt
west blobs fetch hal_espressif

cd ola-home
west build -b esp32c3_supermini --sysbuild firmware/apps/lamp
west flash
west espressif monitor
```

CCT variant:

```sh
west build -b esp32c3_supermini --sysbuild firmware/apps/lamp -- -DEXTRA_DTC_OVERLAY_FILE=variants/cct.overlay
```

## Server quick start

```sh
cd deploy
cp .env.example .env        # edit passwords
# create mosquitto/passwd (see mosquitto/mosquitto.conf)
docker compose up -d
```

## Status

Step 1 of the implementation order (repo scaffold) is in place. The firmware
skeleton has not been built yet; expect small fixes on the first
`west build`. See the brief for the full roadmap and open decisions.
