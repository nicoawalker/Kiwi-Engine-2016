#ifndef _KIWI_WINDOWEVENTBROADCASTER_H_
#define _KIWI_WINDOWEVENTBROADCASTER_H_

#include "..\Core\EventBroadcaster.h"

#include "IWindowEventListener.h"
#include "WindowEvent.h"

namespace Kiwi
{

	class WindowEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>
	{
	public:

		WindowEventBroadcaster() :
			EventBroadcaster<Kiwi::IWindowEventListener, Kiwi::WindowEvent>(&Kiwi::IWindowEventListener::OnWindowEvent) {}

		virtual ~WindowEventBroadcaster() {}

		void AddWindowEventListener( Kiwi::IWindowEventListener* listener )
		{
			this->AddListener( listener );
		}

		void RemoveWindowEventListener( Kiwi::IWindowEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		void BroadcastWindowEvent( const Kiwi::WindowEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif