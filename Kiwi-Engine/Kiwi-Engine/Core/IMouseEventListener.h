#ifndef _KIWI_IMOUSEEVENTLISTENER_H_
#define _KIWI_IMOUSEEVENTLISTENER_H_

#include "MouseEvent.h"

namespace Kiwi
{

	class MouseEventBroadcaster;

	class IMouseEventListener
	{
	friend class MouseEventBroadcaster;
	private:

		void OnMouseEvent( const Kiwi::MouseEvent& evt );

	public:

		IMouseEventListener(){}
		virtual ~IMouseEventListener(){}

		virtual void OnMousePress( const Kiwi::MouseEvent& evt ) {}
		virtual void OnMouseRelease( const Kiwi::MouseEvent& evt ) {}
		virtual void OnMouseHeld( const Kiwi::MouseEvent& evt ) {}
		virtual void OnMouseMove( const Kiwi::MouseEvent& evt ) {}

	};
};

#endif