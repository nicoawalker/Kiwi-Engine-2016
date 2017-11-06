#ifndef _KIWI_UISCROLLBAREVENTBROADCASTER_H_
#define _KIWI_UISCROLLBAREVENTBROADCASTER_H_

#include "..\..\..\Core\EventBroadcaster.h"

#include "UIScrollBarEventListener.h"

namespace Kiwi
{

	class UIScrollBarEvent;

	class UIScrollBarEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::UIScrollBarEventListener, Kiwi::UIScrollBarEvent>
	{
	public:

		UIScrollBarEventBroadcaster() :
			EventBroadcaster<Kiwi::UIScrollBarEventListener, Kiwi::UIScrollBarEvent>( &Kiwi::UIScrollBarEventListener::OnScrollEvent ) {}

		virtual ~UIScrollBarEventBroadcaster() {}

		void AddScrollBarEventListener( Kiwi::UIScrollBarEventListener* listener )
		{
			this->AddListener( listener );
		}

		void RemoveScrollBarEventListener( Kiwi::UIScrollBarEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		void BroadcastScrollBarEvent( const Kiwi::UIScrollBarEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif