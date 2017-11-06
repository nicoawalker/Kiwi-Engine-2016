#ifndef _KIWI_SCENEEVENTBROADCASTER_H_
#define _KIWI_SCENEEVENTBROADCASTER_H_

#include "..\EventBroadcaster.h"

#include "ISceneEventListener.h"

namespace Kiwi
{

	class TransformEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::ISceneEventListener, Kiwi::TransformEvent>
	{
	public:

		TransformEventBroadcaster() :
			EventBroadcaster<Kiwi::ISceneEventListener, Kiwi::TransformEvent>( &Kiwi::ISceneEventListener::OnTransformEvent ) {}

		virtual ~TransformEventBroadcaster() {}

		void AddSceneListener( Kiwi::ISceneEventListener* listener )
		{
			this->AddListener( listener );
		}

		void RemoveSceneListener( Kiwi::ISceneEventListener* listener )
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