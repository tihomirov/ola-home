/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Off-timer capability: switches its target off after N minutes.
 * TODO (step 2): k_work_delayable that turns the target off; preset cycling.
 */

#define DT_DRV_COMPAT hiot_timer

#include <errno.h>

#include <zephyr/devicetree.h>
#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <hiot/cap.h>

struct timer_config {
	const char *target;
	const uint16_t *presets_min;
	size_t num_presets;
};

struct timer_data {
	/* Remaining minutes, 0 = timer not running. */
	uint16_t remaining_min;
};

static int timer_describe(const struct hiot_cap *cap, char *buf, size_t len)
{
	const struct timer_config *cfg = cap->config;
	int n = snprintk(buf, len, "{\"type\":\"%s\",\"name\":\"%s\",\"presets\":[",
			 cap->type, cap->name);

	for (size_t i = 0; i < cfg->num_presets && n > 0 && (size_t)n < len; i++) {
		n += snprintk(buf + n, len - n, "%s%u", i ? "," : "", cfg->presets_min[i]);
	}

	if (n > 0 && (size_t)n < len) {
		n += snprintk(buf + n, len - n, "]}");
	}

	return n;
}

static int timer_get_state(const struct hiot_cap *cap, char *buf, size_t len)
{
	const struct timer_data *data = cap->data;

	return snprintk(buf, len, "{\"remaining_min\":%u}", data->remaining_min);
}

static int timer_set(const struct hiot_cap *cap, const char *json, size_t len)
{
	ARG_UNUSED(cap);
	ARG_UNUSED(json);
	ARG_UNUSED(len);

	/* TODO (step 3): parse {"minutes": N} (0 cancels). */
	return -ENOTSUP;
}

static const struct hiot_cap_api timer_api = {
	.describe = timer_describe,
	.get_state = timer_get_state,
	.set = timer_set,
};

#define TIMER_INST(inst)                                                       \
	static const uint16_t timer_presets_##inst[] =                         \
		DT_INST_PROP(inst, presets_min);                               \
	static const struct timer_config timer_cfg_##inst = {                  \
		.target = DT_NODE_FULL_NAME(DT_INST_PHANDLE(inst, target)),    \
		.presets_min = timer_presets_##inst,                           \
		.num_presets = ARRAY_SIZE(timer_presets_##inst),               \
	};                                                                     \
	static struct timer_data timer_data_##inst;                            \
	HIOT_CAP_DEFINE(hiot_timer_##inst, DT_NODE_FULL_NAME(DT_DRV_INST(inst)), \
			"timer", &timer_api, &timer_cfg_##inst,                \
			&timer_data_##inst);

DT_INST_FOREACH_STATUS_OKAY(TIMER_INST)
