/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Brightness capability: drives one PWM channel.
 * TODO (step 2): proper gamma table, smooth fades (fade-ms), persist level.
 */

#define DT_DRV_COMPAT hiot_brightness

#include <errno.h>

#include <zephyr/devicetree.h>
#include <zephyr/drivers/pwm.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/printk.h>

#include <hiot/cap.h>
#include <hiot/caps/brightness.h>

LOG_MODULE_REGISTER(hiot_brightness, CONFIG_HIOT_LOG_LEVEL);

struct brightness_config {
	struct pwm_dt_spec pwm;
	uint32_t fade_ms;
};

struct brightness_data {
	uint8_t level;
};

/* Gamma ~2.0 approximation until a real lookup table lands. */
static uint32_t level_to_pulse(uint32_t period, uint8_t level)
{
	uint64_t l = level;

	return (uint32_t)((uint64_t)period * l * l /
			  (HIOT_BRIGHTNESS_MAX * HIOT_BRIGHTNESS_MAX));
}

int hiot_brightness_set_level(const struct hiot_cap *cap, uint8_t level)
{
	const struct brightness_config *cfg = cap->config;
	struct brightness_data *data = cap->data;
	int err;

	level = MIN(level, HIOT_BRIGHTNESS_MAX);
	err = pwm_set_pulse_dt(&cfg->pwm, level_to_pulse(cfg->pwm.period, level));
	if (err) {
		return err;
	}

	data->level = level;
	return 0;
}

uint8_t hiot_brightness_get_level(const struct hiot_cap *cap)
{
	const struct brightness_data *data = cap->data;

	return data->level;
}

static int brightness_init(const struct hiot_cap *cap)
{
	const struct brightness_config *cfg = cap->config;

	if (!pwm_is_ready_dt(&cfg->pwm)) {
		return -ENODEV;
	}

	return hiot_brightness_set_level(cap, 0);
}

static int brightness_describe(const struct hiot_cap *cap, char *buf, size_t len)
{
	return snprintk(buf, len, "{\"type\":\"%s\",\"name\":\"%s\",\"min\":0,\"max\":%d}",
			cap->type, cap->name, HIOT_BRIGHTNESS_MAX);
}

static int brightness_get_state(const struct hiot_cap *cap, char *buf, size_t len)
{
	return snprintk(buf, len, "{\"level\":%u}", hiot_brightness_get_level(cap));
}

static int brightness_set(const struct hiot_cap *cap, const char *json, size_t len)
{
	ARG_UNUSED(cap);
	ARG_UNUSED(json);
	ARG_UNUSED(len);

	/* TODO (step 3): parse {"level": N} with Zephyr's JSON library. */
	return -ENOTSUP;
}

static const struct hiot_cap_api brightness_api = {
	.init = brightness_init,
	.describe = brightness_describe,
	.get_state = brightness_get_state,
	.set = brightness_set,
};

#define BRIGHTNESS_INST(inst)                                                  \
	static const struct brightness_config brightness_cfg_##inst = {         \
		.pwm = PWM_DT_SPEC_INST_GET(inst),                              \
		.fade_ms = DT_INST_PROP(inst, fade_ms),                         \
	};                                                                     \
	static struct brightness_data brightness_data_##inst;                  \
	HIOT_CAP_DEFINE(hiot_brightness_##inst,                                \
			DT_NODE_FULL_NAME(DT_DRV_INST(inst)), "brightness",    \
			&brightness_api, &brightness_cfg_##inst,               \
			&brightness_data_##inst);

DT_INST_FOREACH_STATUS_OKAY(BRIGHTNESS_INST)
