#include "config.h"

#include "fu-context-private.h"
#include "fu-device-private.h"
#include "fu-genesys-hubhid-device.h"
#include "fu-genesys-usbhub-device.h"
#include "fu-genesys-usbhub-firmware.h"

static void
fu_genesys_usbhub_device_project_compatibility_matrix_func(void)
{
	const gchar *projects[] = {
	    "LNV_P40WD40_L10",
	    "LNV_P40WD40_L20",
	    "LNV_P40WD40_L30",
	};
	const gchar *mask_project_ic_types[] = {"352510", "352350", "352350"};

	for (guint device_idx = 0; device_idx < G_N_ELEMENTS(projects); device_idx++) {
		g_autoptr(GBytes) device_project =
		    g_bytes_new_static(projects[device_idx], strlen(projects[device_idx]));
		for (guint firmware_idx = 0; firmware_idx < G_N_ELEMENTS(projects);
		     firmware_idx++) {
			g_autoptr(GBytes) firmware_project =
			    g_bytes_new_static(projects[firmware_idx],
					       strlen(projects[firmware_idx]));
			g_autoptr(GError) error = NULL;
			gboolean ret = fu_genesys_usbhub_device_check_project_compatibility(
			    TRUE,
			    device_project,
			    mask_project_ic_types[device_idx],
			    firmware_project,
			    mask_project_ic_types[firmware_idx],
			    &error);

			if (device_idx == firmware_idx) {
				g_assert_no_error(error);
				g_assert_true(ret);
			} else {
				g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
				g_assert_false(ret);
			}
		}
	}
}

static void
fu_genesys_usbhub_device_project_compatibility_ic_mismatch_func(void)
{
	const gchar project[] = "LNV_P40WD40_L20";
	g_autoptr(GBytes) project_bytes = g_bytes_new_static(project, sizeof(project) - 1);
	g_autoptr(GError) error = NULL;
	gboolean ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
									    project_bytes,
									    "352350",
									    project_bytes,
									    "352510",
									    &error);

	g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
	g_assert_false(ret);
}

