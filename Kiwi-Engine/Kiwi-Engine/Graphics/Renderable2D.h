#ifndef _KIWI_RENDERABLE2D_H_
#define _KIWI_RENDERABLE2D_H_

#include "Renderable.h"

namespace Kiwi
{

	class Entity;

	class Renderable2D :
		public Kiwi::Renderable
	{
	protected:



	public:

		Renderable2D( Kiwi::Entity* parentEntity );
		~Renderable2D() {}

	};
}

#endif