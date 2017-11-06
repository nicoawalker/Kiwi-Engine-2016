#ifndef _KIWI_SCENEEVENT_H_
#define _KIWI_SCENEEVENT_H_

#include "..\Event.h"
#include "../Quaternion.h"
#include "../Vector3d.h"

#include <Windows.h>

namespace Kiwi
{

	class Scene;

	class SceneEvent :
		public Kiwi::Event<Kiwi::Scene>
	{
	public:

		enum SCENEEVENT_TYPE { ENTITY_CREATED, ENTITY_DESTROYED, PRERENDER, POSTRENDER };

	protected:

		SCENEEVENT_TYPE m_sceneEventType;

		Kiwi::Scene* m_sourceScene;

	public:

		SceneEvent( Kiwi::Scene* sourceScene ) :
			Event( sourceScene )
		{
		}

		SceneEvent( Kiwi::Scene* sourceScene ) :
			Event( sourceScene )
		{
		}

		virtual ~SceneEvent() {}

		SCENEEVENT_TYPE GetEventType()const { return m_sceneEventType; }

	};
};

#endif