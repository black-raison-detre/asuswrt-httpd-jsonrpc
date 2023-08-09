#include <string.h>
#include <json.h>
#include "jsonrpc.h"

void resp_parse_err(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_PARSE_ERR));
	json_object_object_add(resp, "result", NULL);
}

void resp_invalid_request(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_INVALID_REQUEST));
	json_object_object_add(resp, "result", NULL);
}

void resp_action_not_found(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_ACTION_NOT_FOUND));
	json_object_object_add(resp, "result", NULL);
}

void resp_method_not_found(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_METHOD_NOT_FOUND));
	json_object_object_add(resp, "result", NULL);
}

void resp_invalid_params(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_INVALID_PARAMS));
	json_object_object_add(resp, "result", NULL);
}

void resp_file_not_found(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_FILE_NOT_FOUND));
	json_object_object_add(resp, "result", NULL);
}

void resp_file_write_fail(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_FILE_WRITE_FAIL));
	json_object_object_add(resp, "result", NULL);
}

void resp_file_oversize(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_FILE_OVERSIZE));
	json_object_object_add(resp, "result", NULL);
}

void resp_file_invalid(struct json_object *resp) {
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_FILE_INVALID));
	json_object_object_add(resp, "result", NULL);
}


// this common function is for amng_custom style config handling
void json_rpc_common_config(struct json_object *file, struct json_object *resp, char *method, struct json_object *params) {
	enum json_type params_type = json_object_get_type(params);
	// key object of params
	struct json_object *params_key_obj = NULL;
	// readout from config file
	struct json_object *config_obj = NULL;
	// resp result object for get
	struct json_object *get_result = json_object_new_object();
	// resp deleted list
	struct json_object *deleted_keys = json_object_new_array();
	char *params_key = NULL;
	int params_count = 0;

	if (!strcmp(method, "get")) {
		// get params should be a array of string
		if (params_type != json_type_array) {
			resp_invalid_params(resp);
			goto DONE;
		}

		params_count = json_object_array_length(params);

		if (params_count == 0) {
			resp_invalid_params(resp);
			goto DONE;
		}

		for (int i = 0; i < params_count; i++) {
			params_key_obj = json_object_array_get_idx(params, i);

			// get should be array of string
			if (json_object_get_type(params_key_obj) != json_type_string) {
				resp_invalid_params(resp);
				goto DONE;
			}

			params_key = json_object_get_string(params_key_obj);

			// found key in config file
			if (json_object_object_get_ex(file, params_key, &config_obj)) {
				// config_obj should be string as from the file read function
				json_object_object_add(get_result, params_key, config_obj);
			} else { // key not found, return a null val
				json_object_object_add(get_result, params_key, NULL);
			}
		}

		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", get_result);
		goto DONE;

	} else if (!strcmp(method, "set")) {
		// set params should be an object with key: val pairs
		if (params_type != json_type_object) {
			resp_invalid_params(resp);
			goto DONE;
		}

		json_object_object_foreach(params, key, val) {
			// set object val should be string only
			if (json_object_get_type(val) != json_type_string) {
				resp_invalid_params(resp);
				goto DONE;
			}

			// object_add is able to replace val, so this is ok
			json_object_object_add(file, key, val);
		}

		// set only resp state, result is null
		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", NULL);
		goto DONE;

	} else if (!strcmp(method, "remove")) {
		// remove params should be an string array
		if (params_type != json_type_array) {
			resp_invalid_params(resp);
			goto DONE;
		}

		params_count = json_object_array_length(params);

		if (params_count == 0) {
			resp_invalid_params(resp);
			goto DONE;
		}

		for (int i = 0; i < params_count; i++) {
			params_key_obj = json_object_array_get_idx(params, i);

			// remove should be array of string
			if (json_object_get_type(params_key_obj) != json_type_string) {
				resp_invalid_params(resp);
				goto DONE;
			}

			params_key = json_object_get_string(params_key_obj);
			
			// found the key in file
			if (json_object_object_get_ex(file, params_key)) {
				// key found, delete it, and add key to deleted list
				json_object_object_del(file, params_key);
				json_object_array_add(deleted_keys, params_key_obj);
			}
		}

		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", deleted_keys);
		goto DONE;

	} else {
		resp_method_not_found(resp);
		goto DONE;
	}
	
	DONE:
	if (get_result)
		json_object_put(get_result);
	if (deleted_keys)
		json_object_put(deleted_keys);
	return;
}

