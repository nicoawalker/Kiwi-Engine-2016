#ifndef _KIWI_UISCROLLBAR_H_
#define _KIWI_UISCROLLBAR_H_

#include "Events/UIButtonEventListener.h"
#include "Events/UIScrollBarEventBroadcaster.h"

#include "..\Color.h"

#include "..\..\Core\Component.h"
#include "../../Core/Vector2d.h"

#include <string>

namespace Kiwi
{

	class Texture;
	class UIButton;
	class UISprite;
	class UserInterface;

	class UIScrollBar:
		public Kiwi::Component,
		public Kiwi::UIButtonEventListener,
		public Kiwi::UIScrollBarEventBroadcaster
	{
	protected:

		Kiwi::UIButton* m_button1;
		Kiwi::UIButton* m_button2;
		Kiwi::UIButton* m_slider;
		Kiwi::UISprite* m_bgSprite;

		std::wstring m_buttonTexture;
		std::wstring m_sliderTexture;
		std::wstring m_backgroundTexture;

		Kiwi::Vector2d m_dimensions;

		double m_scrollFrequency; //when a button is held, how many times per second a scroll event will occur
		double m_scrollTimer;

		unsigned int m_linesPerScroll; //how many lines are advanced each time button1 or button2 is pressed/held

		unsigned int m_lineCount; //total number of lines
		unsigned int m_currentLine; //line the slider is currently on

		double m_scrollStepY; //change in position of scroll bar for each line

		int m_sliderHeight;
		int m_minSliderHeight; //slider will not shrink any smaller than this

		int m_sliderBarHeight; //height of the bar in which the slider moves

		double m_sliderYMax; //stores the position of the slider if it is at the top of the scroll bar
		double m_sliderYMin;

		double m_sliderY; //stores the current slider y position

	protected:

		void _OnAttached();

		void _UpdateSlider();
		void _UpdateSliderPosition();

		virtual void _OnButtonPress( const Kiwi::UIButtonEvent& evt );
		virtual void _OnButtonHeld( const Kiwi::UIButtonEvent& evt );
		virtual void _OnButtonRelease( const Kiwi::UIButtonEvent& evt );

	public:

		UIScrollBar( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring buttonTexture = L"", std::wstring sliderTexture = L"", std::wstring backgroundTexture = L"" );
		~UIScrollBar();

		void SetBackgroundColor( const Kiwi::Color& color );
		void SetSliderColor( const Kiwi::Color& color );
		void SetButton1Color( const Kiwi::Color& color );
		void SetButton2Color( const Kiwi::Color& color );

		void SetButton1Texture( std::wstring textureName );
		void SetButton2Texture( std::wstring textureName );
		void SetSliderTexture( std::wstring textureName );
		void SetBackgroundTexture( std::wstring textureName );

		void SetScrollSpeed( unsigned int scrollSpeed ) { m_linesPerScroll = scrollSpeed; }
		void SetLineCount( int lineCount );
		void SetMinimumSliderHeight( int height );
		void ScrollToLine( unsigned int line );
		void SetScrollFrequency( double scrollsPerSecond );

		unsigned int GetLineCount()const { return m_lineCount; }
		unsigned int GetCurrentLine()const { return m_currentLine; }
		unsigned int GetScrollSpeed()const { return m_linesPerScroll; }

		double GetScrollFrequency()const { return m_scrollFrequency; }
		double GetMinimumSliderHeight()const { return m_minSliderHeight; }

	};
}

#endif