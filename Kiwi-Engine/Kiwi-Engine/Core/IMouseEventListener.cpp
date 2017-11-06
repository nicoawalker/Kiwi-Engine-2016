#include "IMouseEventListener.h"

#include "../Core/Utilities.h"

namespace Kiwi
{

	void IMouseEventListener::OnMouseEvent( const Kiwi::MouseEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case MouseEvent::MOUSE_HELD:
				{
					this->OnMouseHeld( evt );
					break;
				}
			case MouseEvent::MOUSE_MOVE:
				{
					this->OnMouseMove( evt );
					break;
				}
			case MouseEvent::MOUSE_PRESS:
				{
					this->OnMousePress( evt );
					break;
				}
			case MouseEvent::MOUSE_RELEASE:
				{
					this->OnMouseRelease( evt );
					break;
				}
			default: return;
		}

	}

};