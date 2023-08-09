#include <stdio.h>
#include <string.h>
#include <json.h>
#ifndef RPC_TEST
#include <bcmnvram.h>
#endif
#include "jsonrpc.h"

#ifdef RPC_TEST
static char *nvram_safe_get(const char *name) {
	printf("nvram_safe_get: %s\r\n", name);
	return "rpc_test_nvram_get";
}

static int nvram_set(const char *name, const char *value) {
	printf("nvram_set: %s=%s\r\n", name, value);
	return 0;
}

static int nvram_commit(void) {
	printf("nvram_commit\r\n");
	return 0;
}
#endif
static int nvram_set_int(const char *key, int value)
{
	char nvram_str[32];

	memset(nvram_str, 0, sizeof(nvram_str));
	snprintf(nvram_str, sizeof(nvram_str), "%d", value);
	return nvram_set(key, nvram_str);
}

void jsonrpc_nvram(struct json_object *resp, char *method, struct json_object *params) {
	enum json_type params_type = json_object_get_type(params);
	enum json_type nvram_val_type;
	struct json_object *nvram_result = json_object_new_object();
	struct json_object *nvram_to_apply = json_object_new_object();
	struct json_object *nvram_key_obj = NULL;
	int params_cnt = 0;
	char *nvram_key = NULL;

	if (!strcmp(method, "get")) {

		// get params should be string for single val, or array of string for multival
		if (params_type == json_type_string) { // single val
			nvram_key = json_object_get_string(params);

			json_object_object_add(nvram_result, nvram_key, json_object_new_string(nvram_safe_get(nvram_key)));
			json_object_object_add(resp, "result", nvram_result);

		} else if (params_type == json_type_array) { // multi vals
			params_cnt = json_object_array_length(params);

			if (params_cnt == 0) {
				resp_invalid_params(resp);
				goto DONE;
			}

			for (int i = 0; i < params_cnt; i++) {
				nvram_key_obj = json_object_array_get_idx(params, i);

				if (json_object_get_type(nvram_key_obj) != json_type_string) {
					resp_invalid_params(resp);
					goto DONE;
				}

				nvram_key = json_object_get_string(nvram_key_obj);

				if (nvram_key == NULL) {
					resp_invalid_params(resp);
					goto DONE;
				}

				json_object_object_add(nvram_result, nvram_key, json_object_new_string(nvram_safe_get(nvram_key)));
			}
			
			json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
			json_object_object_add(resp, "result", nvram_result);
			goto DONE;

		} else {
			resp_invalid_params(resp);
			goto DONE;
		}

	} else if (!strcmp(method, "set")) {

		// set should alway be key: value pair
		if (params_type == json_type_object) {

			// check received nvram apply val
			json_object_object_foreach(params, key, val) {
				nvram_val_type = json_object_get_type(val);
				if (nvram_val_type == json_type_string || nvram_val_type == json_type_int) { // ok, add to apply list
					json_object_object_add(nvram_to_apply, key, val);
				} else { // bad, resp err, do not apply
					resp_invalid_params(resp);
					goto DONE;
				}
			}

			// check completed, apply checked nvram vals
			json_object_object_foreach(nvram_to_apply, key, val) {
				nvram_val_type = json_object_get_type(val);
				if (nvram_val_type == json_type_string)
					nvram_set(key, val);
				if (nvram_val_type == json_type_int)
					nvram_set_int(key, val)
			}
			nvram_commit();

			json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
			// set return NULL result
			json_object_object_add(resp, "result", NULL);
			goto DONE;

		} else {
			resp_invalid_params(resp);
			goto DONE;
		}
	} else {
		resp_method_not_found(resp);
		goto DONE;
	}

DONE:
	if (nvram_result)
		json_object_put(nvram_result);
	if (nvram_to_apply)
		json_object_put(nvram_to_apply);
	if (nvram_key_obj)
		json_object_put(nvram_key_obj);
	return;
}