// this common function is for json config file handling
void json_rpc_common_config_json(struct json_object *file, struct json_object *resp, char *method, struct json_object *params) {
	enum json_type params_type = json_object_get_type(params);
	// key object of params
	struct json_object *params_key_obj = NULL;
	// readout from config file
	struct json_object *config_obj = NULL;
	// resp result object for get
	struct json_object *get_result = json_object_new_object();
	// resp deleted list
	struct json_object *deleted_keys = json_object_new_array();
	char *params_key = NULL;
	int params_count = 0;

	if (!strcmp(method, "get")) {
		// get params should be a array of string
		if (params_type != json_type_array) {
			resp_invalid_params(resp);
			goto DONE;
		}

		params_count = json_object_array_length(params);

		if (params_count == 0) {
			resp_invalid_params(resp);
			goto DONE;
		}

		for (int i = 0; i < params_count; i++) {
			params_key_obj = json_object_array_get_idx(params, i);

			// get should be array of string
			if (json_object_get_type(params_key_obj) != json_type_string) {
				resp_invalid_params(resp);
				goto DONE;
			}

			params_key = json_object_get_string(params_key_obj);

			// found key in config file
			if (json_object_object_get_ex(file, params_key, &config_obj)) {
				// config object could be any, but this should be fine
				json_object_object_add(get_result, params_key, config_obj);
			} else { // key not found, return a null val
				json_object_object_add(get_result, params_key, NULL);
			}
		}

		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", get_result);
		goto DONE;

	} else if (!strcmp(method, "set")) {
		// set params should be an object with key: val pairs
		if (params_type != json_type_object) {
			resp_invalid_params(resp);
			goto DONE;
		}

		json_object_object_foreach(params, key, val) {
			// set object val could be any
			// object_add is able to replace val, so this is ok
			json_object_object_add(file, key, val);
		}

		// set only resp state, result is null
		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", NULL);
		goto DONE;

	} else if (!strcmp(method, "remove")) {
		// remove params should be an string array
		if (params_type != json_type_array) {
			resp_invalid_params(resp);
			goto DONE;
		}

		params_count = json_object_array_length(params);

		if (params_count == 0) {
			resp_invalid_params(resp);
			goto DONE;
		}

		for (int i = 0; i < params_count; i++) {
			params_key_obj = json_object_array_get_idx(params, i);

			// remove should be array of string
			if (json_object_get_type(params_key_obj) != json_type_string) {
				resp_invalid_params(resp);
				goto DONE;
			}

			params_key = json_object_get_string(params_key_obj);
			
			// found the key in file
			if (json_object_object_get_ex(file, params_key)) {
				// key found, delete it, and add key to deleted list
				json_object_object_del(file, params_key);
				json_object_array_add(deleted_keys, params_key_obj);
			}
		}

		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", deleted_keys);
		goto DONE;

	} else if (!strcmp(method, "read")) {
		// this read the whole config file
		// not need to read setting object in params
		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", file);

	} else if (!strcmp(method, "write")) {
		// this overwrite the whole config file
		// return is null, just state to show if success
		if (json_object_get_type(params) != json_type_object) {
			resp_invalid_params(resp);
			goto DONE;
		}

		// free the readout file object and create an new one
		json_object_put(file);
		file = json_object_new_object();

		json_object_object_foreach(params, key, val) {
			json_object_object_add(file, key, val);
		}

		json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
		json_object_object_add(resp, "result", NULL);
		goto DONE;
	} else {
		resp_method_not_found(resp);
		goto DONE;
	}

DONE:
	if (get_result)
		json_object_put(get_result);
	if (deleted_keys)
		json_object_put(deleted_keys);

}


