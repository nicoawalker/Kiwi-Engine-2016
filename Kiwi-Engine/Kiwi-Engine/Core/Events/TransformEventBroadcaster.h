#ifndef _KIWI_TRANSFORMEVENTBROADCASTER_H_
#define _KIWI_TRANSFORMEVENTBROADCASTER_H_

#include "..\EventBroadcaster.h"

#include "ITransformEventListener.h"
#include "TransformEvent.h"

namespace Kiwi
{

	class TransformEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::ITransformEventListener, Kiwi::TransformEvent>
	{
	public:

		TransformEventBroadcaster() :
			EventBroadcaster<Kiwi::ITransformEventListener, Kiwi::TransformEvent>(&Kiwi::ITransformEventListener::OnTransformEvent) {}

		virtual ~TransformEventBroadcaster() {}

		void AddTransformListener( Kiwi::ITransformEventListener* listener )
		{
			this->AddListener( listener );
		}

		void RemoveTransformListener( Kiwi::ITransformEventListener* listener )
		{
			this->RemoveListener( listener );
		}

		void BroadcastTransformEvent( const Kiwi::TransformEvent& evt )
		{
			this->BroadcastEvent( evt );
		}

	};
};

#endif