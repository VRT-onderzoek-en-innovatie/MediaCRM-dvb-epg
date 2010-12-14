#include "Unknown.hpp"

namespace Descriptor {

Unknown::Unknown(const unsigned char **descriptor) {
	m_tag = (*descriptor)[0];
	m_length = (*descriptor)[1];
	*descriptor += 2;
	// No processing for Unknown
	*descriptor += m_length;
}

} //namespace
