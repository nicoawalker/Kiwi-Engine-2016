#include "RawInputWrapper.h"
#include "Exception.h"
#include "Assert.h"
#include "MouseEvent.h"

#include "../Graphics/RenderWindow.h"

#include <Windows.h>

namespace Kiwi
{

	RawInputWrapper::RawInputWrapper( Kiwi::RenderWindow* targetWindow )
	{

		assert( targetWindow != 0 );

		m_renderWindow = targetWindow;

		m_wheelDelta = 0;
		m_mouseSpeed = Kiwi::Vector2( 1.0f, 1.0f );

	}

	RawInputWrapper::~RawInputWrapper()
	{
	}

	void RawInputWrapper::Initialize()
	{

		assert( m_renderWindow != 0 );

		RAWINPUTDEVICE Mouse;
		Mouse.usUsage = 0x02;    // register mouse
		Mouse.usUsagePage = 0x01;    // top-level mouse
		Mouse.dwFlags = NULL;    // flags
		Mouse.hwndTarget = m_renderWindow->GetHandle();    // handle to a window

		if( !RegisterRawInputDevices( &Mouse, 1, sizeof( RAWINPUTDEVICE ) ) )   // register the device
		{
			throw Kiwi::Exception( L"RawInputWrapper", L"[Mouse] Call to RegisterRawInputDevices failed" );
		}

		/*intialize all key values to the default key_up state*/
		m_keyboardState.resize( 256 );
		std::fill( m_keyboardState.begin(), m_keyboardState.end(), KEY_UP );
		m_lastKeyboardState.resize( 256 );
		std::fill( m_lastKeyboardState.begin(), m_lastKeyboardState.end(), KEY_UP );

	}

	void RawInputWrapper::OnUpdate( float deltaTime )
	{

		assert( m_renderWindow != 0 );

		//first get and set the current position of the mouse
		POINT mousePos;
		GetCursorPos( &mousePos ); //get cursor position in screen coords
		ScreenToClient( m_renderWindow->GetHandle(), &mousePos ); //transform screen coords into local window coords
		
		//convert so that 0,0 is at the center of the window
		mousePos.y -= m_renderWindow->GetClientSize().y / 2.0;
		mousePos.y *= -1.0;
		mousePos.x -= m_renderWindow->GetClientSize().x / 2.0;

		m_currentMouseState.pos.Set( (float)mousePos.x, (float)mousePos.y );

		//store the delta mouse from last frame, and reset the deltaMouse counter
		m_currentMouseState.deltaPos.x = m_deltaMouse.x * m_mouseSpeed.x;
		m_currentMouseState.deltaPos.y = m_deltaMouse.y * m_mouseSpeed.y;
		m_deltaMouse = Kiwi::Vector2( 0.0f, 0.0f );

		//same for the wheel delta
		m_currentMouseState.wheelDelta = m_wheelDelta;
		m_wheelDelta = 0;

		//update all of the mouse button states
		for( unsigned int i = 0; i < NUM_MOUSE_BUTTONS; i++ )
		{
			if( ( m_currentMouseState.mouseStates[i] != BUTTON_RELEASED && m_currentMouseState.mouseStates[i] != BUTTON_UP ) && 
				(m_lastMouseState.mouseStates[i] == BUTTON_PRESSED || m_lastMouseState.mouseStates[i] == BUTTON_HELD) )
			{
				//if the button was pressed or held last frame and has not been released, set the status to held
				m_currentMouseState.mouseStates[i] = BUTTON_HELD;
				m_renderWindow->BroadcastMouseEvent( Kiwi::MouseEvent( m_renderWindow, MouseEvent::MOUSE_HELD, (MOUSE_BUTTON)i, m_currentMouseState.deltaPos, m_currentMouseState.pos, m_currentMouseState.wheelDelta ) );

			} else if( m_currentMouseState.mouseStates[i] == BUTTON_RELEASED && m_lastMouseState.mouseStates[i] == BUTTON_RELEASED )
			{
				//if the button was released last frame, and hasn't been pressed, set the status to up
				m_currentMouseState.mouseStates[i] = BUTTON_UP;

			} else if( m_currentMouseState.mouseStates[i] == BUTTON_PRESSED )
			{//mouse was just pressed this frame
				Kiwi::MouseEvent evt( m_renderWindow, MouseEvent::MOUSE_PRESS, (MOUSE_BUTTON)i, m_currentMouseState.deltaPos, m_currentMouseState.pos, m_currentMouseState.wheelDelta );
				m_renderWindow->BroadcastMouseEvent( evt );

			} else if( m_currentMouseState.mouseStates[i] == BUTTON_RELEASED )
			{//mouse was just released this frame
				Kiwi::MouseEvent evt( m_renderWindow, MouseEvent::MOUSE_RELEASE, (MOUSE_BUTTON)i, m_currentMouseState.deltaPos, m_currentMouseState.pos, m_currentMouseState.wheelDelta );
				m_renderWindow->BroadcastMouseEvent( evt );
			}
		}

		if( m_currentMouseState.deltaPos.x != 0.0f || m_currentMouseState.deltaPos.y != 0.0f )
		{
			Kiwi::MouseEvent evt( m_renderWindow, MouseEvent::MOUSE_MOVE, NO_BUTTON, m_currentMouseState.deltaPos, m_currentMouseState.pos, m_currentMouseState.wheelDelta );
			m_renderWindow->BroadcastMouseEvent( evt );
		}

		m_lastMouseState = m_currentMouseState;

		//get the current keyboard key states as false or true values
		std::vector<unsigned char> newKeyboardState(256);
		GetKeyboardState( &newKeyboardState[0] );

		//copy the current key states into the last buffer
		m_lastKeyboardState.swap( m_keyboardState );

		for( unsigned int i = 0; i < m_lastKeyboardState.size(); i++ )
		{
			//will set m_keyboardState[i] to either KEY_UP or KEY_PRESSED (false or true)
			//if the high order bit is 1, the key is down, otherwise it is up
			if( (newKeyboardState[i] >> 7) == 1 )
			{
				m_keyboardState[i] = KEY_PRESSED;

			} else
			{
				m_keyboardState[i] = KEY_UP;
			}

			if( m_keyboardState[i] == KEY_UP )
			{
				if( m_lastKeyboardState[i] == KEY_HELD || m_lastKeyboardState[i] == KEY_PRESSED )
				{
					//if the key was held last frame and has now been released, change the state to KEY_RELEASED
					m_keyboardState[i] = KEY_RELEASED;
					m_renderWindow->BroadcastKeyboardEvent( Kiwi::KeyboardEvent( m_renderWindow, (Kiwi::KEY)i, KEY_RELEASED ) );
				}

			} else
			{
				if( m_lastKeyboardState[i] == KEY_PRESSED || m_lastKeyboardState[i] == KEY_HELD )
				{
					//if the key was pressed or held last frame, and is still being held change the state to KEY_HELD
					m_keyboardState[i] = KEY_HELD;
					m_renderWindow->BroadcastKeyboardEvent( Kiwi::KeyboardEvent( m_renderWindow, (Kiwi::KEY)i, KEY_HELD ) );

				} else if( m_lastKeyboardState[i] == KEY_UP )
				{
					//key was pressed
					m_renderWindow->BroadcastKeyboardEvent( Kiwi::KeyboardEvent( m_renderWindow, (Kiwi::KEY)i, KEY_PRESSED ) );
				}
			}
		}

	}

