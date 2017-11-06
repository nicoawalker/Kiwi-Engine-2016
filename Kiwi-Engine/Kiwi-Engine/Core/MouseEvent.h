#ifndef _KIWI_MOUSEEVENT_H_
#define _KIWI_MOUSEEVENT_H_

#include "RawInputWrapper.h"

#include "..\Core\Event.h"

#include <Windows.h>

namespace Kiwi
{

	class RenderWindow;

	class MouseEvent: 
		public Kiwi::Event<Kiwi::RenderWindow>
	{
	public:

		enum MOUSEEVENT_TYPE { MOUSE_MOVE = 0, MOUSE_PRESS, MOUSE_RELEASE, MOUSE_HELD };

	protected:

		MOUSEEVENT_TYPE m_mouseEventType;

		MOUSE_BUTTON m_mouseButton;
		short m_wheelDelta; //amount the scroll wheel was moved
		Kiwi::Vector2 m_deltaPos; //change in position since last frame
		Kiwi::Vector2 m_pos; //current position of the mouse

	public:

		MouseEvent( Kiwi::RenderWindow* source, MOUSEEVENT_TYPE eventType, MOUSE_BUTTON button, const Kiwi::Vector2& deltaPos, const Kiwi::Vector2& pos, short wheelDelta ) :
			Event(source),
			m_mouseEventType(eventType), m_mouseButton(button), m_wheelDelta(wheelDelta), m_deltaPos(deltaPos), m_pos(pos) {}

		virtual ~MouseEvent() {}

		MOUSEEVENT_TYPE GetEventType()const { return m_mouseEventType; }
		MOUSE_BUTTON GetButton()const { return m_mouseButton; }
		Kiwi::Vector2 GetDeltaPos()const { return m_deltaPos; }
		Kiwi::Vector2 GetPos()const { return m_pos; }
		short GetWheelDelta()const { return m_wheelDelta; }

	};
};

#endif