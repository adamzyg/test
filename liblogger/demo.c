#include <stdio.h>

#include "logger.h"

void log_callback(char *msg, void *arg)
{
	printf("Received in callback: %s\n", msg);
}

int main(int argc, char **argv)
{
	int i;

	/* Initialize the logger module. */
	logger_init(LOG_LEVEL_DEBUG);
    char *filepath = "/tmp/test.log";

	/* Log to stderr. */
	//logger_add_fp_handler(fd);
    logger_add_file_handler(filepath, 0);

	/* Add a callback handler. */
	logger_add_callback_handler(log_callback, NULL);

	/* Add a rotating file handler. */
	//logger_add_rotating_handler("demo_log", 5000, 3);

	/* Log an info message. */
	for (i = 0; i < 5; i++) {
		LOG_INFO_PRINT("Info log %d", i);
		LOG_DEBUG_PRINT("Debug log %d", i);
		LOG_ERROR_PRINT("Error log %d", i);
	}

	/* Reset the logger - frees all allocated resources and keeps
	 * valgrind happy. */
	logger_reset();

	return 0;
}
