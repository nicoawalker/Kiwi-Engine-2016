#ifndef _KIWI_IENGINEAPP_H_
#define _KIWI_IENGINEAPP_H_

#include "EngineRoot.h"

#include "../Graphics/IWindowEventListener.h"

namespace Kiwi
{

	class EngineRoot;

	class IEngineApp:
		public Kiwi::IWindowEventListener,
		public Kiwi::IFrameEventListener
	{
	protected:

		Kiwi::EngineRoot* m_engine;
		Kiwi::GraphicsManager* m_graphicsManager;
		Kiwi::SceneManager* m_sceneManager;

	protected:

		virtual void OnWindowEvent( const Kiwi::WindowEvent& evt ) {}
		virtual void OnUpdate() {}
		virtual void OnFixedUpdate() {}
		virtual void OnShutdown() {}

	public:

		IEngineApp();
		virtual ~IEngineApp();

		virtual void Shutdown() { this->OnShutdown(); }

		virtual void Launch() = 0;

	};
};

#endif