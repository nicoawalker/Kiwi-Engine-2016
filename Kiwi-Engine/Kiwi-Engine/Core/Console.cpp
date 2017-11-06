#include "Console.h"

#include "EngineRoot.h"
#include "Scene.h"

#include "../Graphics/UI/UITextBox.h"
#include "../Graphics/UI/UIScrollBar.h"
#include "../Graphics/Texture.h"
#include "../Graphics/RenderWindow.h"

namespace Kiwi
{

	Console::Console( Kiwi::EngineRoot& engine, std::wstring logFile )
	{

		m_engine = &engine;
		m_textBox = 0;
		m_scene = 0;
		m_hidden = true;
		m_debugEnabled = true;

		m_textBoxArrows = 0;
		m_textBoxBackground = 0;
		m_textBoxScrollBar = 0;
		m_textBoxSlider = 0;

		m_textColor = Kiwi::Color( 1.0, 1.0, 1.0, 1.0 );
		m_textBoxScrollBarArrowColor = Kiwi::Color( 0.0, 0.0, 0.0, 1.0 ); 
		
		m_textBoxScrollBarColor.Set( 0.7, 0.7, 0.7, 1.0 );
		m_textBoxScrollBarSliderColor.Set( 0.5, 0.5, 0.5, 1.0 );

		this->Initialize( logFile );

	}

	Console::~Console()
	{

	}

	void Console::_Show()
	{

		if( m_textBox )
		{
			m_textBox->SetActive( true );
		}

	}

	void Console::_Hide()
	{

		if( m_textBox )
		{
			m_textBox->SetActive( false );
		}

	}

	void Console::_Print( std::wstring message, const Kiwi::Color& color )
	{

		ConsoleMessage msg = { color, message };
		m_messages.push_back( msg );

		if( m_textBox )
		{
			m_textBox->AddLine( message, color );
		}

	}

	void Console::Shutdown()
	{

		m_textBox = 0;
		m_textBoxArrows = 0;
		m_textBoxBackground = 0;
		m_textBoxScrollBar = 0;
		m_textBoxSlider = 0;

	}

	void Console::Update()
	{

		std::lock_guard<std::mutex> guard( m_consoleMutex );

		if( m_scene && m_scene->GetRenderWindow() )
		{
			if( m_scene->GetRenderWindow()->GetInput().QueryKeyState( Kiwi::KEY::OEM3, Kiwi::KEY_RELEASED ) )
			{//if tilde key is released toggle hide
				if( m_hidden )
				{
					m_hidden = false;
					this->_Show();

				} else
				{
					m_hidden = true;
					this->_Hide();
				}
			}
		}

	}

	void Console::InitializeInterface( Kiwi::Scene& scene, std::wstring textFont, const Kiwi::Vector2d& dimensions, const Kiwi::Vector3d& position )
	{

		if( m_scene )
		{
			if( m_scene == &scene )
			{
				return;
			}
		}

		m_scene = &scene;

		if( m_scene->GetRenderWindow() == 0 )
		{
			return;
		}

		m_textBox = 0;

		/*m_interface = new Kiwi::UserInterface( scene );
		if( m_interface )
		{

			m_textBox = new Kiwi::UITextBox( L"console/outputbox", *m_interface, Kiwi::UITextBox::MULTILINE_SCROLLABLE, textFont, dimensions );
			if( m_textBox )
			{
				m_textBoxBackground = m_scene->FindAsset<Kiwi::Texture>( L"ConsoleTextField" );
				m_textBoxArrows = m_scene->FindAsset<Kiwi::Texture>( L"ConsoleScrollArrow" );

				m_textBox->SetTexture( m_textBoxBackground );
				m_textBox->SetPadding( Kiwi::Vector4( 3.0f, 3.0f, 0.0f, 0.0f ) );
				Kiwi::UIScrollBar* scrollBar = m_textBox->GetScrollBar();
				if( scrollBar )
				{
					scrollBar->SetBackgroundTexture( m_textBoxScrollBar );
					scrollBar->SetButton1Texture( m_textBoxArrows );
					scrollBar->SetButton2Texture( m_textBoxArrows );
					scrollBar->SetSliderTexture( m_textBoxSlider );
					scrollBar->SetBackgroundColor( m_textBoxScrollBarColor );
					scrollBar->SetSliderColor( m_textBoxScrollBarSliderColor );
					scrollBar->SetButton1Color( m_textBoxScrollBarArrowColor );
					scrollBar->SetButton2Color( m_textBoxScrollBarArrowColor );
				}

				m_interface->AddComponent( m_textBox );
			}

			Kiwi::Transform* tbTransform = m_textBox->FindComponent<Kiwi::Transform>();
			if( tbTransform )
			{
				tbTransform->SetPosition( Kiwi::Vector3d( position ) );
			}

			this->_Print( L"Console Initialized", Kiwi::Color(1.0, 1.0, 1.0, 1.0) );

			for( unsigned int i = 0; i < m_messages.size(); i++ )
			{
				m_textBox->AddLine( m_messages[i].message, m_messages[i].textColor );
			}

			this->_Hide();
		}*/

	}

	void Console::PrintDebug( std::wstring message )
	{

		if( m_debugEnabled == false ) return;

		std::lock_guard<std::mutex> guard( m_consoleMutex );

		this->_Print( message, m_textColor );

	}

	void Console::Print( std::wstring message )
	{

		std::lock_guard<std::mutex> guard( m_consoleMutex );

		this->_Print( message, m_textColor );

	}

	void Console::Print( std::wstring message, const Kiwi::Color& color )
	{

		std::lock_guard<std::mutex> guard( m_consoleMutex );

		this->_Print( message, color );

	}

	void Console::Write( std::wstring message )
	{

		std::lock_guard<std::mutex> guard( m_consoleMutex );

		this->Log( message );

	}

}