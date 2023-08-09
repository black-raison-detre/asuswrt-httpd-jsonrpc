#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <json.h>
#include "jsonrpc.h"

// Read json type config file
enum FILE_OPS_RET file_read_json(struct json_object *settings, char *file_path) {
	FILE *fp = NULL;
	char *file_buf = malloc(MAX_FILE_SIZE * sizeof(char));
	int file_size = 0;
	enum FILE_OPS_RET ret;

	if (!file_buf) {
		ret = FILE_OPS_RET_MALLOC_FAIL;
		goto CLOSE;
	}

	fp = fopen(file_path, "r");

	if (!fp) {
		ret = FILE_OPS_RET_NOFILE;
		goto CLOSE;
	}

	// check file size
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (file_size > MAX_FILE_SIZE) {
		ret = FILE_OPS_RET_OVERSIZE;
		goto CLOSE;
	}

	fread(file_buf, sizeof(char), MAX_FILE_SIZE, fp);

	settings = json_tokener_parse(file_buf);

	if (!settings) {
		ret = FILE_OPS_RET_READ_FAIL;
		goto CLOSE;
	}

	ret = FILE_OPS_RET_OK;
	
CLOSE:
	if (file_buf)
		free(file_buf);
	if (fp)
		fclose(fp);
	
	return ret;
}

// write json type config file
enum FILE_OPS_RET file_write_json(struct json_object *settings, char *file_path) {
	FILE *fp = NULL;
	char *json_string = NULL;
	int json_string_len = 0;
	int string_written = 0;
	enum FILE_OPS_RET ret;

	fp = fopen(file_path, "w");

	if (!fp) {
		ret = FILE_OPS_RET_WRITE_FAIL;
		goto CLOSE;
	}

	json_string = json_object_to_json_string_ext(settings, JSON_C_TO_STRING_PRETTY);
	json_string_len = strlen(json_string);

	if (!json_string) {
		ret = FILE_OPS_RET_WRITE_FAIL;
		goto CLOSE;
	}

	if (json_string_len > MAX_FILE_SIZE) {
		ret = FILE_OPS_RET_OVERSIZE;
		goto CLOSE;
	}

	string_written = fwrite(json_string, sizeof(char), json_string_len, fp);

	if (string_written != json_string_len) {
		ret = FILE_OPS_RET_WRITE_FAIL;
		goto CLOSE;
	}

	ret = FILE_OPS_RET_OK;

CLOSE:
	if (fp)
		fclose(fp);
	
	return ret;
}

// Read amng style txt config file
enum FILE_OPS_RET file_read_amng_custom(struct json_object *settings, char *file_path) {
	FILE *fp = NULL;
	char line[3040];
	char name[30];
	char value[3000];
	int file_size = 0;
	enum FILE_OPS_RET ret;

	fp = fopen(file_path, "r");

	if (!fp) {
		ret = FILE_OPS_RET_NOFILE;
		goto CLOSE;
	}

	// check file size
	fseek(fp, 0, SEEK_END);
	file_size = ftell(fp);
	fseek(fp, 0, SEEK_SET);

	if (file_size > MAX_FILE_SIZE) {
		ret = FILE_OPS_RET_OVERSIZE;
		goto CLOSE;		
	}

	// Mostly copied from RMerlin's Code
	while (fgets(line, sizeof(line), fp)) {
		if (sscanf(line, "%29s%*[ ]%2999s%*[ \n]", name, value) == 2) {
			json_object_object_add(settings, name, json_object_new_string(value));
		}
	}

	ret = FILE_OPS_RET_OK;

CLOSE:
	if (fp)
		fclose(fp);

	return ret;
}

// Read amng style txt config file
enum FILE_OPS_RET file_write_amng_custom(struct json_object *settings, char *file_path) {
	FILE *fp = NULL;
	char line[3040];
	int file_size = 0;
	enum FILE_OPS_RET ret;

	fp = fopen(file_path, "w");

	if (!fp) {
		ret = FILE_OPS_RET_WRITE_FAIL;
		goto CLOSE;
	}
	
	// Mostly copied from RMerlin's Code
	json_object_object_foreach(settings, key, val) {
		snprintf(line, sizeof(line), "%s %s\n", key, json_object_get_string(val));
		fwrite(line, sizeof(char), strlen(line), fp);
	}

	ret = FILE_OPS_RET_OK;
	
CLOSE:
	if (fp)
		fclose(fp);
	
	return ret;
}


