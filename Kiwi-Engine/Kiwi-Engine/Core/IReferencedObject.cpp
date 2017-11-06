#include "IReferencedObject.h"

#include <Windows.h>

namespace Kiwi
{

	IReferencedObject::~IReferencedObject()
	{
		if( m_refenceCount != 0 )
		{
			MessageBox( NULL, L"Referenced object was deleted with a reference count not 0", L"Error", MB_OK );
		}
	}

	void IReferencedObject::Reserve()
	{

		std::lock_guard<std::mutex> lock( m_referenceMutex );

		m_refenceCount++;

	}

	void IReferencedObject::Free()
	{

		std::lock_guard<std::mutex> lock( m_referenceMutex );
		if( m_refenceCount > 0 )
		{
			m_refenceCount--;
		}

	}

}