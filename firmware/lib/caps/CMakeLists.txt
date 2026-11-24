# SPDX-License-Identifier: Apache-2.0

zephyr_sources_ifdef(CONFIG_HIOT_CAP_SWITCH switch/switch.c)
zephyr_sources_ifdef(CONFIG_HIOT_CAP_BRIGHTNESS brightness/brightness.c)
zephyr_sources_ifdef(CONFIG_HIOT_CAP_TIMER timer/timer.c)
