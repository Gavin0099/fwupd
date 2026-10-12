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
#define FU_GENESYS_USBHUB_FLAG_EXIT_ISP_BEFORE_RESET	"exit-isp-before-reset"
#define FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY	"require-hid-proxy"
#define FU_GENESYS_USBHUB_INHIBIT_HID_PROXY		"hid-proxy"
G_DECLARE_FINAL_TYPE(FuGenesysUsbhubDevice,
		     fu_genesys_usbhub_device,
		     FU,
		     GENESYS_USBHUB_DEVICE,
		     FuUsbDevice)

gboolean
fu_genesys_usbhub_device_set_proxy(FuGenesysUsbhubDevice *self,
				   FuDevice *proxy,
				   const gchar *hid_parent_physical_id,
				   GError **error);

void
fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FuGenesysUsbhubDevice *self);

gboolean
fu_genesys_usbhub_device_verify_hid_proxy(FuGenesysUsbhubDevice *self,
					  FuDevice *proxy,
					  const gchar *hid_parent_physical_id,
					  GError **error);

gboolean
fu_genesys_usbhub_device_check_hid_binding(const gchar *hub_physical_id,
					   guint16 hub_vid,
					   guint16 expected_hid_pid,
					   const gchar *hid_parent_physical_id,
					   guint16 hid_vid,
					   guint16 hid_pid,
					   GError **error);

void
fu_genesys_usbhub_device_apply_p40_remove_delay(FuGenesysUsbhubDevice *self);

gboolean
fu_genesys_usbhub_device_setup_flash_late(FuGenesysUsbhubDevice *self, GError **error);

gboolean
fu_genesys_usbhub_device_check_project_compatibility(gboolean strict,
						     GBytes *device_project,
						     const gchar *device_mask_project_ic_type,
						     GBytes *firmware_project,
						     const gchar *firmware_mask_project_ic_type,
						     GError **error);
