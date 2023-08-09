#include <string.h>
#include <json.h>
#include "jsonrpc.h"


void jsonrpc_addon_config_json(struct json_object *resp, char *method, struct json_object *params
) {
	struct json_object *addon_setting_list = NULL;
	enum json_type params_type = json_object_get_type(params);
	enum json_type settings_type;
	// params processing
	struct json_object *addon_name_obj = NULL, *config_name_obj = NULL, *settings_obj = NULL;
	char *addon_name = NULL, *config_name = NULL, *setting_key = NULL;
	// resp state check
	struct json_object *resp_state_obj = NULL;
	enum JSON_RPC_RET resp_state;
	// file read write ops
	char file_path[128];
	enum FILE_OPS_RET file_ret;


	// params should be an object contain { addonName: "<addon name>", configName: "<config name>",
	// settings: [] or {} }
	if (params_type != json_type_object) {
		resp_invalid_params(resp);
		goto DONE;
	}
		
	if (!json_object_object_get_ex(params, "addonName", &addon_name_obj) ||
		!json_object_object_get_ex(params, "configName", &config_name_obj) ||
		!json_object_object_get_ex(params, "settings", &settings_obj)
		) {
		resp_invalid_params(resp);
		goto DONE;
	}

	if (json_object_get_type(addon_name_obj) != json_type_string ||
		json_object_get_type(config_name_obj) != json_type_string) {
		resp_invalid_params(resp);
		goto DONE;
	}

	addon_name = json_object_get_string(addon_name_obj);
	config_name = json_object_get_string(config_name_obj);

	if (strlen(addon_name) == 0 || strlen(config_name) == 0) {
		resp_invalid_params(resp);
		goto DONE;
	}

	// create file path
	snprintf(file_path, sizeof(file_path), "%s%s/configs/%s.json", CONFIG_FILE_BASE_PATH, addon_name, config_name);

	// read config file
	file_ret = file_read_json(addon_setting_list, file_path);

	switch (file_ret) { //
		case FILE_OPS_RET_MALLOC_FAIL:
		case FILE_OPS_RET_NOFILE:
			resp_file_not_found(resp);
			goto DONE;
		case FILE_OPS_RET_OVERSIZE:
			resp_file_oversize(resp);
			goto DONE;
		case FILE_OPS_RET_READ_FAIL:
			resp_file_invalid(resp);
			goto DONE;
		default:
			break;
	}

	json_rpc_common_config_json(addon_setting_list, resp, method, settings_obj);

	// get the resp state after common config ops
	json_object_object_get_ex(resp, "state", &resp_state_obj);
	resp_state = json_object_get_int(resp_state_obj);

	// handle by common_config_json
	if (!strcmp(method, "get") || !strcmp(method, "read")) {
		goto DONE;    
	} else if (!strcmp(method, "set") || !strcmp(method, "write")) {
		// need file write operation, check resp state first
		if (resp_state == JSON_RPC_RET_OK) {

			file_ret = file_write_json(addon_setting_list, file_path);

			switch (file_ret) {
				case FILE_OPS_RET_OVERSIZE:
					resp_file_oversize(resp);
					goto DONE;
				case FILE_OPS_RET_WRITE_FAIL:
					resp_file_write_fail(resp);
					goto DONE;
				default:
					break;
			}
			
		} else {
			goto DONE;
		}
	} else {
		goto DONE;
	}

DONE:
	if (addon_setting_list)
		json_object_put(addon_setting_list);
	return;
}

