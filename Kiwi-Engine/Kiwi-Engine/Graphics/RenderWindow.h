#ifndef _KIWI_RENDERWINDOW_H_
#define _KIWI_RENDERWINDOW_H_

#include "../Core/Vector2.h"
#include "..\Core\MouseEventBroadcaster.h"
#include "../Core/Events/KeyboardEventBroadcaster.h"

#include "WindowEventBroadcaster.h"

#include <Windows.h>
#include <string>

namespace Kiwi
{

	class WindowEvent;
	class Renderer;
	class RawInputWrapper;

	class RenderWindow :
		public Kiwi::WindowEventBroadcaster,
		public Kiwi::MouseEventBroadcaster,
		public Kiwi::KeyboardEventBroadcaster
	{
	protected:

		HWND m_hwnd;

		HINSTANCE m_hInst;

		std::wstring m_name;

		Kiwi::Vector2 m_position;
		Kiwi::Vector2 m_dimensions;

		RawInputWrapper m_inputDevice;

		//if set, only this listener will receive any mouse events
		Kiwi::IMouseEventListener* m_mouseFocus;

		bool m_vsyncEnabled;
		bool m_isVisible;
		bool m_isFullscreen;

	public:

		RenderWindow( std::wstring windowName, DWORD style, DWORD exStyle, int x, int y,
					  int width, int height, bool vsyncEnabled, HWND parent, HMENU menu );
		virtual ~RenderWindow();

		void Shutdown();

		LRESULT HandleMessage(UINT msg, WPARAM wParam, LPARAM lParam);

		void Update( float deltaTime );

		bool Show(int cmdShow);

		bool CenterWindow(HWND parent);

		void RestrictMouse(bool isRestricted);

		static LRESULT CALLBACK WindowProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam);

		/*registers the listener as the mouse focus
		if a focus is set, only the corresponding listener will receive mouse events until the focus is released*/
		void SetMouseFocus( Kiwi::IMouseEventListener* listener ) { m_mouseFocus = listener; }

		/*removes any active mouse focus so that all mouse listeners will receive mouse events*/
		void ReleaseMouseFocus() { m_mouseFocus = 0; }

		bool IsFullscreen()const { return m_isFullscreen; }
		bool IsVisible()const { return m_isVisible; }
		bool GetVSyncEnabled()const { return m_vsyncEnabled; }
		HWND GetHandle()const { return m_hwnd; }
		HINSTANCE GetInstance()const { return m_hInst; }
		RawInputWrapper& GetInput() { return m_inputDevice; }
		Kiwi::Vector2 GetPosition();
		Kiwi::Vector2 GetWindowSize();
		Kiwi::Vector2 GetClientSize();

		//void BroadcastMouseEvent( const Kiwi::MouseEvent& evt );

	};
};

#endif