#include <stdio.h>
#include <unistd.h>
#include <expat.h>

void start(void *userData, const XML_Char *name, const XML_Char **atts) {
	printf("Open %s\n", name);
}

void end(void *userData, const XML_Char *name) {
	printf("Close %s\n", name);
}


#define BUFF_SIZE 4096
int main(int argc, char *argv[]) {
	XML_Parser xp = XML_ParserCreate(NULL);

	XML_SetElementHandler(xp, start, end);

	XML_Parse(xp, "<dummyRootElement>", 18, 0);
	while(1) {
		int bytes_read;
		void *buff = XML_GetBuffer(xp, BUFF_SIZE);
		if (buff == NULL) {
			/* handle error */
			fprintf(stderr, "Buffer error\n");
			return 1;
		}
		bytes_read = read(0, buff, BUFF_SIZE);
		if (bytes_read < 0) {
			/* handle error */
			fprintf(stderr, "Read error\n");
			return 1;
		}
		
		if (! XML_ParseBuffer(xp, bytes_read, bytes_read == 0)) {
			/* handle parse error */
			fprintf(stderr, "Parse error at byte %d (line %d, column %d): %s\n",
				XML_GetCurrentByteIndex(xp), XML_GetCurrentLineNumber(xp),
				XML_GetCurrentColumnNumber(xp), XML_ErrorString( XML_GetErrorCode(xp) ) );
			return 1;
		}
		if (bytes_read == 0)
			break;
	}

	XML_Parse(xp, "</dummyRootElement>", 19, 1);

	XML_ParserFree(xp);
	return 0;
}
