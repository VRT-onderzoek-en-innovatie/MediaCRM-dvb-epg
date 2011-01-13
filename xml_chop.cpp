#include <stdio.h>
#include <unistd.h>
#include <expat.h>
#include <string>

std::string buffer;
unsigned long depth = 0;

void start(void *userData, const XML_Char *name, const XML_Char **atts) {
	if( depth == 1 ) { // new root element (inside our own dummy-root)
		buffer.clear();
	}
	buffer.append("<");
	buffer.append(name);
	for( int i = 0; atts[i]; i+=2 ) {
		buffer.append(" ");
		buffer.append(atts[i]);
		buffer.append("=\"");
		buffer.append(atts[i+1]);
		buffer.append("\"");
	}
	buffer.append(">");
	depth++;
}

void cdatastart(void *userData) {
	buffer.append("<![CDATA[");
}

void chardata(void *userData, const XML_Char *s, int len) {
	buffer.append(s, len);
}

void cdataend(void *userData) {
	buffer.append("]]>");
}

void end(void *userData, const XML_Char *name) {
	depth--;
	buffer.append("</");
	buffer.append(name);
	buffer.append(">");
}


#define BUFF_SIZE 4096
int main(int argc, char *argv[]) {
	XML_Parser xp = XML_ParserCreate(NULL);

	XML_SetElementHandler(xp, start, end);
	XML_SetCdataSectionHandler(xp, cdatastart, cdataend);
	XML_SetCharacterDataHandler(xp, chardata);

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
		
		if (! XML_ParseBuffer(xp, bytes_read, 0)) {
			/* handle parse error */
			fprintf(stderr, "Parse error at byte %ld (line %ld, column %ld): %s\n",
				XML_GetCurrentByteIndex(xp), XML_GetCurrentLineNumber(xp),
				XML_GetCurrentColumnNumber(xp), XML_ErrorString( XML_GetErrorCode(xp) ) );
			return 1;
		}
		if (bytes_read == 0)
			break;
	}

	write(1, buffer.data(), buffer.length());

	XML_Parse(xp, "</dummyRootElement>", 19, 1);

	XML_ParserFree(xp);
	return 0;
}
