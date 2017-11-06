#ifndef _KIWI_FRAMEEVENTLISTENER_H_
#define _KIWI_FRAMEEVENTLISTENER_H_

#include "FrameEvent.h"

namespace Kiwi
{

	class IFrameEventListener
	{
	protected:

		virtual void OnUpdate() {}
		virtual void OnFixedUpdate() {}

	public:

		IFrameEventListener() {}
		virtual ~IFrameEventListener() {}

		virtual void OnFrameEvent( const Kiwi::FrameEvent& evt );

	};

}

#endif