static void
fu_genesys_usbhub_device_project_compatibility_missing_data_func(void)
{
	const gchar project[] = "LNV_P40WD40_L10";
	g_autoptr(GBytes) project_bytes = g_bytes_new_static(project, sizeof(project) - 1);
	g_autoptr(GBytes) short_project = g_bytes_new_static("LNV_P40WD40_L1", 14);
	g_autoptr(GError) error_device_project = NULL;
	g_autoptr(GError) error_firmware_project = NULL;
	g_autoptr(GError) error_device_ic = NULL;
	g_autoptr(GError) error_firmware_ic = NULL;
	g_autoptr(GError) error_short_project = NULL;
	gboolean ret;

	ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
								   NULL,
								   "352510",
								   project_bytes,
								   "352510",
								   &error_device_project);
	g_assert_error(error_device_project, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
	g_assert_false(ret);

	ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
								   project_bytes,
								   "352510",
								   NULL,
								   "352510",
								   &error_firmware_project);
	g_assert_error(error_firmware_project, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
	g_assert_false(ret);

	ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
								   project_bytes,
								   NULL,
								   project_bytes,
								   "352510",
								   &error_device_ic);
	g_assert_error(error_device_ic, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
	g_assert_false(ret);

	ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
								   project_bytes,
								   "352510",
								   project_bytes,
								   NULL,
								   &error_firmware_ic);
	g_assert_error(error_firmware_ic, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
	g_assert_false(ret);

	ret = fu_genesys_usbhub_device_check_project_compatibility(TRUE,
								   short_project,
								   "352510",
								   project_bytes,
								   "352510",
								   &error_short_project);
	g_assert_error(error_short_project, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
	g_assert_false(ret);
}

static void
fu_genesys_usbhub_device_project_compatibility_non_p40_func(void)
{
	g_autoptr(GError) error = NULL;
	gboolean ret = fu_genesys_usbhub_device_check_project_compatibility(FALSE,
									    NULL,
									    NULL,
									    NULL,
									    NULL,
									    &error);

	g_assert_no_error(error);
	g_assert_true(ret);
}

static void
fu_genesys_usbhub_device_p40_quirk_activation_func(void)
{
	const gchar *p40_instance_ids[] = {
	    "USB\\VID_17EF&PID_1151",
	    "USB\\VID_17EF&PID_1153",
	    "USB\\VID_17EF&PID_1155",
	};
	g_autofree gchar *quirks_dir = g_strdup(SRCDIR);
	g_autoptr(FuContext) ctx = g_object_new(FU_TYPE_CONTEXT, NULL);
	g_autoptr(FuProgress) progress = fu_progress_new(G_STRLOC);
	g_autoptr(GError) error = NULL;
	gboolean ret;

	fu_context_set_path(ctx, FU_PATH_KIND_DATADIR_QUIRKS, quirks_dir);
	fu_context_add_flag(ctx, FU_CONTEXT_FLAG_NO_CACHE);
	ret = fu_context_load(ctx, progress, FU_CONTEXT_LOAD_FLAG_NONE, &error);
	g_assert_no_error(error);
	g_assert_true(ret);

	for (guint i = 0; i < G_N_ELEMENTS(p40_instance_ids); i++) {
		g_autoptr(FuDevice) device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		fu_device_set_context(device, ctx);
		fu_device_add_instance_id(device, p40_instance_ids[i]);
		g_assert_true(
		    fu_device_has_private_flag(device,
					       FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK));
		/* only the L1 hub (1151) answers vendor commands through its HID alone */
		g_assert_cmpint(
		    fu_device_has_private_flag(device, FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT),
		    ==,
		    i == 0);
		/* every P40 hub leaves isp mode before it is reset */
		g_assert_true(
		    fu_device_has_private_flag(device,
					       FU_GENESYS_USBHUB_FLAG_EXIT_ISP_BEFORE_RESET));
		g_assert_cmpuint(fu_device_get_specialized_gtype(device),
				 ==,
				 FU_TYPE_GENESYS_USBHUB_DEVICE);
		/* genesys.quirk provides the replug delay */
		g_assert_cmpuint(fu_device_get_remove_delay(device),
				 ==,
				 FU_DEVICE_REMOVE_DELAY_USER_REPLUG);

		/* no quirk value: fall back to the P40 delay */
		fu_device_set_remove_delay(device, 5000);
		fu_genesys_usbhub_device_apply_p40_remove_delay(FU_GENESYS_USBHUB_DEVICE(device));
		g_assert_cmpuint(fu_device_get_remove_delay(device),
				 ==,
				 FU_DEVICE_REMOVE_DELAY_USER_REPLUG);

		{
			g_autoptr(FuDevice) replacement =
			    g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
			FuDeviceClass *device_class = FU_DEVICE_GET_CLASS(replacement);
			fu_device_set_context(replacement, ctx);
			fu_device_add_instance_id(replacement, p40_instance_ids[i]);
			fu_device_set_remove_delay(replacement, 5000);
			g_assert_nonnull(device_class->replace);
			device_class->replace(replacement, device);
			g_assert_cmpuint(fu_device_get_remove_delay(replacement),
					 ==,
					 FU_DEVICE_REMOVE_DELAY_USER_REPLUG);
		}

		/* an explicit quirk RemoveDelay stays authoritative */
		{
			g_autoptr(FuDevice) tuned =
			    g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
			g_autoptr(FuDevice) replacement =
			    g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
			FuDeviceClass *device_class = FU_DEVICE_GET_CLASS(replacement);
			fu_device_set_context(tuned, ctx);
			fu_device_add_instance_id(tuned, p40_instance_ids[i]);
			ret = fu_device_set_quirk_kv(tuned,
						     FU_QUIRKS_REMOVE_DELAY,
						     "120000",
						     FU_CONTEXT_QUIRK_SOURCE_DB,
						     &error);
			g_assert_no_error(error);
			g_assert_true(ret);
			fu_genesys_usbhub_device_apply_p40_remove_delay(
			    FU_GENESYS_USBHUB_DEVICE(tuned));
			g_assert_cmpuint(fu_device_get_remove_delay(tuned), ==, 120000);

			fu_device_set_context(replacement, ctx);
			fu_device_add_instance_id(replacement, p40_instance_ids[i]);
			ret = fu_device_set_quirk_kv(replacement,
						     FU_QUIRKS_REMOVE_DELAY,
						     "120000",
						     FU_CONTEXT_QUIRK_SOURCE_DB,
						     &error);
			g_assert_no_error(error);
			g_assert_true(ret);
			device_class->replace(replacement, device);
			g_assert_cmpuint(fu_device_get_remove_delay(replacement), ==, 120000);
		}
	}

	{
		g_autoptr(FuDevice) device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		g_autoptr(FuDevice) donor = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		FuDeviceClass *device_class = FU_DEVICE_GET_CLASS(device);
		fu_device_set_context(device, ctx);
		fu_device_add_instance_id(device, "USB\\VID_17EF&PID_1159");
		g_assert_false(
		    fu_device_has_private_flag(device,
					       FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK));
		g_assert_false(
		    fu_device_has_private_flag(device, FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT));
		g_assert_false(
		    fu_device_has_private_flag(device,
					       FU_GENESYS_USBHUB_FLAG_EXIT_ISP_BEFORE_RESET));
		g_assert_cmpuint(fu_device_get_specialized_gtype(device), ==, G_TYPE_INVALID);
		fu_genesys_usbhub_device_apply_p40_remove_delay(FU_GENESYS_USBHUB_DEVICE(device));
		g_assert_cmpuint(fu_device_get_remove_delay(device), ==, 5000);

		fu_device_set_context(donor, ctx);
		fu_device_add_instance_id(donor, "USB\\VID_17EF&PID_1159");
		fu_device_set_remove_delay(device, 5000);
		g_assert_nonnull(device_class->replace);
		device_class->replace(device, donor);
		g_assert_cmpuint(fu_device_get_remove_delay(device), ==, 5000);
	}
}

static void
fu_genesys_usbhub_device_p40_hid_quirk_func(void)
{
	const gchar *hid_instance_ids[] = {
	    "USB\\VID_17EF&PID_F00F", /* L1 */
	    "USB\\VID_17EF&PID_4C41", /* L2 */
	    "USB\\VID_17EF&PID_4C42", /* L3 */
	};
	g_autofree gchar *quirks_dir = g_strdup(SRCDIR);
	g_autoptr(FuContext) ctx = g_object_new(FU_TYPE_CONTEXT, NULL);
	g_autoptr(FuProgress) progress = fu_progress_new(G_STRLOC);
	g_autoptr(GError) error = NULL;
	gboolean ret;

	/* the quirk GType is looked up by name, so the type must be registered */
	g_type_ensure(FU_TYPE_GENESYS_HUBHID_DEVICE);

	fu_context_set_path(ctx, FU_PATH_KIND_DATADIR_QUIRKS, quirks_dir);
	fu_context_add_flag(ctx, FU_CONTEXT_FLAG_NO_CACHE);
	ret = fu_context_load(ctx, progress, FU_CONTEXT_LOAD_FLAG_NONE, &error);
	g_assert_no_error(error);
	g_assert_true(ret);

	for (guint i = 0; i < G_N_ELEMENTS(hid_instance_ids); i++) {
		g_autoptr(FuDevice) device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		fu_device_set_context(device, ctx);
		fu_device_add_instance_id(device, hid_instance_ids[i]);
		g_assert_cmpuint(fu_device_get_specialized_gtype(device),
				 ==,
				 FU_TYPE_GENESYS_HUBHID_DEVICE);
	}
}

/*
 * HID proxy safety gate. Expected values come from the verified P40WD-40 topology:
 * L1 1151<->f00f, L2 1153<->4c41, L3 1155<->4c42. No test here touches hardware.
 */
typedef struct {
	const gchar *hub_instance_id;
	guint16 hub_pid;
	guint16 hid_pid;
} FuGenesysTestPair;

static const FuGenesysTestPair fu_genesys_test_pairs[] = {
    {"USB\\VID_17EF&PID_1151", 0x1151, 0xF00F},
    {"USB\\VID_17EF&PID_1153", 0x1153, 0x4C41},
    {"USB\\VID_17EF&PID_1155", 0x1155, 0x4C42},
};

#define FU_GENESYS_TEST_HUB_PHYSICAL_ID	  "usb:01:04"
#define FU_GENESYS_TEST_OTHER_PHYSICAL_ID "usb:01:07"

static FuContext *
fu_genesys_test_context_new(void)
{
	g_autofree gchar *quirks_dir = g_strdup(SRCDIR);
	g_autoptr(FuContext) ctx = g_object_new(FU_TYPE_CONTEXT, NULL);
	g_autoptr(FuProgress) progress = fu_progress_new(G_STRLOC);
	g_autoptr(GError) error = NULL;
	gboolean ret;

	g_type_ensure(FU_TYPE_GENESYS_HUBHID_DEVICE);
	fu_context_add_quirk_key(ctx, "GenesysHidProxyPid");
	fu_context_set_path(ctx, FU_PATH_KIND_DATADIR_QUIRKS, quirks_dir);
	fu_context_add_flag(ctx, FU_CONTEXT_FLAG_NO_CACHE);
	ret = fu_context_load(ctx, progress, FU_CONTEXT_LOAD_FLAG_NONE, &error);
	g_assert_no_error(error);
	g_assert_true(ret);
	return g_steal_pointer(&ctx);
}

static FuDevice *
fu_genesys_test_hub_new(FuContext *ctx, const gchar *instance_id, guint16 pid)
{
	FuDevice *device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
	fu_device_set_context(device, ctx);
	fu_device_add_instance_id(device, instance_id);
	fu_device_set_vid(device, 0x17EF);
	fu_device_set_pid(device, pid);
	fu_device_set_physical_id(device, FU_GENESYS_TEST_HUB_PHYSICAL_ID);
	return device;
}

static FuDevice *
fu_genesys_test_hid_new(guint16 vid, guint16 pid)
{
	FuDevice *device = g_object_new(FU_TYPE_GENESYS_HUBHID_DEVICE, NULL);
	fu_device_set_vid(device, vid);
	fu_device_set_pid(device, pid);
	return device;
}

static void
fu_genesys_test_assert_gate_error(GError *error, FwupdError code)
{
	g_assert_error(error, FWUPD_ERROR, code);
	g_assert_nonnull(g_strstr_len(error->message, -1, "HID proxy"));
}

/* every entry that can change hardware state must refuse before sending any command */
static void
fu_genesys_test_assert_entries_refuse(FuDevice *hub, FwupdError code)
{
	FuDeviceClass *klass = FU_DEVICE_GET_CLASS(hub);
	g_autoptr(FuProgress) progress = fu_progress_new(G_STRLOC);
	g_autoptr(FuFirmware) firmware = fu_firmware_new();

	{
		g_autoptr(GError) error = NULL;
		g_assert_false(klass->detach(hub, progress, &error));
		fu_genesys_test_assert_gate_error(error, code);
	}
	{
		g_autoptr(GError) error = NULL;
		g_assert_false(klass->prepare(hub, progress, FWUPD_INSTALL_FLAG_NONE, &error));
		fu_genesys_test_assert_gate_error(error, code);
	}
	{
		g_autoptr(GError) error = NULL;
		g_assert_false(klass->write_firmware(hub,
						     firmware,
						     progress,
						     FWUPD_INSTALL_FLAG_NONE,
						     &error));
		fu_genesys_test_assert_gate_error(error, code);
	}
	{
		g_autoptr(GError) error = NULL;
		g_autoptr(GBytes) blob = klass->dump_firmware(hub, progress, &error);
		g_assert_null(blob);
		fu_genesys_test_assert_gate_error(error, code);
	}
}

/* acceptance 1: no proxy -> inhibited and every state-changing entry refuses */
static void
fu_genesys_usbhub_device_hid_proxy_missing_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();

	for (guint i = 0; i < G_N_ELEMENTS(fu_genesys_test_pairs); i++) {
		g_autoptr(FuDevice) hub =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[i].hub_instance_id,
					    fu_genesys_test_pairs[i].hub_pid);
		g_autoptr(GError) error = NULL;

		g_assert_true(
		    fu_device_has_private_flag(hub, FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY));

		/* what setup does for a hub that needs the HID; init() already made it updatable */
		g_assert_true(fu_device_has_flag(hub, FWUPD_DEVICE_FLAG_UPDATABLE));
		fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FU_GENESYS_USBHUB_DEVICE(hub));
		g_assert_true(fu_device_has_inhibit(hub, "hid-proxy"));
		g_assert_false(fu_device_has_flag(hub, FWUPD_DEVICE_FLAG_UPDATABLE));
		g_assert_true(fu_device_has_flag(hub, FWUPD_DEVICE_FLAG_UPDATABLE_HIDDEN));

		g_assert_false(
		    fu_genesys_usbhub_device_verify_hid_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
							      NULL,
							      NULL,
							      &error));
		fu_genesys_test_assert_gate_error(error, FWUPD_ERROR_NOT_FOUND);

		fu_genesys_test_assert_entries_refuse(hub, FWUPD_ERROR_NOT_FOUND);
	}
}

/* acceptance 2: the right HID bound to its own hub is allowed (decision logic, no hardware) */
static void
fu_genesys_usbhub_device_hid_proxy_correct_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();

	for (guint i = 0; i < G_N_ELEMENTS(fu_genesys_test_pairs); i++) {
		g_autoptr(FuDevice) hub =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[i].hub_instance_id,
					    fu_genesys_test_pairs[i].hub_pid);
		g_autoptr(FuDevice) hid =
		    fu_genesys_test_hid_new(0x17EF, fu_genesys_test_pairs[i].hid_pid);
		g_autoptr(GError) error = NULL;

		fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FU_GENESYS_USBHUB_DEVICE(hub));
		g_assert_true(fu_device_has_inhibit(hub, "hid-proxy"));

		g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								 hid,
								 FU_GENESYS_TEST_HUB_PHYSICAL_ID,
								 &error));
		g_assert_no_error(error);
		g_assert_true(fu_device_get_proxy(hub, NULL) == hid);
		g_assert_false(fu_device_has_inhibit(hub, "hid-proxy"));
		g_assert_true(fu_device_has_flag(hub, FWUPD_DEVICE_FLAG_UPDATABLE));
		g_assert_true(
		    fu_genesys_usbhub_device_verify_hid_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
							      hid,
							      FU_GENESYS_TEST_HUB_PHYSICAL_ID,
							      &error));
		g_assert_no_error(error);

		/* a proxy whose parent cannot be resolved is not trusted (fail closed) */
		fu_genesys_test_assert_entries_refuse(hub, FWUPD_ERROR_INVALID_DATA);
	}
}

