#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json.h>
#include "jsonrpc.h"

void jsonrpc(char *post_buf, char *resp_buf) {
	struct json_object *rpc = json_tokener_parse(post_buf);
	struct json_object *resp = json_object_new_object();
	struct json_object *action_obj = NULL, *method_obj = NULL, *params_obj = NULL;
	char *action = NULL, char *method = NULL, char *resp_string = NULL;

	if (!rpc) {
		resp_parse_err(resp);
		goto DONE;
	}

	if (!json_object_object_get_ex(rpc, "action", &action_obj) ||
		!json_object_object_get_ex(rpc, "method", &method_obj) ||
		!json_object_object_get_ex(rpc, "params", &params_obj)) {
		resp_invalid_request(resp);
	goto DONE;
	}

	if (json_object_get_type(action_obj) != json_type_string ||
		json_object_get_type(method_obj) != json_type_string) {
		resp_invalid_request(resp);
		goto DONE;
	}

	action = json_object_get_string(action_obj);

	if (!strcmp(action, "nvram")) {
		jsonrpc_nvram(resp, method, params_obj);
	} else if (!strcmp(action, "customSettings")) {
		jsonrpc_amngcustom(resp, method, params_obj);
	} else if (!strcmp(action, "actionScript")) {
		jsonrpc_action_script(resp, method, params_obj);
	} else if (!strcmp(action, "addonConfigJson")) {
		jsonrpc_addon_config_json(resp, method, params_obj);
	} else if (!strcmp(action, "addonConfig")) {
		jsonrpc_addon_config(resp, method, params_obj);
	} else if (!strcmp(action, "appCentreConfig")) {
		//jsonrpc_app_centre_config(resp, method, params_obj);
		printf("nothing here yet!!\r\n");
	} else {
		resp_action_not_found(resp);
		goto DONE;
	}

DONE:

#ifdef RPC_TEST
	resp_string = json_object_to_json_string_ext(resp, JSON_C_TO_STRING_PRETTY);
#else
	resp_string = json_object_to_json_string_ext(resp, JSON_C_TO_STRING_PLAIN);
#endif

	memcpy(resp_buf, resp_string, sizeof(char) * strlen(resp_string));

	if (rpc)
		json_object_put(rpc);
	return;
}