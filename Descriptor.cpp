#include "Descriptor.hpp"

Descriptor::Descriptor(const unsigned char **descriptor) {
	m_tag = (*descriptor)[0];
	m_length = (*descriptor)[1];
	*descriptor += 2;

	*descriptor += m_length;
}
