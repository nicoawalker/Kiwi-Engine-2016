#include "IThreadParam.h"

namespace Kiwi
{

	long IThreadParam::GlobalThreadID = 0;
	std::mutex IThreadParam::GlobalThreadIDMutex;

	IThreadParam::IThreadParam( long threadID, std::wstring threadName )
	{

		GlobalThreadIDMutex.lock();
		m_globalThreadID = GlobalThreadID++;
		GlobalThreadIDMutex.unlock();

		m_threadID = threadID;
		m_threadName = threadName;
		m_shutdownFlag = false;

	}

	IThreadParam::~IThreadParam()
	{
	}

	void IThreadParam::Lock()
	{

		m_threadMutex.lock();

	}

	void IThreadParam::Unlock()
	{

		m_threadMutex.unlock();

	}

	bool IThreadParam::TryLock()
	{

		return m_threadMutex.try_lock();

	}

}