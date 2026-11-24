/*
 * SPDX-License-Identifier: Apache-2.0
 */

#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>

#include <hiot/core.h>

LOG_MODULE_REGISTER(lamp, LOG_LEVEL_INF);

int main(void)
{
	int err = hiot_core_init();

	if (err) {
		LOG_ERR("hiot core init failed (%d)", err);
		return 0;
	}

	LOG_INF("lamp ready");
	return 0;
}
