/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HIOT_CORE_H_
#define HIOT_CORE_H_

#include <stddef.h>
#include <hiot/cap.h>

#ifdef __cplusplus
extern "C" {
#endif

/** Initialize all registered capabilities. */
int hiot_core_init(void);

/** Number of registered capabilities. */
size_t hiot_cap_count(void);

/** Find a capability by instance name. Returns NULL if not found. */
struct hiot_cap *hiot_cap_find(const char *name);

/**
 * Get the device ID as a lowercase hex string (from hwinfo_get_device_id()).
 * Never hardcoded. Returns 0 or -errno.
 */
int hiot_device_id(char *buf, size_t len);

#ifdef __cplusplus
}
#endif

#endif /* HIOT_CORE_H_ */
