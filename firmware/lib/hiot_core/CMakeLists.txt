# SPDX-License-Identifier: Apache-2.0

zephyr_sources(core.c)

# Iterable section holding every HIOT_CAP_DEFINE()'d capability.
zephyr_linker_sources(DATA_SECTIONS hiot_cap.ld)