	void RawInputWrapper::ProcessInput( WPARAM wParam, LPARAM lParam )
	{

		RAWINPUT InputData;

		UINT DataSize = sizeof( RAWINPUT );
		GetRawInputData( (HRAWINPUT)lParam,
						 RID_INPUT,
						 &InputData,
						 &DataSize,
						 sizeof( RAWINPUTHEADER ) );

		if( InputData.header.dwType == RIM_TYPEMOUSE )
		{
			// read the mouse data
			m_deltaMouse.x += (float)InputData.data.mouse.lLastX;
			m_deltaMouse.y += (float)InputData.data.mouse.lLastY;
			
			if( InputData.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_LEFT] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_LEFT_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_LEFT] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_RIGHT] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_RIGHT_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_RIGHT] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_MIDDLE] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_MIDDLE_BUTTON_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_MIDDLE] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_4] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_4_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_4] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_DOWN )
			{
				m_currentMouseState.mouseStates[MOUSE_5] = BUTTON_PRESSED;

			} else if( InputData.data.mouse.ulButtons & RI_MOUSE_BUTTON_5_UP )
			{
				m_currentMouseState.mouseStates[MOUSE_5] = BUTTON_RELEASED;
			}

			if( InputData.data.mouse.ulButtons & RI_MOUSE_WHEEL )
			{
				m_wheelDelta += InputData.data.mouse.usButtonData;
			}

		}

	}

	bool RawInputWrapper::QueryKeyState( KEY key, KEY_STATE keyState )
	{

		if( m_keyboardState[key] == keyState )
		{
			return true;

		} else if( keyState == KEY_ACTIVE && (m_keyboardState[key] == KEY_PRESSED || m_keyboardState[key] == KEY_HELD) )
		{
			return true;

		}else if(keyState == KEY_INACTIVE && (m_keyboardState[key] == KEY_RELEASED || m_keyboardState[key] == KEY_UP) )
		{
			return true;
		}

		return false;

	}

}