/* acceptance 3: a proxy that belongs to another hub, port or vendor is refused */
static void
fu_genesys_usbhub_device_hid_proxy_wrong_hub_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();

	for (guint i = 0; i < G_N_ELEMENTS(fu_genesys_test_pairs); i++) {
		for (guint j = 0; j < G_N_ELEMENTS(fu_genesys_test_pairs); j++) {
			g_autoptr(FuDevice) hub =
			    fu_genesys_test_hub_new(ctx,
						    fu_genesys_test_pairs[i].hub_instance_id,
						    fu_genesys_test_pairs[i].hub_pid);
			g_autoptr(FuDevice) hid =
			    fu_genesys_test_hid_new(0x17EF, fu_genesys_test_pairs[j].hid_pid);
			g_autoptr(GError) error = NULL;
			gboolean ret;

			fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(
			    FU_GENESYS_USBHUB_DEVICE(hub));
			ret = fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								 hid,
								 FU_GENESYS_TEST_HUB_PHYSICAL_ID,
								 &error);
			if (i == j) {
				g_assert_no_error(error);
				g_assert_true(ret);
				continue;
			}
			/* only the diagonal pairs are valid */
			g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
			g_assert_false(ret);
			g_assert_null(fu_device_get_proxy(hub, NULL));
			g_assert_true(fu_device_has_inhibit(hub, "hid-proxy"));
		}
	}

	/* the right HID model, but enumerated below a different port */
	{
		g_autoptr(FuDevice) hub =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[1].hub_instance_id,
					    fu_genesys_test_pairs[1].hub_pid);
		g_autoptr(FuDevice) hid = fu_genesys_test_hid_new(0x17EF, 0x4C41);
		g_autoptr(GError) error = NULL;

		fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FU_GENESYS_USBHUB_DEVICE(hub));
		g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								  hid,
								  FU_GENESYS_TEST_OTHER_PHYSICAL_ID,
								  &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
		g_assert_null(fu_device_get_proxy(hub, NULL));
		g_assert_true(fu_device_has_inhibit(hub, "hid-proxy"));
	}

	/* the right model number from another vendor */
	{
		g_autoptr(FuDevice) hub =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[2].hub_instance_id,
					    fu_genesys_test_pairs[2].hub_pid);
		g_autoptr(FuDevice) hid = fu_genesys_test_hid_new(0x05E3, 0x4C42);
		g_autoptr(GError) error = NULL;

		g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								  hid,
								  FU_GENESYS_TEST_HUB_PHYSICAL_ID,
								  &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
	}

	/* an unresolvable parent or hub id cannot be verified, so it is refused */
	{
		g_autoptr(FuDevice) hub =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[0].hub_instance_id,
					    fu_genesys_test_pairs[0].hub_pid);
		g_autoptr(FuDevice) hid = fu_genesys_test_hid_new(0x17EF, 0xF00F);
		g_autoptr(GError) error = NULL;

		g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								  hid,
								  NULL,
								  &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
		g_assert_null(fu_device_get_proxy(hub, NULL));
	}
}

