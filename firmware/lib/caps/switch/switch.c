/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Switch capability: on/off. If the node has a `target` phandle to a
 * brightness capability, turning on restores the last level.
 * TODO (step 2): drive the target capability.
 */

#define DT_DRV_COMPAT hiot_switch

#include <errno.h>
#include <stdbool.h>

#include <zephyr/sys/printk.h>
#include <zephyr/sys/util.h>

#include <hiot/cap.h>

struct switch_config {
	/* Instance name of the target capability, or NULL. */
	const char *target;
};

struct switch_data {
	bool on;
};

static int switch_describe(const struct hiot_cap *cap, char *buf, size_t len)
{
	return snprintk(buf, len, "{\"type\":\"%s\",\"name\":\"%s\"}", cap->type, cap->name);
}

static int switch_get_state(const struct hiot_cap *cap, char *buf, size_t len)
{
	const struct switch_data *data = cap->data;

	return snprintk(buf, len, "{\"on\":%s}", data->on ? "true" : "false");
}

static int switch_set(const struct hiot_cap *cap, const char *json, size_t len)
{
	ARG_UNUSED(cap);
	ARG_UNUSED(json);
	ARG_UNUSED(len);

	/* TODO (step 3): parse {"on": bool}. */
	return -ENOTSUP;
}

static const struct hiot_cap_api switch_api = {
	.describe = switch_describe,
	.get_state = switch_get_state,
	.set = switch_set,
};

#define SWITCH_TARGET(inst)                                                    \
	COND_CODE_1(DT_INST_NODE_HAS_PROP(inst, target),                       \
		    (DT_NODE_FULL_NAME(DT_INST_PHANDLE(inst, target))), (NULL))

#define SWITCH_INST(inst)                                                      \
	static const struct switch_config switch_cfg_##inst = {                \
		.target = SWITCH_TARGET(inst),                                 \
	};                                                                     \
	static struct switch_data switch_data_##inst;                          \
	HIOT_CAP_DEFINE(hiot_switch_##inst, DT_NODE_FULL_NAME(DT_DRV_INST(inst)), \
			"switch", &switch_api, &switch_cfg_##inst,             \
			&switch_data_##inst);

DT_INST_FOREACH_STATUS_OKAY(SWITCH_INST)
