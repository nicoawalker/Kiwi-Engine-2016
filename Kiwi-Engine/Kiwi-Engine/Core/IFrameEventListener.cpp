#include "IFrameEventListener.h"

namespace Kiwi
{

	void IFrameEventListener::OnFrameEvent( const Kiwi::FrameEvent& evt )
	{

		switch( evt.GetEventType() )
		{
			case Kiwi::FrameEvent::EventType::UNTIMED_EVENT:
				{
					this->OnUpdate();
					break;
				}
			case Kiwi::FrameEvent::EventType::TIMED_EVENT:
				{
					this->OnFixedUpdate();
					break;
				}
			default: break;
		}

	}

}