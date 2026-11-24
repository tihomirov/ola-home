# MQTT contract (draft)

Single source of truth for device ↔ server messages. Firmware, backend and
SPA all follow this file and the JSON schemas in [`schemas/`](schemas/).

`{id}` is the device ID: hex string from `hwinfo_get_device_id()`, never hardcoded.

| Topic | Direction | Retained | Payload |
|---|---|---|---|
| `home/{id}/config` | device → broker | yes | [`config.schema.json`](schemas/config.schema.json): device type, fw version, capability list |
| `home/{id}/state` | device → broker | yes | [`state.schema.json`](schemas/state.schema.json): `{ "<cap name>": <cap state>, ... }` |
| `home/{id}/set` | backend → device | no | [`set.schema.json`](schemas/set.schema.json): `{ "<cap name>": <cap command>, ... }` |
| `home/{id}/availability` | device → broker | yes | `online` / `offline` (MQTT Last Will) |

## Rules

- Devices talk only to the broker, never to the backend directly.
- Each device has its own MQTT credentials; the broker is never exposed to the internet.
- Device type (`CONFIG_HIOT_DEVICE_TYPE`) is for UI / grouping / OTA image selection only.
  Behavior comes from capabilities.
- Local input and remote `set` commands go through the same capability `set()` path.

## Capability types

Each capability type has one schema in [`schemas/capabilities/`](schemas/capabilities/)
describing its `describe` / `state` / `set` shapes.

| Type | State | Command |
|---|---|---|
| `switch` | `{ "on": bool }` | `{ "on": bool }` |
| `brightness` | `{ "level": 0..100 }` | `{ "level": 0..100 }` |
| `timer` | `{ "remaining_min": int }` | `{ "minutes": int }` (0 cancels) |

## Example

```
home/a1b2c3d4e5f6/config  (retained)
{
  "device_type": "lamp",
  "fw_version": "0.1.0",
  "capabilities": [
    { "type": "brightness", "name": "lamp-brightness", "min": 0, "max": 100 },
    { "type": "switch",     "name": "lamp-switch" },
    { "type": "timer",      "name": "lamp-timer", "presets": [15, 30, 60] }
  ]
}

home/a1b2c3d4e5f6/set
{ "lamp-brightness": { "level": 40 } }
```

## Codegen

TODO: generate TypeScript types for the SPA (and backend, if TS) from the schemas.
