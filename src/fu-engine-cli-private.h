/*
 * SPDX-License-Identifier: LGPL-2.1-or-later
 */

#pragma once

#include "fu-engine.h"

GPtrArray *
fu_engine_cli_get_install_devices(FuEngine *engine, FuDevice *device, GError **error);
