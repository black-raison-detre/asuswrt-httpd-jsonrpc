#ifndef _jsonrpc_h_
#define _jsonrpc_h_
#endif

#define MAX_FILE_SIZE 8192 // 8KB
#ifdef RPC_TEST
#define CONFIG_FILE_BASE_PATH "./addons/"
#else
#define CONFIG_FILE_BASE_PATH "/jffs/addons/"
#endif

enum JSON_RPC_RET {
	JSON_RPC_RET_OK,
	JSON_RPC_RET_PARSE_ERR,
	JSON_RPC_RET_INVALID_REQUEST,
	JSON_RPC_RET_ACTION_NOT_FOUND,
	JSON_RPC_RET_METHOD_NOT_FOUND,
	JSON_RPC_RET_INVALID_METHOD,
	JSON_RPC_RET_INVALID_PARAMS,
	JSON_RPC_RET_FILE_OVERSIZE,
	JSON_RPC_RET_FILE_NOT_FOUND,
	JSON_RPC_RET_FILE_INVALID, // if it is a json file, amng file no errors
	JSON_RPC_RET_FILE_WRITE_FAIL
};

enum FILE_OPS_RET {
	FILE_OPS_RET_OK,
	FILE_OPS_RET_NOFILE,
	FILE_OPS_RET_OVERSIZE,
	FILE_OPS_RET_READ_FAIL, // json file format err, amng no check
	FILE_OPS_RET_WRITE_FAIL,
	FILE_OPS_RET_MALLOC_FAIL // should not happen
};

// common resp err
void resp_parse_err(struct json_object *resp);
void resp_invalid_request(struct json_object *resp);
void resp_action_not_found(struct json_object *resp);
void resp_method_not_found(struct json_object *resp);
void resp_invalid_params(struct json_object *resp);
void resp_file_not_found(struct json_object *resp);
void resp_file_write_fail(struct json_object *resp);
void resp_file_oversize(struct json_object *resp);
void resp_file_invalid(struct json_object *resp);

// common function use by method
void json_rpc_common_config(struct json_object *file, struct json_object *resp, char *method, struct json_object *params);
void json_rpc_common_config_json(struct json_object *file, struct json_object *resp, char *method, struct json_object *params);

// files read write ops
enum FILE_OPS_RET file_read_json(struct json_object *settings, char *file_path);
enum FILE_OPS_RET file_write_json(struct json_object *settings, char *file_path);
enum FILE_OPS_RET file_read_amng_custom(struct json_object *settings, char *file_path);
enum FILE_OPS_RET file_write_amng_custom(struct json_object *settings, char *file_path);

// jsonrpc methods
void jsonrpc_nvram(struct json_object *resp, char *method, struct json_object *params);
void jsonrpc_amngcustom(struct json_object *resp, char *method, struct json_object *params);
void jsonrpc_addon_config(struct json_object *resp, char *method, struct json_object *params);
void jsonrpc_addon_config_json(struct json_object *resp, char *method, struct json_object *params);
