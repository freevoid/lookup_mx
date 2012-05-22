#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "mx.h"
#include "log.h"

int main(int argc, char *argv[])
{
	if (argc != 2) {
		fprintf(stderr, "Usage: lookup_mx <domainname>\n");
		return EXIT_FAILURE;
	}

	log_configure(LOG_INFO, stderr);

	const char *domain_name = argv[1];
	mxrecord_t record;

	if (resolve_mx(domain_name, &record) == 0) {
		printf("%s\n", record.name);
		return EXIT_SUCCESS;
	} else {
		return EXIT_FAILURE;
	}
}
