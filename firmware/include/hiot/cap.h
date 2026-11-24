/*
 * SPDX-License-Identifier: Apache-2.0
 *
 * Capability interface. A device is a list of capabilities; each
 * capability instance comes from one devicetree node and registers
 * itself with HIOT_CAP_DEFINE().
 */

#ifndef HIOT_CAP_H_
#define HIOT_CAP_H_

#include <stddef.h>
#include <zephyr/sys/iterable_sections.h>

#ifdef __cplusplus
extern "C" {
#endif

struct hiot_cap;

struct hiot_cap_api {
	/** Optional one-time init (check devices, restore settings). */
	int (*init)(const struct hiot_cap *cap);

	/**
	 * Write a JSON object describing this capability (type, name, ranges)
	 * into @p buf. Returns the number of bytes written or -errno.
	 */
	int (*describe)(const struct hiot_cap *cap, char *buf, size_t len);

	/** Write the current state as a JSON object. Returns length or -errno. */
	int (*get_state)(const struct hiot_cap *cap, char *buf, size_t len);

	/**
	 * Apply a command given as a JSON object (validated against
	 * protocol/schemas/capabilities/<type>.schema.json).
	 * Local input and remote MQTT commands both go through here.
	 */
	int (*set)(const struct hiot_cap *cap, const char *json, size_t len);
};

struct hiot_cap {
	/** Instance name (devicetree node name, e.g. "lamp-brightness"). */
	const char *name;
	/** Capability type (e.g. "brightness"); matches a protocol schema. */
	const char *type;
	const struct hiot_cap_api *api;
	const void *config;
	void *data;
};

/**
 * Register a capability instance.
 *
 * @param _id     C identifier for the instance
 * @param _name   instance name string
 * @param _type   capability type string
 * @param _api    pointer to struct hiot_cap_api
 * @param _config pointer to instance config (const)
 * @param _data   pointer to instance runtime data
 */
#define HIOT_CAP_DEFINE(_id, _name, _type, _api, _config, _data)                \
	STRUCT_SECTION_ITERABLE(hiot_cap, _id) = {                             \
		.name = (_name),                                               \
		.type = (_type),                                               \
		.api = (_api),                                                 \
		.config = (_config),                                           \
		.data = (_data),                                               \
	}

/** Iterate over all registered capabilities. */
#define HIOT_CAP_FOREACH(_var) STRUCT_SECTION_FOREACH(hiot_cap, _var)

#ifdef __cplusplus
}
#endif

#endif /* HIOT_CAP_H_ */
