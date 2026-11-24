/*
 * SPDX-License-Identifier: Apache-2.0
 */

#ifndef HIOT_CAPS_BRIGHTNESS_H_
#define HIOT_CAPS_BRIGHTNESS_H_

#include <stdint.h>
#include <hiot/cap.h>

#define HIOT_BRIGHTNESS_MAX 100

/** Set brightness level 0..HIOT_BRIGHTNESS_MAX (0 = off). */
int hiot_brightness_set_level(const struct hiot_cap *cap, uint8_t level);

/** Get current brightness level. */
uint8_t hiot_brightness_get_level(const struct hiot_cap *cap);

#endif /* HIOT_CAPS_BRIGHTNESS_H_ */
