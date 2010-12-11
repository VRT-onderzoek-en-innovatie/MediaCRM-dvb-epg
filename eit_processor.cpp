#include "eit_processor.hpp"
#include "debug.h"
#include <stdio.h>

void EIT_processor::process_section(char *section, size_t nsection) {
	if( nsection == 0 ) {
		warn_assert(nsection != 0);
		return;
	}
	assert(section != NULL);

	printf("Processing section of %lu bytes\n", nsection);
}
