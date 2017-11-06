#ifndef _KIWI_ITRANSFORMEVENTLISTENER_H_
#define _KIWI_ITRANSFORMEVENTLISTENER_H_

#include "TransformEvent.h"

namespace Kiwi
{

	class TransformEventBroadcaster;

	class ITransformEventListener
	{
	friend class TransformEventBroadcaster;
	private:

		virtual void OnTransformEvent( const Kiwi::TransformEvent& evt );

	public:

		ITransformEventListener(){}
		virtual ~ITransformEventListener(){}

		virtual void OnRotate( const Kiwi::TransformEvent& evt ) {}
		virtual void OnTranslate( const Kiwi::TransformEvent& evt ) {}

	};
};

#endif