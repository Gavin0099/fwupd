/*
 * Copyright 2021 Ricardo Cañuelo <ricardo.canuelo@collabora.com>
 *
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include <fwupdplugin.h>

#define FU_TYPE_GENESYS_USBHUB_DEVICE			(fu_genesys_usbhub_device_get_type())
#define FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK "p40-strict-project-check"
#define FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT		"hid-transport"
G_DECLARE_FINAL_TYPE(FuGenesysUsbhubDevice,
		     fu_genesys_usbhub_device,
		     FU,
		     GENESYS_USBHUB_DEVICE,
		     FuUsbDevice)

void
fu_genesys_usbhub_device_set_proxy(FuGenesysUsbhubDevice *self, FuDevice *proxy);

gboolean
fu_genesys_usbhub_device_setup_flash_late(FuGenesysUsbhubDevice *self, GError **error);

gboolean
fu_genesys_usbhub_device_check_project_compatibility(gboolean strict,
						     GBytes *device_project,
						     const gchar *device_mask_project_ic_type,
						     GBytes *firmware_project,
						     const gchar *firmware_mask_project_ic_type,
						     GError **error);