/* acceptance 4: a late HID, a replaced hub and a moved HID are verified again */
static void
fu_genesys_usbhub_device_hid_proxy_replug_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();
	g_autoptr(FuDevice) hub = fu_genesys_test_hub_new(ctx,
							  fu_genesys_test_pairs[2].hub_instance_id,
							  fu_genesys_test_pairs[2].hub_pid);
	g_autoptr(FuDevice) hid = fu_genesys_test_hid_new(0x17EF, 0x4C42);
	g_autoptr(GError) error = NULL;

	/* the hub is up before its HID: not allowed yet */
	fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FU_GENESYS_USBHUB_DEVICE(hub));
	g_assert_true(fu_device_has_inhibit(hub, "hid-proxy"));
	fu_genesys_test_assert_entries_refuse(hub, FWUPD_ERROR_NOT_FOUND);

	/* the HID shows up late: bound after verification only */
	g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
							 hid,
							 FU_GENESYS_TEST_HUB_PHYSICAL_ID,
							 &error));
	g_assert_no_error(error);
	g_assert_false(fu_device_has_inhibit(hub, "hid-proxy"));

	/* the HID is re-enumerated below another port: verified again at the point of use */
	g_assert_false(fu_genesys_usbhub_device_verify_hid_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								 hid,
								 FU_GENESYS_TEST_OTHER_PHYSICAL_ID,
								 &error));
	fu_genesys_test_assert_gate_error(error, FWUPD_ERROR_INVALID_DATA);
	g_clear_error(&error);

	/* the hub is replugged: the new object starts inhibited without the old proxy */
	{
		g_autoptr(FuDevice) hub_new =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[2].hub_instance_id,
					    fu_genesys_test_pairs[2].hub_pid);
		FuDeviceClass *klass = FU_DEVICE_GET_CLASS(hub_new);

		klass->replace(hub_new, hub);
		fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(
		    FU_GENESYS_USBHUB_DEVICE(hub_new));
		g_assert_null(fu_device_get_proxy(hub_new, NULL));
		g_assert_true(fu_device_has_inhibit(hub_new, "hid-proxy"));
		g_assert_false(fu_device_has_flag(hub_new, FWUPD_DEVICE_FLAG_UPDATABLE));
		fu_genesys_test_assert_entries_refuse(hub_new, FWUPD_ERROR_NOT_FOUND);

		/* the HID is bound to the new hub again only through verification */
		g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_new),
								  hid,
								  FU_GENESYS_TEST_OTHER_PHYSICAL_ID,
								  &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
		g_clear_error(&error);
		g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_new),
								 hid,
								 FU_GENESYS_TEST_HUB_PHYSICAL_ID,
								 &error));
		g_assert_no_error(error);
		g_assert_false(fu_device_has_inhibit(hub_new, "hid-proxy"));
	}
}

/* acceptance 5: hubs that do not ask for the gate (and 1153/1155 setup flags) are unchanged */
static void
fu_genesys_usbhub_device_hid_proxy_no_regression_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();
	g_autoptr(FuDevice) hub = fu_genesys_test_hub_new(ctx, "USB\\VID_17EF&PID_1159", 0x1159);
	g_autoptr(FuDevice) hid = fu_genesys_test_hid_new(0x05E3, 0x0102);
	g_autoptr(GError) error = NULL;

	/* a hub without the flag is never inhibited and never verified */
	g_assert_false(fu_device_has_private_flag(hub, FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY));
	fu_genesys_usbhub_device_refresh_hid_proxy_inhibit(FU_GENESYS_USBHUB_DEVICE(hub));
	g_assert_false(fu_device_has_inhibit(hub, "hid-proxy"));
	g_assert_true(fu_genesys_usbhub_device_verify_hid_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
								NULL,
								NULL,
								&error));
	g_assert_no_error(error);
	g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub),
							 hid,
							 "usb:unrelated",
							 &error));
	g_assert_no_error(error);
	g_assert_true(fu_device_get_proxy(hub, NULL) == hid);

	/* P40 setup flags are untouched; only the gate flag is new */
	for (guint i = 0; i < G_N_ELEMENTS(fu_genesys_test_pairs); i++) {
		g_autoptr(FuDevice) p40 =
		    fu_genesys_test_hub_new(ctx,
					    fu_genesys_test_pairs[i].hub_instance_id,
					    fu_genesys_test_pairs[i].hub_pid);
		g_assert_true(
		    fu_device_has_private_flag(p40, FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY));
		g_assert_true(
		    fu_device_has_private_flag(p40,
					       FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK));
		g_assert_true(
		    fu_device_has_private_flag(p40, FU_GENESYS_USBHUB_FLAG_EXIT_ISP_BEFORE_RESET));
		g_assert_cmpint(
		    fu_device_has_private_flag(p40, FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT),
		    ==,
		    i == 0);
	}
}

