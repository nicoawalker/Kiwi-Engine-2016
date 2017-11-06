#ifndef _KIWI_UIBUTTONEVENTBROADCASTER_H_
#define _KIWI_UIBUTTONEVENTBROADCASTER_H_

#include "..\..\Core\EventBroadcaster.h"

#include "UIButtonEventListener.h"

namespace Kiwi
{

	class UIButtonEvent;

	class UIButtonEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::UIButtonEventListener, Kiwi::UIButtonEvent>
	{
	public:

		UIButtonEventBroadcaster() :
			EventBroadcaster<Kiwi::UIButtonEventListener, Kiwi::UIButtonEvent>(&Kiwi::UIButtonEventListener::OnButtonEvent) {}

		virtual ~UIButtonEventBroadcaster() {}

		void AddButtonEventListener( Kiwi::UIButtonEventListener* listener )
		{
			this->AddListener( listener );
		}

		void RemoveButtonEventListener( Kiwi::UIButtonEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		void BroadcastButtonEvent( const Kiwi::UIButtonEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif