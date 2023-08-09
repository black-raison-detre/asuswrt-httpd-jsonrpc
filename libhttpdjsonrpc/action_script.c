#include <stdio.h>
#include <string.h>
#include <json.h>
#ifndef RPC_TEST
#include <shared.h>
#endif
#include "jsonrpc.h"

void jsonrpc_action_script(struct json_object *resp, char *method, struct json_object *params) {
	enum json_type params_type = json_object_get_type(params);
	char *script_name = NULL;
	char rc_buf[64];

	if (params_type != json_type_string) {
		resp_invalid_params(resp);
		goto DONE;
	}

	script_name = json_object_get_string(params);

	if (strlen(script_name) == 0) {
		resp_invalid_params(resp);
		goto DONE;
	}

	if (strlen(method) == 0) {
		resp_action_not_found(resp);
		goto DONE;
	}

	sprintf(rc_buf, "%s_%s", method, script_name);

#ifndef RPC_TEST
	notify_rc(rc_buf);
#else
	printf("notify_rc: %s\r\n", rc_buf);
#endif
	json_object_object_add(resp, "state", json_object_new_int(JSON_RPC_RET_OK));
	goto DONE;

DONE:
	// empty resp for action script, just state
	json_object_object_add(resp, "result", NULL);
	return;
}

