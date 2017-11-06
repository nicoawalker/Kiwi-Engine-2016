#ifndef _KIWI_ICOLLISIONEVENTBROADCASTER_H_
#define _KIWI_ICOLLISIONEVENTBROADCASTER_H_

#include "..\Core\EventBroadcaster.h"

#include "ICollisionEventListener.h"

namespace Kiwi
{

	class CollisionEvent;

	class ICollisionEventBroadcaster :
		public Kiwi::EventBroadcaster<Kiwi::ICollisionEventListener, Kiwi::CollisionEvent>
	{
	public:

		ICollisionEventBroadcaster() :
			EventBroadcaster<Kiwi::ICollisionEventListener, Kiwi::CollisionEvent>(&Kiwi::ICollisionEventListener::OnCollisionEvent) {}

		virtual ~ICollisionEventBroadcaster() {}

	};
};

#endif