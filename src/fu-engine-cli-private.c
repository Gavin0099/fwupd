/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#include "config.h"

#include "fu-engine-cli-private.h"

GPtrArray *
fu_engine_cli_get_install_devices(FuEngine *engine, FuDevice *device, GError **error)
{
	g_autoptr(GPtrArray) devices = NULL;
	const gchar *device_id;

	g_return_val_if_fail(FU_IS_ENGINE(engine), NULL);
	g_return_val_if_fail(FU_IS_DEVICE(device), NULL);
	g_return_val_if_fail(error == NULL || *error == NULL, NULL);

	device_id = fu_device_get_id(device);
	devices = fu_engine_get_devices_by_composite_id(engine,
							fu_device_get_composite_id(device),
							error);
	if (devices == NULL)
		return NULL;

	for (guint i = 0; i < devices->len; i++) {
		FuDevice *device_tmp = g_ptr_array_index(devices, i);
		if (g_strcmp0(fu_device_get_id(device_tmp), device_id) == 0)
			return g_steal_pointer(&devices);
	}

	g_ptr_array_add(devices, g_object_ref(device));
	return g_steal_pointer(&devices);
}
