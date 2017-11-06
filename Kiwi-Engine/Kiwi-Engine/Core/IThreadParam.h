#ifndef _KIWI_ITHREADPARAM_H_
#define _KIWI_ITHREADPARAM_H_

#include <string>
#include <mutex>

namespace Kiwi
{

	class IThreadParam
	{
	protected:

		static long GlobalThreadID;
		static std::mutex GlobalThreadIDMutex;

		std::mutex m_threadMutex;

		long m_globalThreadID; //ID unique to each thread

		long m_threadID; //custom ID

		std::wstring m_threadName; //custom Name

		//if set to true, indicates to a thread that it should shut down immediately
		//this should never be modified by a thread
		bool m_shutdownFlag;

	public:

		IThreadParam( long threadID = 0, std::wstring threadName = L"" );
		virtual ~IThreadParam() = 0;

		virtual void Lock();
		virtual void Unlock();

		virtual bool TryLock();

		/*if set to true, indicates to a thread that it should shut down immediately
		this should never be modified by a thread and is not thread safe*/
		bool SetShutdownFlag( bool shutdown ) { m_shutdownFlag; }

		/*returns the unique global id of the thread*/
		long GetGlobalID()const { return m_globalThreadID; }

		/*returns the custom, user-defined id of the thread*/
		long GetID()const { return m_threadID; }

		std::wstring GetName()const { return m_threadName; }

		bool GetShutdownFlag()const { return m_shutdownFlag; }

	};
}


#endif