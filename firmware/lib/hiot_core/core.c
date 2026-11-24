/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Capability registry. Later: config/state publishing and command routing
 * (via zbus, bridged to MQTT by hiot_net).
 */

#include <errno.h>
#include <string.h>

#include <zephyr/drivers/hwinfo.h>
#include <zephyr/logging/log.h>
#include <zephyr/sys/util.h>

#include <hiot/core.h>

LOG_MODULE_REGISTER(hiot_core, CONFIG_HIOT_LOG_LEVEL);

int hiot_core_init(void)
{
	char buf[128];
	int err;

	err = hiot_device_id(buf, sizeof(buf));
	if (err == 0) {
		LOG_INF("device id %s, type %s, fw %s", buf, CONFIG_HIOT_DEVICE_TYPE,
			CONFIG_HIOT_FW_VERSION);
	} else {
		LOG_WRN("no device id (%d)", err);
	}

	HIOT_CAP_FOREACH(cap) {
		if (cap->api->init != NULL) {
			err = cap->api->init(cap);
			if (err) {
				LOG_ERR("cap %s init failed (%d)", cap->name, err);
				return err;
			}
		}

		if (cap->api->describe != NULL &&
		    cap->api->describe(cap, buf, sizeof(buf)) > 0) {
			LOG_INF("cap %s", buf);
		}
	}

	LOG_INF("%zu capabilities registered", hiot_cap_count());
	return 0;
}

size_t hiot_cap_count(void)
{
	size_t n = 0;

	HIOT_CAP_FOREACH(cap) {
		n++;
	}

	return n;
}

struct hiot_cap *hiot_cap_find(const char *name)
{
	HIOT_CAP_FOREACH(cap) {
		if (strcmp(cap->name, name) == 0) {
			return cap;
		}
	}

	return NULL;
}

int hiot_device_id(char *buf, size_t len)
{
	uint8_t id[16];
	ssize_t n;

	n = hwinfo_get_device_id(id, sizeof(id));
	if (n < 0) {
		return (int)n;
	}

	if (bin2hex(id, (size_t)n, buf, len) == 0) {
		return -ENOMEM;
	}

	return 0;
}
