/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Local UX:
 *   rotate       -> brightness
 *   short press  -> on/off
 *   long press   -> cycle off-timer (15/30/60 min)
 *
 * Input events go through the same capability path as remote commands.
 * Kept app-specific on purpose: don't abstract input bindings until a
 * second device needs them.
 */

#include <zephyr/device.h>
#include <zephyr/input/input.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <hiot/core.h>
#include <hiot/caps/brightness.h>

LOG_MODULE_REGISTER(lamp_input, LOG_LEVEL_INF);

#define STEP_PER_DETENT 5

static void on_encoder(struct input_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	if (evt->type != INPUT_EV_REL || evt->code != INPUT_REL_WHEEL) {
		return;
	}

	struct hiot_cap *cap = hiot_cap_find("lamp-brightness");

	if (cap == NULL) {
		return;
	}

	int level = hiot_brightness_get_level(cap) + evt->value * STEP_PER_DETENT;

	level = CLAMP(level, 0, HIOT_BRIGHTNESS_MAX);
	(void)hiot_brightness_set_level(cap, (uint8_t)level);
	LOG_DBG("brightness %d", level);
}
INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(encoder)), on_encoder, NULL);

static void on_button(struct input_event *evt, void *user_data)
{
	ARG_UNUSED(user_data);

	if (evt->type != INPUT_EV_KEY || evt->value != 1) {
		return;
	}

	switch (evt->code) {
	case INPUT_KEY_A:
		/* TODO (step 2): toggle the "lamp-switch" capability. */
		LOG_INF("short press");
		break;
	case INPUT_KEY_X:
		/* TODO (step 2): cycle the "lamp-timer" presets. */
		LOG_INF("long press");
		break;
	default:
		break;
	}
}
INPUT_CALLBACK_DEFINE(DEVICE_DT_GET(DT_NODELABEL(longpress)), on_button, NULL);
