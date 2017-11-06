#ifndef _KIWI_THREADMANAGER_H_
#define _KIWI_THREADMANAGER_H_

#include "Exception.h"
#include "IThreadParam.h"

#include <thread>
#include <future>
#include <unordered_map>

namespace Kiwi
{

	enum ThreadStatus { THREAD_INVALID, THREAD_READY, THREAD_DEFERRED, THREAD_TIMEOUT, THREAD_UNKNOWN };

	template<typename ThreadReturnType = void>
	class ThreadManager
	{
	protected:

		//map of futures, key value = threadID, value = thread future
		std::unordered_map<unsigned int, std::shared_future<ThreadReturnType>> m_futures;

		unsigned int m_threadID;

	public:

		ThreadManager()
		{
			m_threadID = 1;
		}

		~ThreadManager() 
		{

			this->JoinAll();

		}

		/*spawns a new thread that takes no parameters and returns a value of type ThreadReturnType
		returns the threadID of the new thread*/
		unsigned int SpawnThread( ThreadReturnType (*threadFunction)(void) )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction );

			return m_threadID;

		}

		/*spawns a new thread that takes a parameter of type ThreadParameterType and returns a value of type ThreadReturnType
		returns the threadID of the new thread*/
		template<typename ThreadParameterType>
		unsigned int SpawnThread( ThreadReturnType ( *threadFunction )(ThreadParameterType), ThreadParameterType parameter )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, parameter );

			return m_threadID;

		}

		/*spawns a new thread using a member function of class FunctionClass, that takes no parameters
		returns the threadID of the new thread*/
		template<typename FunctionClass>
		unsigned int SpawnThread( FunctionClass* fClass, ThreadReturnType ( FunctionClass::*threadFunction )(void) )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, fClass );

			return m_threadID;

		}

		/*spawns a new thread using a member function of class FunctionClass, that takes no parameters
		returns the threadID of the new thread*/
		template<typename FunctionClass, typename ThreadParameterType>
		unsigned int SpawnThread( FunctionClass* fClass, ThreadReturnType ( FunctionClass::*threadFunction )(ThreadParameterType), ThreadParameterType parameter )
		{

			m_threadID += 1;
			m_futures[m_threadID] = std::async( std::launch::async, threadFunction, fClass, parameter );
			//MessageBox( NULL, Kiwi::ToWString( m_threadID ).c_str(), L"A", MB_OK );
			return m_threadID;

		}

		/*returns the status of the requested thread
		if threadID is invalid it will throw*/
		ThreadStatus GetThreadStatus( unsigned int threadID )
		{

			auto futureItr = m_futures.find( threadID );
			if( futureItr != m_futures.end() )
			{
				auto fStatus = futureItr->second.wait_for( std::chrono::milliseconds( 0 ) );
				switch( fStatus )
				{
					case std::future_status::ready:
						{
							return ThreadStatus::THREAD_READY;
						}
					case std::future_status::deferred:
						{
							return ThreadStatus::THREAD_DEFERRED;
						}
					case std::future_status::timeout:
						{
							return ThreadStatus::THREAD_TIMEOUT;
						}
					default: return ThreadStatus::THREAD_UNKNOWN;
				}

			} else
			{
				throw Kiwi::Exception( L"ThreadManager::GetThreadStatus", L"Thread ID is invalid: " + Kiwi::ToWString( threadID ) );
			}

		}

		/*calls get() on the thread's future
		if the thread has not finished this function will block until the thread finishes
		if the thread threw an exception this function will re-throw it
		returns the value returned by the function
		if threadID is invalid it will throw*/
		ThreadReturnType GetThread( unsigned int threadID )
		{

			auto futureItr = m_futures.find( threadID );
			if( futureItr != m_futures.end() )
			{
				//copy the future, and remove it from the future map
				std::shared_future<ThreadReturnType> future = futureItr->second;
				
				futureItr = m_futures.erase( futureItr );

				//if all threads are complete, reset the thread ID
				if( m_futures.size() == 0 )
				{
					m_threadID = 0;
				}

				/*gets the value returned by the thread. if the thread is not finished, blocks until it is done
				if the thread threw an exception, this will re-throw it*/
				return future.get();
			} else
			{
				throw Kiwi::Exception( L"ThreadManager::GetThreadStatus", L"Thread ID is invalid: " + Kiwi::ToWString( threadID ), 1 );
			}

		}

		/*joins all currently running threads, blocking until this is done
		if maxWaitTime is > 0, this function will wait for each thread only for the specified time, in milliseconds
		any exceptions thrown by the threads are ignored*/
		void JoinAll( int maxWaitTime = -1 )
		{

			for( auto futureItr = m_futures.begin(); futureItr != m_futures.end(); )
			{
				if( futureItr->second.valid() )
				{
					if( maxWaitTime > 0 )
					{
						futureItr->second.wait_for( std::chrono::milliseconds( maxWaitTime ) );

					} else
					{
						futureItr->second.wait();
					}
				}

				futureItr = m_futures.erase( futureItr );
			}

		}

	};
}

#endif