/*
 * Offline identity tests. Every PROJECT string, IC type and PID below is synthetic test data,
 * not a statement about real hardware. Only an exact PROJECT + mask IC match is accepted,
 * anything else, including unknown identity, is refused.
 */
typedef struct {
	const gchar *project;
	const gchar *ic;
} FuGenesysTestIdentity;

static const FuGenesysTestIdentity fu_genesys_test_identities[] = {
    {"LNV_P40WD40_L10", "352510"},
    {"LNV_P40WD40_L20", "352350"},
    {"LNV_P40WD40_L30", "352350"},
    /* synthetic: the IC types are invented */
    {"LNV_P34WD40_L10", "352520"},
    {"LNV_P34WD40_L20", "352360"},
    {"LNV_P34WD40_L30", "352360"},
    /* synthetic: two models share one IC type, so the PROJECT string alone must separate them */
    {"SYN_COLLIDE_A10", "999001"},
    {"SYN_COLLIDE_B10", "999001"},
};

/* no two identities accept each other's firmware, including identities that share an IC type */
static void
fu_genesys_usbhub_device_identity_cross_model_func(void)
{
	for (guint d = 0; d < G_N_ELEMENTS(fu_genesys_test_identities); d++) {
		g_autoptr(GBytes) dev =
		    g_bytes_new_static(fu_genesys_test_identities[d].project,
				       strlen(fu_genesys_test_identities[d].project));
		for (guint f = 0; f < G_N_ELEMENTS(fu_genesys_test_identities); f++) {
			g_autoptr(GBytes) fw =
			    g_bytes_new_static(fu_genesys_test_identities[f].project,
					       strlen(fu_genesys_test_identities[f].project));
			g_autoptr(GError) error = NULL;
			gboolean ret = fu_genesys_usbhub_device_check_project_compatibility(
			    TRUE,
			    dev,
			    fu_genesys_test_identities[d].ic,
			    fw,
			    fu_genesys_test_identities[f].ic,
			    &error);
			if (d == f) {
				g_assert_no_error(error);
				g_assert_true(ret);
			} else {
				g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
				g_assert_false(ret);
			}
		}
	}

	/* same PROJECT, other mask IC: refused; same mask IC, other PROJECT: refused */
	{
		g_autoptr(GBytes) project = g_bytes_new_static("LNV_P34WD40_L20", 15);
		g_autoptr(GBytes) other = g_bytes_new_static("LNV_P40WD40_L20", 15);
		g_autoptr(GError) error_ic = NULL;
		g_autoptr(GError) error_project = NULL;

		g_assert_false(fu_genesys_usbhub_device_check_project_compatibility(TRUE,
										    project,
										    "352360",
										    project,
										    "352350",
										    &error_ic));
		g_assert_error(error_ic, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
		g_assert_false(
		    fu_genesys_usbhub_device_check_project_compatibility(TRUE,
									 project,
									 "352350",
									 other,
									 "352350",
									 &error_project));
		g_assert_error(error_project, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
	}
}

/* unknown or malformed identity is refused on both sides; there is no fallback to a looser match */
static void
fu_genesys_usbhub_device_identity_unknown_identity_func(void)
{
	const gsize lengths[] = {0, 1, 14, 16, 64};
	g_autoptr(GBytes) good = g_bytes_new_static("LNV_P34WD40_L20", 15);
	g_autoptr(GError) error_ok = NULL;

	/* control: identical identities are accepted, so refusals below are not fixture noise */
	g_assert_true(fu_genesys_usbhub_device_check_project_compatibility(TRUE,
									   good,
									   "352360",
									   good,
									   "352360",
									   &error_ok));
	g_assert_no_error(error_ok);

	for (guint i = 0; i < G_N_ELEMENTS(lengths); i++) {
		g_autofree guint8 *buf = g_malloc(lengths[i] + 1);
		g_autoptr(GBytes) odd = NULL;
		g_autoptr(GError) error_dev = NULL;
		g_autoptr(GError) error_fw = NULL;

		/* g_bytes_new() copies its input: fill the buffer first */
		memset(buf, 'X', lengths[i] + 1);
		odd = g_bytes_new(buf, lengths[i]);
		g_assert_cmpuint(g_bytes_get_size(odd), ==, lengths[i]);

		g_assert_false(fu_genesys_usbhub_device_check_project_compatibility(TRUE,
										    odd,
										    "352360",
										    good,
										    "352360",
										    &error_dev));
		g_assert_error(error_dev, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
		g_assert_false(fu_genesys_usbhub_device_check_project_compatibility(TRUE,
										    good,
										    "352360",
										    odd,
										    "352360",
										    &error_fw));
		g_assert_error(error_fw, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
	}

	/* mask IC type that is not six characters */
	{
		const gchar *bad_ics[] = {"", "35236", "3523600"};
		for (guint i = 0; i < G_N_ELEMENTS(bad_ics); i++) {
			g_autoptr(GError) error_dev = NULL;
			g_autoptr(GError) error_fw = NULL;
			g_assert_false(
			    fu_genesys_usbhub_device_check_project_compatibility(TRUE,
										 good,
										 bad_ics[i],
										 good,
										 "352360",
										 &error_dev));
			g_assert_error(error_dev, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
			g_assert_false(
			    fu_genesys_usbhub_device_check_project_compatibility(TRUE,
										 good,
										 "352360",
										 good,
										 bad_ics[i],
										 &error_fw));
			g_assert_error(error_fw, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);
		}
	}
}

/* the strict check is the flag, and no other flag combination relaxes it in check_firmware() */
static void
fu_genesys_usbhub_device_identity_check_firmware_not_relaxed_func(void)
{
	const gchar *others[] = {
	    FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT,
	    FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY,
	    FU_GENESYS_USBHUB_FLAG_EXIT_ISP_BEFORE_RESET,
	};

	for (guint mask = 0; mask < (1u << G_N_ELEMENTS(others)); mask++) {
		g_autoptr(FuDevice) device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		g_autoptr(FuFirmware) firmware = fu_firmware_new();
		g_autoptr(FuFirmware) genesys_fw =
		    g_object_new(FU_TYPE_GENESYS_USBHUB_FIRMWARE, NULL);
		g_autoptr(GError) error = NULL;
		g_autoptr(GError) error_genesys = NULL;
		FuDeviceClass *klass = FU_DEVICE_GET_CLASS(device);

		fu_device_add_private_flag(device, FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK);
		for (guint i = 0; i < G_N_ELEMENTS(others); i++) {
			if (mask & (1u << i))
				fu_device_add_private_flag(device, others[i]);
		}

		/* not a Genesys hub firmware, and the device exposes no identity at all */
		g_assert_nonnull(klass->check_firmware);
		g_assert_false(
		    klass->check_firmware(device, firmware, FU_FIRMWARE_PARSE_FLAG_NONE, &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_FILE);

		/* a Genesys hub firmware without device identity is refused (fail closed) */
		g_assert_false(klass->check_firmware(device,
						     genesys_fw,
						     FU_FIRMWARE_PARSE_FLAG_NONE,
						     &error_genesys));
		g_assert_error(error_genesys, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
	}
}

static FuDevice *
fu_genesys_test_synthetic_hub_new(FuContext *ctx,
				  const gchar *physical_id,
				  guint16 pid,
				  const gchar *hid_pid)
{
	g_autoptr(GError) error = NULL;
	FuDevice *hub = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);

	fu_device_set_context(hub, ctx);
	fu_device_set_vid(hub, 0x17EF);
	fu_device_set_pid(hub, pid);
	fu_device_set_physical_id(hub, physical_id);
	fu_device_add_private_flag(hub, FU_GENESYS_USBHUB_FLAG_REQUIRE_HID_PROXY);
	g_assert_true(fu_device_set_quirk_kv(hub,
					     "GenesysHidProxyPid",
					     hid_pid,
					     FU_CONTEXT_QUIRK_SOURCE_DB,
					     &error));
	g_assert_no_error(error);
	return hub;
}

/*
 * The HID VID/PIDs 17ef:f00f, 17ef:4c41 and 17ef:4c42 are shared between models. A HID may only
 * become the update channel of the hub it hangs below, never of another dock's hub or of a hub
 * that expects another HID model, even when the VID/PID looks right.
 */
static void
fu_genesys_usbhub_device_identity_shared_hid_own_dock_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();
	/* two docks, both with a hub that expects the shared 4c41 HID */
	g_autoptr(FuDevice) hub_a =
	    fu_genesys_test_synthetic_hub_new(ctx, "usb:01:04", 0x9901, "0x4C41");
	g_autoptr(FuDevice) hub_b =
	    fu_genesys_test_synthetic_hub_new(ctx, "usb:02:04", 0x9901, "0x4C41");
	g_autoptr(FuDevice) hid_a = fu_genesys_test_hid_new(0x17EF, 0x4C41);
	g_autoptr(FuDevice) hid_b = fu_genesys_test_hid_new(0x17EF, 0x4C41);
	g_autoptr(GError) error = NULL;

	/* each dock binds its own HID */
	g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_a),
							 hid_a,
							 "usb:01:04",
							 &error));
	g_assert_no_error(error);
	g_assert_true(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_b),
							 hid_b,
							 "usb:02:04",
							 &error));
	g_assert_no_error(error);
	g_assert_true(fu_device_get_proxy(hub_a, NULL) == hid_a);
	g_assert_true(fu_device_get_proxy(hub_b, NULL) == hid_b);
}

