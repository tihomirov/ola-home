# tools

Helper scripts: flashing, provisioning, MQTT testing.

## TLS certificates (`certs/mkcerts.sh`)

MQTT runs over TLS (port 8883). The broker certificate is signed by our own
CA, and firmware trusts that CA. Details: [docs/spec/server.md §12.1](../docs/spec/server.md).

```sh
tools/certs/mkcerts.sh ca                            # once; CA goes to ~/.ola-home/ca
tools/certs/mkcerts.sh broker ola.lan 192.168.1.50   # names/IPs devices use for the broker
tools/certs/mkcerts.sh info                          # subjects and expiry dates
```

- The CA key (`~/.ola-home/ca/ca.key`) stays on your PC. Never commit it or copy it
  to the server. Back it up offline: without it, every device has to be re-flashed with a new CA.
- The broker cert is valid for ~2 years. Renew it by running `broker` again (no firmware change).
- Works in Git Bash on Windows, Linux and macOS (needs OpenSSL 1.1.1+).

## MQTT checks

```sh
# watch everything (TLS)
mosquitto_sub -h ola.lan -p 8883 --cafile deploy/mosquitto/certs/ca.crt \
  -u <user> -P <pass> -t 'hiot/#' -v

# set brightness on a device
mosquitto_pub -h ola.lan -p 8883 --cafile deploy/mosquitto/certs/ca.crt \
  -u <user> -P <pass> -t 'hiot/<hw>/cmd' \
  -m '{"v":1,"cid":"test-1","caps":{"light_level":{"level":40}}}'
```
