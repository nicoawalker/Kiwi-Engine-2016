#include "EngineRoot.h"
#include "Logger.h"
#include "Console.h"
#include "RawInputWrapper.h"
#include "Exception.h"

#include <Windows.h>

namespace Kiwi
{

	Kiwi::Logger _Logger;

	EngineRoot::EngineRoot():
		m_sceneManager( this ), m_physicsSystem( *this )
	{

		m_gameTimer.SetTargetUpdatesPerSecond( 60 );
		m_initialized = false;
		m_threadManager = new Kiwi::ThreadManager<Kiwi::IThreadParam*>();
		assert( m_threadManager != 0 );

	}

	EngineRoot::~EngineRoot()
	{

		SAFE_DELETE( m_console );
		SAFE_DELETE( m_threadManager );

		//SAFE_DELETE( m_gameWindow );

		_Logger.Shutdown();

	}

	void EngineRoot::_MainLoop()
	{

		m_gameTimer.StartTimer();
		m_gameTimer.Update();

		m_engineRunning = true;
		while( m_engineRunning )
		{
			m_gameTimer.Update();

			this->_PumpMessages();

			if( m_gameWindow ) m_gameWindow->Update( m_gameTimer.GetDeltaTime() );

			//if enough time has passed, send a fixed update
			if( m_gameTimer.QueryFixedUpdate() )
			{
				m_physicsSystem.FixedUpdate();

				this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::TIMED_EVENT ) );

				if( m_console ) m_console->FixedUpdate();
			}

			m_physicsSystem.Update();

			//broadcast a new untimed frame event
			this->BroadcastEvent( Kiwi::FrameEvent( this, Kiwi::FrameEvent::EventType::UNTIMED_EVENT ) );

			if( m_console ) m_console->Update();

			m_sceneManager.Render();

		}

		this->Shutdown();

	}

	void EngineRoot::_PumpMessages()
	{

		MSG msg;
		while( PeekMessage( &msg, NULL, 0, 0, PM_REMOVE ) )
		{
			TranslateMessage( &msg );
			DispatchMessage( &msg );
		}

	}

	void EngineRoot::Initialize( std::wstring logFile, Kiwi::RenderWindow* gameWindow )
	{

		assert( gameWindow != 0 );

		m_gameWindow = gameWindow;

		m_console = new Kiwi::Console( *this, logFile );
		_Logger.Initialize( logFile );

		m_initialized = true;

	}

	void EngineRoot::Shutdown()
	{

		m_threadManager->JoinAll();

		m_sceneManager.Shutdown();

		m_graphicsManager.Shutdown();

		m_physicsSystem.Shutdown();

		if( m_gameWindow ) m_gameWindow->Shutdown();

	}

	void EngineRoot::Start()
	{

		assert( m_initialized );

		this->_MainLoop();

	}

	void EngineRoot::Stop()
	{

		m_engineRunning = false;

	}

	void EngineRoot::SetMouseSensitivity( const Kiwi::Vector2& sens )
	{

		auto rwItr = m_graphicsManager.m_renderWindows.Front();
		for( ; rwItr != m_graphicsManager.m_renderWindows.Back(); rwItr++ )
		{
			rwItr->second->GetInput().SetMouseSpeed( sens );
		}

	}

	unsigned int EngineRoot::SpawnThread( Kiwi::IThreadParam* (*threadFunction)(Kiwi::IThreadParam*), Kiwi::IThreadParam* threadParam )
	{

		if( m_threadManager != 0 )
		{
			unsigned int threadID = m_threadManager->SpawnThread( threadFunction, threadParam );
			if( threadID != 0 )
			{
				m_activeThreadIDs.insert( threadID );
			}
			return threadID;
		}

		return 0;

	}

	Kiwi::IThreadParam* EngineRoot::JoinThread( unsigned int threadID )
	{

		if( m_threadManager != 0 )
		{
			return m_threadManager->GetThread( threadID );
		}

		return 0;

	}

	Kiwi::ThreadStatus EngineRoot::GetThreadStatus( unsigned int threadID )
	{

		if( m_threadManager != 0 )
		{
			return m_threadManager->GetThreadStatus( threadID );

		} else
		{
			throw Kiwi::Exception( L"EngineRoot::GetThreadStatus", L"Failed to get thread status: thread with ID " + Kiwi::ToWString( threadID ) + L" does not exist" );
		}

	}

}