# SPDX-License-Identifier: Apache-2.0

add_subdirectory_ifdef(CONFIG_HIOT_CORE hiot_core)
add_subdirectory_ifdef(CONFIG_HIOT_CORE caps)

# Not implemented yet (see docs/PROJECT_BRIEF.md, implementation order):
#   hiot_net        - Wi-Fi, MQTT, LWT, SNTP, reconnect   (step 4)
#   hiot_ota        - MCUboot-based OTA                     (step 8)
#   hiot_provision  - Wi-Fi setup (SoftAP + QR) and pairing (step 8), spec in hiot_provision/README.md
