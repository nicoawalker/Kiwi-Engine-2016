#include "IKeyboardEventListener.h"

#include "../RawInputWrapper.h"

namespace Kiwi
{

	void IKeyboardEventListener::_OnKeyboardEvent( const Kiwi::KeyboardEvent& evt )
	{

		switch( evt.GetKeyState() )
		{
			case Kiwi::KEY_STATE::KEY_PRESSED:
				{
					this->_OnKeyPress( evt.GetKey() );
					break;
				}
			case Kiwi::KEY_STATE::KEY_RELEASED:
				{
					this->_OnKeyReleased( evt.GetKey() );
					break;
				}
			case Kiwi::KEY_STATE::KEY_HELD:
				{
					this->_OnKeyHeld( evt.GetKey() );
					break;
				}
			default:return;
		}

	}

};