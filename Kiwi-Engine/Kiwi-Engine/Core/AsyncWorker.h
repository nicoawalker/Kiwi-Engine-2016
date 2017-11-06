#ifndef _KIWI_ASYNCWORKER_H_
#define _KIWI_ASYNCWORKER_H_

namespace Kiwi
{

	template<typename ReturnType>
	class AsyncWorker
	{
	public:

		enum WORKER_STATUS { FAILED = 0, SUCCEEDED, IN_PROGRESS, WAITING };

	protected:

		WORKER_STATUS m_workerStatus;

		unsigned int m_progressPercent;

	public:

		AsyncWorker()
		{

		}

		virtual ~AsyncWorker() {}

		virtual void Start() = 0;

		//returns the current task status
		AsyncWorker::WORKER_STATUS GetStatus() { return m_workerStatus; }

		//returns the current progress, in percent
		unsigned int GetProgress() { return m_progressPercent; }

	};
}

#endif