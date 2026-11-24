# tools

Helper scripts: flashing, provisioning, MQTT testing.

Quick MQTT checks against the home broker:

```sh
# watch everything
mosquitto_sub -h <broker> -u <user> -P <pass> -t 'home/#' -v

# set brightness on a device
mosquitto_pub -h <broker> -u <user> -P <pass> -t 'home/<id>/set' \
  -m '{"lamp-brightness":{"level":40}}'
```
