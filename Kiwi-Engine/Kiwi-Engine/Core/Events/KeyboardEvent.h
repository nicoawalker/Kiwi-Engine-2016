#ifndef _KIWI_KEYBOARDEVENT_H_
#define _KIWI_KEYBOARDEVENT_H_

#include "../RawInputWrapper.h"
#include "..\Event.h"

#include <Windows.h>

namespace Kiwi
{

	class RenderWindow;

	class KeyboardEvent :
		public Kiwi::Event<Kiwi::RenderWindow>
	{
	protected:

		Kiwi::KEY m_key;
		Kiwi::KEY_STATE m_keyState;

	public:

		KeyboardEvent( Kiwi::RenderWindow* source, Kiwi::KEY key, Kiwi::KEY_STATE newState ) :
			Event( source ),
			m_key(key), m_keyState(newState)
		{
		}

		virtual ~KeyboardEvent() {}

		Kiwi::KEY GetKey()const { return m_key; }
		Kiwi::KEY_STATE GetKeyState()const { return m_keyState; }

	};
};

#endif