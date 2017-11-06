#ifndef _KIWI_ENGINEROOT_H_
#define _KIWI_ENGINEROOT_H_

#include "FrameEventBroadcaster.h"
#include "SceneManager.h"
#include "GameTimer.h"
#include "ThreadManager.h"

#include "../Graphics/GraphicsManager.h"

#include "../Physics/PhysicsSystem.h"

#include <unordered_set>

namespace Kiwi
{

	class Console;

	class EngineRoot:
		public Kiwi::FrameEventBroadcaster
	{
	protected:

		//class that contains and controls all created scenes
		Kiwi::SceneManager m_sceneManager;

		//class used to create and manage graphical components (e.g. renderer, renderWindow...)
		Kiwi::GraphicsManager m_graphicsManager;

		Kiwi::PhysicsSystem m_physicsSystem;

		//stores a pointer to the main game window
		Kiwi::RenderWindow* m_gameWindow;

		Kiwi::GameTimer m_gameTimer;

		Kiwi::Console* m_console;

		Kiwi::ThreadManager<Kiwi::IThreadParam*>* m_threadManager;
		std::unordered_set<unsigned int> m_activeThreadIDs;

		bool m_engineRunning;
		bool m_initialized;

	protected:

		virtual void _MainLoop();
		virtual void _PumpMessages();

	public:

		EngineRoot();
		~EngineRoot();

		void Initialize( std::wstring logFile, Kiwi::RenderWindow* gameWindow );

		void Shutdown();

		void Start();
		void Pause() {}
		void Stop();

		void SetMouseSensitivity( const Kiwi::Vector2& sens );

		unsigned int SpawnThread( Kiwi::IThreadParam* ( *threadFunction )(Kiwi::IThreadParam*), Kiwi::IThreadParam* threadParam = 0 );

		/*spawns a class member function as a thread*/
		template<typename FunctionClass>
		unsigned int SpawnThread( FunctionClass* fClass, Kiwi::IThreadParam* (FunctionClass::*threadFunction)(Kiwi::IThreadParam*), Kiwi::IThreadParam* parameter )
		{

			if( m_threadManager != 0 )
			{
				unsigned int threadID = m_threadManager->SpawnThread( fClass, threadFunction, parameter );
				if( threadID != 0 )
				{
					m_activeThreadIDs.insert( threadID );
				}
				return threadID;
			}

			return 0;

		}

		/*joins the specified thread*/
		Kiwi::IThreadParam* JoinThread( unsigned int threadID );

		Kiwi::ThreadStatus GetThreadStatus( unsigned int threadID );

		Kiwi::GameTimer* GetGameTimer() { return &m_gameTimer; }
		Kiwi::Console* GetConsole() { return m_console; }
		Kiwi::RenderWindow* GetGameWindow()const { return m_gameWindow; }
		Kiwi::SceneManager* GetSceneManager() { return &m_sceneManager; }
		Kiwi::GraphicsManager* GetGraphicsManager() { return &m_graphicsManager; }
		Kiwi::PhysicsSystem* GetPhysicsSystem() { return &m_physicsSystem; }

	};

}

#endif