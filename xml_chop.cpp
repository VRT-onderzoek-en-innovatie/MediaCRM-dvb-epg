#include <stdio.h>
#include <unistd.h>
#include <expat.h>

int main(int argc, char *argv[]) {
	char buf[4096];

	XML_Parser xp = XML_ParserCreate(NULL);

	ssize_t nbuf;
	while( (nbuf = read(0, buf, sizeof(buf))) > 0 ) {
		if( XML_Parse(xp, buf, nbuf, 0) ) {
			fprintf(stderr, "Parse error\n");
			return 1;
		}
	}
	if( nbuf == -1 ) {
		perror("Could not read input");
	}

	XML_ParserFree(xp);
	return 0;
}
