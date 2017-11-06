#ifndef _KIWI_UITEXTBOX_H_
#define _KIWI_UITEXTBOX_H_

#include "Events\UIScrollBarEventListener.h"

#include "..\Text.h"

#include "..\..\Core\Component.h"
#include "../../Core/Vector2d.h"
#include "..\..\Core\Vector4.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class UserInterface;
	class Texture;
	class UIScrollBar;
	class UISprite;

	class UITextBox:
		public Kiwi::Component,
		public Kiwi::UIScrollBarEventListener
	{
	public:

		enum TEXTBOX_TYPE { SINGLE_LINE, MULTILINE, MULTILINE_SCROLLABLE };

	protected:

		struct TextLine
		{
			std::wstring text;
			Kiwi::Color lineColor;
			unsigned int lineNumber;
			unsigned int lineCount; //if multiple of the same message are added in one update cycle this contains that number
			Kiwi::Font::ALIGNMENT alignment;

			bool operator< ( const TextLine& l1 )
			{
				return text < l1.text;
			}
		};

		TEXTBOX_TYPE m_textboxType;
		Kiwi::Font::ALIGNMENT m_textAlignment;

		Kiwi::Vector2d m_dimensions;

		Kiwi::Text* m_textField;
		std::vector<TextLine> m_text;
		std::vector<TextLine> m_tempText; //holds all lines added since the last call to FixedUpdate

		Kiwi::UIScrollBar* m_scrollBar;

		std::wstring m_textFont;

		Kiwi::UISprite* m_backgroundSprite;

		int m_visibleLineCount; //how many lines of text are visible
		int m_lineHeight;
		int m_textAreaHeight;

		bool m_suppressNew; //while true, no new text lines can be added
		bool m_updateText; //if true, the visible text will be updated/recreated on the next fixed update

		std::mutex m_textBoxMutex;

		Kiwi::Vector4 m_padding;

	protected:

		virtual void _OnAttached();

		void _UpdateVisibleText();
		virtual void _OnFixedUpdate();

	public:

		UITextBox( std::wstring name, TEXTBOX_TYPE textboxType, std::wstring textFont, const Kiwi::Vector2d& dimensions );
		~UITextBox();

		void AddLine( std::wstring text );
		void AddLine( std::wstring text, const Kiwi::Color& color );

		void SetDimensions( const Kiwi::Vector2d& dimensions ) {}

		/*sets the padding around the text field in TLBR oreder*/
		void SetPadding( const Kiwi::Vector4& padding );

		void SetTexture( std::wstring textureName );

		Kiwi::UIScrollBar* GetScrollBar()const { return m_scrollBar; }

		virtual void OnScrollEvent( const Kiwi::UIScrollBarEvent& evt );
		virtual void OnTranslate( const Kiwi::TransformEvent& evt );

	};
}

#endif