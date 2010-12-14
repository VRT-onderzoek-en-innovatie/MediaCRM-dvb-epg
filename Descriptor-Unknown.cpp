#include "Descriptor-Unknown.hpp"

Descriptor_Unknown::Descriptor_Unknown(const unsigned char **descriptor) {
	m_tag = (*descriptor)[0];
	m_length = (*descriptor)[1];
	*descriptor += 2;
	// No processing for Unknown
	*descriptor += m_length;
}
