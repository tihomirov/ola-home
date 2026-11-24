# server

Backend: device registry (built from `home/+/config` messages), rules, Postgres,
REST for CRUD and WebSocket for live state.

Talks to devices only through the MQTT broker. Has an adapter interface for
device sources (own MQTT devices now, Zigbee2MQTT later).

**Open decision:** language / framework. Implementation is step 6.