static void
fu_genesys_usbhub_device_identity_shared_hid_refused_func(void)
{
	g_autoptr(FuContext) ctx = fu_genesys_test_context_new();
	g_autoptr(FuDevice) hub_a =
	    fu_genesys_test_synthetic_hub_new(ctx, "usb:01:04", 0x9901, "0x4C41");
	g_autoptr(FuDevice) hub_other =
	    fu_genesys_test_synthetic_hub_new(ctx, "usb:03:04", 0x9902, "0x4C42");
	g_autoptr(FuDevice) hub_unconfigured =
	    fu_genesys_test_synthetic_hub_new(ctx, "usb:04:04", 0x9903, "0x0");
	g_autoptr(FuDevice) hid_4c41_dock_b = fu_genesys_test_hid_new(0x17EF, 0x4C41);
	g_autoptr(FuDevice) hid_4c41 = fu_genesys_test_hid_new(0x17EF, 0x4C41);
	g_autoptr(GError) error = NULL;

	/* dock B's 4c41 HID is never bound to dock A's hub */
	g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_a),
							  hid_4c41_dock_b,
							  "usb:02:04",
							  &error));
	g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
	g_assert_null(fu_device_get_proxy(hub_a, NULL));
	g_clear_error(&error);

	/* a hub of another model, same shared VID/PID family but a different expected PID */
	g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_other),
							  hid_4c41,
							  "usb:03:04",
							  &error));
	g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
	g_assert_null(fu_device_get_proxy(hub_other, NULL));
	g_clear_error(&error);

	/* no configured HID model: nothing can be bound, even the right-looking HID */
	g_assert_false(
	    fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(hub_unconfigured),
					       hid_4c41,
					       "usb:04:04",
					       &error));
	g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_NOT_SUPPORTED);
	g_assert_null(fu_device_get_proxy(hub_unconfigured, NULL));
	g_clear_error(&error);

	/* a P40 hub (1153 expects 4c41) never takes a 4c41 HID that sits below another dock */
	{
		g_autoptr(FuContext) ctx_p40 = fu_genesys_test_context_new();
		g_autoptr(FuDevice) p40 =
		    fu_genesys_test_hub_new(ctx_p40, "USB\\VID_17EF&PID_1153", 0x1153);

		g_assert_false(fu_genesys_usbhub_device_set_proxy(FU_GENESYS_USBHUB_DEVICE(p40),
								  hid_4c41,
								  FU_GENESYS_TEST_OTHER_PHYSICAL_ID,
								  &error));
		g_assert_error(error, FWUPD_ERROR, FWUPD_ERROR_INVALID_DATA);
		g_assert_null(fu_device_get_proxy(p40, NULL));
	}
}

