#include "ShortEvent.hpp"
#include <string.h>
#include <iconv.h>
#include <assert.h>

namespace Descriptor {

const char* encoding(const char **text) {
	switch(*(*text)++) {
	case 0x00: return "ISO-8859-15"; // approximately; TODO: define the correct table
	case 0x10:
		switch( *(*text)++ ) {
		case 0x00:
			switch( *(*text)++ ) {
			case 0x01: return "ISO-8859-1";
			default:
				fprintf(stderr, "Unknown character encoding: 0x10 0x00 0x%02x\n", (*text)[-1]);
				return NULL;
			}
		default:
			fprintf(stderr, "Unknown character encoding: 0x10 0x%02x\n", (*text)[-1]);
			return NULL;
		}
	default:
		fprintf(stderr, "Unknown character encoding: 0x%02x\n", (*text)[-1]);
		return NULL;
	}
}

ShortEvent::ShortEvent(const unsigned char **descriptor) {
	m_tag = (*descriptor)[0]; assert( m_tag == 0x4d );
	m_length = (*descriptor)[1];

	m_ISO_639_language_code[0] = (*descriptor)[2];
	m_ISO_639_language_code[1] = (*descriptor)[3];
	m_ISO_639_language_code[2] = (*descriptor)[4];
	m_ISO_639_language_code[3] = '\0';
	*descriptor += 5;

	m_nevent_name = (*descriptor)[0];
	m_event_name = new char[m_nevent_name];
	memcpy(m_event_name, (*descriptor)+1, m_nevent_name);
	*descriptor += 1 + m_nevent_name;

	m_ntext = (*descriptor)[0];
	m_text = new char[m_ntext];
	memcpy(m_text, (*descriptor)+1, m_ntext);
	*descriptor += 1 + m_ntext;
}

ShortEvent::~ShortEvent() {
	delete[] m_event_name;
	delete[] m_text;
}

std::string ShortEvent::XMLTV() const {
	std::string ret;

	{ 
		ret += "<title lang=\"";
		ret += m_ISO_639_language_code;
		ret += "\">";

		const char* t = m_event_name;
		const char* t_end = m_event_name + m_nevent_name;
		const char* from_encoding = encoding(&t);
		if( from_encoding ) {
			iconv_t ic = iconv_open("UTF-8", from_encoding);
			assert( ic != (iconv_t)-1 );
	
			size_t nt = t_end - t;
			char text_utf8[6*256]; // worst possible case is 6bytes/char, 256chars max
			size_t nout = sizeof(text_utf8);
			{
				char *out = text_utf8;
				iconv(ic, const_cast<char**>(&t), &nt,  &out, &nout);
			}
			
			iconv_close( ic );
	
			ret.append(text_utf8, sizeof(text_utf8)-nout);
		}
		ret += "</title>";
	}
	if( m_ntext > 0 ) {
		ret += "<desc lang=\"";
		ret += m_ISO_639_language_code;
		ret += "\">";

		const char* t = m_text;
		const char* t_end = m_text + m_ntext;
		const char* from_encoding = encoding(&t);
		if( from_encoding ) {
			iconv_t ic = iconv_open("UTF-8", from_encoding);
			assert( ic != (iconv_t)-1 );
	
			size_t nt = t_end - t;
			char text_utf8[6*256]; // worst possible case is 6bytes/char, 256chars max
			size_t nout = sizeof(text_utf8);
			{
				char *out = text_utf8;
				iconv(ic, const_cast<char**>(&t), &nt,  &out, &nout);
			}
			
			iconv_close( ic );
	
			ret.append(text_utf8, sizeof(text_utf8)-nout);
		}
		ret += "</desc>";
	}

	return ret;
}

} // namespace
