#ifndef __DESCRIPTOR_SHORTEVENT_HPP__
#define __DESCRIPTOR_SHORTEVENT_HPP__

#include "Descriptor.hpp"
#include <string.h>

namespace Descriptor {

class ShortEvent : public Descriptor {
public:
	char m_ISO_639_language_code[4];
	char *m_event_name;
	size_t m_nevent_name;
	char *m_text;
	size_t m_ntext;

	ShortEvent(const unsigned char **descriptor);
	~ShortEvent();

	virtual std::string XMLTV() const;
};

} // namespace

#endif // __DESCRIPTOR_SHORTEVENT_HPP__
