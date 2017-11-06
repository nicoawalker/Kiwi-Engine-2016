#ifndef _KIWI_UIBUTTON_H_
#define _KIWI_UIBUTTON_H_

#include "Events\UIButtonEventBroadcaster.h"

#include "..\..\Core\Component.h"
#include "../../Core/Vector2d.h"
#include "..\..\Core\IMouseEventListener.h"

#include <string>

namespace Kiwi
{

	class UISprite;
	class Text;

	class UIButton :
		public Kiwi::Component,
		public Kiwi::UIButtonEventBroadcaster,
		public Kiwi::IMouseEventListener
	{
	protected:

		Kiwi::UISprite* m_buttonSprite;
		Kiwi::Text* m_label;

		Kiwi::Vector2d m_dimensions;

		std::wstring m_labelFont;
		std::wstring m_labelText;
		std::wstring m_textureName;
		std::wstring m_shaderName;

		bool m_mouseOver;
		bool m_pressed;

	protected:

		virtual void _OnAttached();
		virtual void _OnShutdown();

	public:

		/*creates an unlabeled button*/
		UIButton( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring buttonTextureName = L"" );

		/*creates a button with a label*/
		UIButton( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring label, std::wstring labelFont, std::wstring buttonTextureName = L"" );
		virtual ~UIButton();

		virtual void OnMouseEnter();
		virtual void OnMouseExit();
		virtual void OnMouseHover();

		virtual void OnMousePress( const Kiwi::MouseEvent& evt );
		virtual void OnMouseRelease( const Kiwi::MouseEvent& evt );
		virtual void OnMouseHeld( const Kiwi::MouseEvent& evt );
		virtual void OnMouseMove( const Kiwi::MouseEvent& evt );

		void SetTexture( std::wstring textureName );
		void SetLabel( std::wstring label );
		void SetShader( std::wstring shader );

		bool IsPressed()const { return m_pressed; }

		Kiwi::UISprite* GetSprite()const { return m_buttonSprite; }
		std::wstring GetLabel()const { return m_labelText; }

	};
}

#endif