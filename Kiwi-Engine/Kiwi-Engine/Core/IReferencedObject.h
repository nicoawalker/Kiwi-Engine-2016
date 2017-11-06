#ifndef _KIWI_IREFERENCEDOBJECT_H_
#define _KIWI_IREFERENCEDOBJECT_H_

#include <mutex>

namespace Kiwi
{

	class IReferencedObject
	{
	protected:

		std::mutex m_referenceMutex;

		int m_refenceCount;

	public:

		IReferencedObject() :
			m_refenceCount( 0 )
		{
		}

		virtual ~IReferencedObject();

		/*increases the reference count of the object
		the reference count must then be decremented at some point by calling Free()*/
		void Reserve();

		/*decreases the reference count*/
		void Free();

		/*returns the current reference count*/
		int GetReferenceCount()const { return m_refenceCount; }

	};
}

#endif