#!/usr/bin/env bash
# Create the ola-home private CA and the Mosquitto broker certificate
# (MQTT over TLS, see docs/spec/server.md §12.1).
#
#   tools/certs/mkcerts.sh ca                      # once: create the CA
#   tools/certs/mkcerts.sh broker ola.lan 192.168.1.50   # broker cert for these names/IPs
#   tools/certs/mkcerts.sh info                    # show what exists and expiry dates
#
# Keys are ECDSA P-256 (fast on ESP32-C3, small certs).
#
# Locations (override with env vars):
#   OLA_CA_DIR       CA key + cert. Default: ~/.ola-home/ca  (outside the repo!)
#   OLA_BROKER_DIR   broker key + cert + CA cert copy. Default: deploy/mosquitto/certs (git-ignored)
#
# The CA private key must never be committed or copied to the server.
# Back it up offline: losing it means re-flashing every device with a new CA.

set -euo pipefail

# Stop Git Bash on Windows from rewriting "/O=..." subjects into paths
# (other paths still get converted, which the Windows openssl needs).
export MSYS2_ARG_CONV_EXCL='/O='

REPO_ROOT="$(cd "$(dirname "$0")/../.." && pwd)"
CA_DIR="${OLA_CA_DIR:-$HOME/.ola-home/ca}"
BROKER_DIR="${OLA_BROKER_DIR:-$REPO_ROOT/deploy/mosquitto/certs}"

CA_DAYS=7300      # ~20 years: firmware trusts this CA for the device's lifetime
BROKER_DAYS=825   # ~2 years: rotate by re-running "broker", no firmware change needed

die() { echo "error: $*" >&2; exit 1; }

usage() {
	sed -n '2,16p' "$0" | sed 's/^# \{0,1\}//'
	exit 1
}

gen_key() {
	openssl genpkey -algorithm EC -pkeyopt ec_paramgen_curve:P-256 -out "$1"
	chmod 600 "$1"
}

cmd_ca() {
	[ -f "$CA_DIR/ca.key" ] && die "CA already exists in $CA_DIR (refusing to overwrite)"
	mkdir -p "$CA_DIR"
	chmod 700 "$CA_DIR" 2>/dev/null || true

	gen_key "$CA_DIR/ca.key"
	openssl req -x509 -new -sha256 -days "$CA_DAYS" \
		-key "$CA_DIR/ca.key" \
		-subj "/O=ola-home/CN=ola-home CA" \
		-addext "basicConstraints=critical,CA:TRUE,pathlen:0" \
		-addext "keyUsage=critical,keyCertSign,cRLSign" \
		-addext "subjectKeyIdentifier=hash" \
		-out "$CA_DIR/ca.crt"

	echo "CA created:"
	echo "  key  $CA_DIR/ca.key   <- keep secret, back up offline"
	echo "  cert $CA_DIR/ca.crt   <- public, goes into firmware and broker"
}

cmd_broker() {
	[ -f "$CA_DIR/ca.key" ] || die "no CA in $CA_DIR, run '$0 ca' first"
	[ $# -gt 0 ] || die "give at least one DNS name or IP the devices use to reach the broker"

	# Always valid for the docker-internal name and localhost too.
	local names=("$@" mosquitto localhost)
	local san="" n
	for n in "${names[@]}"; do
		if [[ "$n" =~ ^[0-9]+\.[0-9]+\.[0-9]+\.[0-9]+$ ]]; then
			san+="IP:$n,"
		else
			san+="DNS:$n,"
		fi
	done
	san="${san%,}"

	mkdir -p "$BROKER_DIR"
	tmp="$(mktemp -d)"
	trap 'rm -rf "$tmp"' EXIT

	cat >"$tmp/ext.cnf" <<EOF
basicConstraints=critical,CA:FALSE
keyUsage=critical,digitalSignature
extendedKeyUsage=serverAuth
subjectAltName=$san
subjectKeyIdentifier=hash
authorityKeyIdentifier=keyid
EOF

	gen_key "$BROKER_DIR/broker.key"
	openssl req -new -sha256 -key "$BROKER_DIR/broker.key" \
		-subj "/O=ola-home/CN=$1" -out "$tmp/broker.csr"
	openssl x509 -req -sha256 -days "$BROKER_DAYS" \
		-in "$tmp/broker.csr" \
		-CA "$CA_DIR/ca.crt" -CAkey "$CA_DIR/ca.key" \
		-set_serial "0x$(openssl rand -hex 16)" \
		-extfile "$tmp/ext.cnf" \
		-out "$BROKER_DIR/broker.crt"
	cp "$CA_DIR/ca.crt" "$BROKER_DIR/ca.crt"

	openssl verify -CAfile "$BROKER_DIR/ca.crt" "$BROKER_DIR/broker.crt" >/dev/null

	echo "Broker certificate created in $BROKER_DIR (valid for: $san)"
	echo "On the Linux server, let the mosquitto container (uid 1883) read the key:"
	echo "  sudo chown 1883:1883 $BROKER_DIR/broker.key"
}

cmd_info() {
	local f
	for f in "$CA_DIR/ca.crt" "$BROKER_DIR/broker.crt"; do
		if [ -f "$f" ]; then
			echo "== $f"
			openssl x509 -in "$f" -noout -subject -enddate -ext subjectAltName 2>/dev/null ||
				openssl x509 -in "$f" -noout -subject -enddate
		else
			echo "== $f (missing)"
		fi
	done
}

case "${1:-}" in
ca) shift; cmd_ca "$@" ;;
broker) shift; cmd_broker "$@" ;;
info) shift; cmd_info ;;
*) usage ;;
esac
