#include <stdio.h>
#include <stdlib.h>
#include "../jsonrpc.h"

int main(void) {
	char *resp_buf = malloc(16384 * sizeof(char));
	char *input_buf = malloc(8192 * sizeof(char));

	if (!resp_buf || !input_buf) {
		printf("malloc Fail!!\r\n");
		return -1;
	}

	printf("Enter test json String: \r\n");

	while(1) {
		gets(input_buf);

		jsonrpc(input_buf, resp_buf);

		printf("resp_buf: \r\n");
		printf("%s", resp_buf);
		printf("\r\n");
	}
	return 0;
}


