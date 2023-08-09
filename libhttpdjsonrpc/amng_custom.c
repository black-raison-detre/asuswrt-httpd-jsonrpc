#include <string.h>
#include <json.h>
#include "jsonrpc.h"

void jsonrpc_amngcustom(struct json_object *resp, char *method, struct json_object *params) {
	struct json_object *custom_settings_list = NULL;
	struct json_object *resp_state_obj = NULL;
	enum JSON_RPC_RET resp_state;
	// file read write ops
	char file_path[128];
	enum FILE_OPS_RET file_ret;

	// Read /jffs/addons/custom_settings.txt for set get operation
	snprintf(file_path, sizeof(file_path), "%s%s", CONFIG_FILE_BASE_PATH, "custom_settings.txt");

	file_ret = file_read_amng_custom(custom_settings_list, file_path);

	switch (file_ret) { // read amng custom style file only have 3 ret type
		case FILE_OPS_RET_NOFILE:
			resp_file_not_found(resp);
			goto DONE;
		case FILE_OPS_RET_OVERSIZE:
			resp_file_oversize(resp);
			goto DONE;
		default:
			break;
	}

	json_rpc_common_config(custom_settings_list, resp, method, params);

	// get the resp state after common config ops
	json_object_object_get_ex(resp, "state", &resp_state_obj);
	resp_state = json_object_get_int(resp_state_obj);

	// except set and remove method all actions is handle by json_rpc_common_config
	// for set and remove, need to do a file write action
	if (!strcmp(method, "get")) {
		goto DONE;
	} else if (!strcmp(method, "set") || !strcmp(method, "remove")) {
		// check for resp state first
		if (resp_state == JSON_RPC_RET_OK) {
			
			// ok, now do file write
			file_ret = file_write_amng_custom(custom_settings_list, file_path);

			switch (file_ret) {
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
	if (custom_settings_list)
		json_object_put(custom_settings_list);
	return;
}

