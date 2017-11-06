#include "ITransformEventListener.h"

#include "../Utilities.h"

namespace Kiwi
{

	void ITransformEventListener::OnTransformEvent( const Kiwi::TransformEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case TransformEvent::TRANSFORM_TRANSLATION:
				{
					this->OnTranslate( evt );
					break;
				}
			case TransformEvent::TRANSFORM_ROTATION:
				{
					this->OnRotate( evt );
					break;
				}
			default: return;
		}

	}

}