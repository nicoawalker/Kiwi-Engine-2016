#ifndef _KIWI_MOUSEEVENTBROADCASTER_H_
#define _KIWI_MOUSEEVENTBROADCASTER_H_

#include "..\Core\EventBroadcaster.h"

#include "IMouseEventListener.h"
#include "MouseEvent.h"

namespace Kiwi
{

	class MouseEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::IMouseEventListener, Kiwi::MouseEvent>
	{
	protected:

		/*void _BroadcastMouseEvent( const Kiwi::MouseEvent& evt )
		{
			this->_BroadcastEvent( evt );
		}*/

	public:

		MouseEventBroadcaster() :
			EventBroadcaster<Kiwi::IMouseEventListener, Kiwi::MouseEvent>( &Kiwi::IMouseEventListener::OnMouseEvent ) {}

		virtual ~MouseEventBroadcaster() {}

		virtual void AddMouseListener( Kiwi::IMouseEventListener* listener )
		{
			this->AddListener( listener );
		}

		virtual void RemoveMouseListener( Kiwi::IMouseEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		virtual void BroadcastMouseEvent( const Kiwi::MouseEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif