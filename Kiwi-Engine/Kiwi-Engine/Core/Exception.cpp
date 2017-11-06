#include "Exception.h"

namespace Kiwi
{

	Exception::Exception(std::wstring source, std::wstring error, int exceptionID )
	{

		m_source = source;
		m_error = error;
		m_exceptionID = exceptionID;

	}

};