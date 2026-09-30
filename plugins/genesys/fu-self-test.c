#include "config.h"

#include "fu-context-private.h"
#include "fu-genesys-hubhid-device.h"
#include "fu-genesys-usbhub-device.h"

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
		g_assert_cmpuint(fu_device_get_specialized_gtype(device),
				 ==,
				 FU_TYPE_GENESYS_USBHUB_DEVICE);
	}

	{
		g_autoptr(FuDevice) device = g_object_new(FU_TYPE_GENESYS_USBHUB_DEVICE, NULL);
		fu_device_set_context(device, ctx);
		fu_device_add_instance_id(device, "USB\\VID_17EF&PID_1159");
		g_assert_false(
		    fu_device_has_private_flag(device,
					       FU_GENESYS_USBHUB_FLAG_P40_STRICT_PROJECT_CHECK));
		g_assert_false(
		    fu_device_has_private_flag(device, FU_GENESYS_USBHUB_FLAG_HID_TRANSPORT));
		g_assert_cmpuint(fu_device_get_specialized_gtype(device), ==, G_TYPE_INVALID);
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
	return g_test_run();
}