int
main(int argc, char **argv)
{
	g_setenv("G_TEST_SRCDIR", SRCDIR, FALSE);
	g_test_init(&argc, &argv, NULL);
	g_test_add_func("/fwupd/genesys/project-compatibility/matrix",
			fu_genesys_usbhub_device_project_compatibility_matrix_func);
	g_test_add_func("/fwupd/genesys/project-compatibility/ic-mismatch",
			fu_genesys_usbhub_device_project_compatibility_ic_mismatch_func);
	g_test_add_func("/fwupd/genesys/project-compatibility/missing-data",
			fu_genesys_usbhub_device_project_compatibility_missing_data_func);
	g_test_add_func("/fwupd/genesys/project-compatibility/non-p40",
			fu_genesys_usbhub_device_project_compatibility_non_p40_func);
	g_test_add_func("/fwupd/genesys/quirk/p40-strict-project-check",
			fu_genesys_usbhub_device_p40_quirk_activation_func);
	g_test_add_func("/fwupd/genesys/quirk/p40-hid",
			fu_genesys_usbhub_device_p40_hid_quirk_func);
	g_test_add_func("/fwupd/genesys/hid-proxy/missing",
			fu_genesys_usbhub_device_hid_proxy_missing_func);
	g_test_add_func("/fwupd/genesys/hid-proxy/correct",
			fu_genesys_usbhub_device_hid_proxy_correct_func);
	g_test_add_func("/fwupd/genesys/hid-proxy/wrong-hub",
			fu_genesys_usbhub_device_hid_proxy_wrong_hub_func);
	g_test_add_func("/fwupd/genesys/hid-proxy/replug",
			fu_genesys_usbhub_device_hid_proxy_replug_func);
	g_test_add_func("/fwupd/genesys/hid-proxy/no-regression",
			fu_genesys_usbhub_device_hid_proxy_no_regression_func);
	g_test_add_func("/fwupd/genesys/identity/cross-model",
			fu_genesys_usbhub_device_identity_cross_model_func);
	g_test_add_func("/fwupd/genesys/identity/unknown-identity",
			fu_genesys_usbhub_device_identity_unknown_identity_func);
	g_test_add_func("/fwupd/genesys/identity/check-firmware-not-relaxed",
			fu_genesys_usbhub_device_identity_check_firmware_not_relaxed_func);
	g_test_add_func("/fwupd/genesys/identity/shared-hid-own-dock",
			fu_genesys_usbhub_device_identity_shared_hid_own_dock_func);
	g_test_add_func("/fwupd/genesys/identity/shared-hid-refused",
			fu_genesys_usbhub_device_identity_shared_hid_refused_func);
	return g_test_run();
}