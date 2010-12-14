#include "ShortEvent.hpp"
#include <string.h>
#include <assert.h>

namespace Descriptor {

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

} // namespace
