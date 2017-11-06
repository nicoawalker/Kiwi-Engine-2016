#ifndef _KIWI_IKEYBOARDEVENTLISTENER_H_
#define _KIWI_IKEYBOARDEVENTLISTENER_H_

#include "KeyboardEvent.h"

namespace Kiwi
{

	class KeyboardEventBroadcaster;

	class IKeyboardEventListener
	{
	friend class KeyboardEventBroadcaster;
	private:

		void _OnKeyboardEvent( const Kiwi::KeyboardEvent& evt );

	protected:

		virtual void _OnKeyPress( Kiwi::KEY key ) {}
		virtual void _OnKeyHeld( Kiwi::KEY key ) {}
		virtual void _OnKeyReleased( Kiwi::KEY key ) {}

	public:

		IKeyboardEventListener() {}
		virtual ~IKeyboardEventListener() {}

	};
};

#endif