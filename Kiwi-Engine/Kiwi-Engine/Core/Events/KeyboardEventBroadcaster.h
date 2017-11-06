#ifndef _KIWI_KEYBOARDEVENTBROADCASTER_H_
#define _KIWI_KEYBOARDEVENTBROADCASTER_H_

#include "..\EventBroadcaster.h"

#include "IKeyboardEventListener.h"
#include "KeyboardEvent.h"

namespace Kiwi
{

	class KeyboardEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::IKeyboardEventListener, Kiwi::KeyboardEvent>
	{
	public:

		KeyboardEventBroadcaster() :
			EventBroadcaster<Kiwi::IKeyboardEventListener, Kiwi::KeyboardEvent>( &Kiwi::IKeyboardEventListener::_OnKeyboardEvent ) {}

		virtual ~KeyboardEventBroadcaster() {}

		virtual void AddInputListener( Kiwi::IKeyboardEventListener* listener )
		{
			this->AddListener( listener );
		}

		virtual void RemoveInputListener( Kiwi::IKeyboardEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		virtual void BroadcastKeyboardEvent( const Kiwi::KeyboardEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif