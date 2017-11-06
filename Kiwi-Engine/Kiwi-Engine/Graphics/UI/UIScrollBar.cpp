#include "UIScrollBar.h"
#include "UIButton.h"
#include "UISprite.h"

#include "../Texture.h"

#include "../../Core/Scene.h"
#include "../../Core/EngineRoot.h"

namespace Kiwi
{

	UIScrollBar::UIScrollBar( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring buttonTexture, std::wstring sliderTexture, std::wstring backgroundTexture ) :
		Kiwi::Component( name )
	{

		m_button1 = 0;
		m_button2 = 0;
		m_bgSprite = 0;
		m_slider = 0;

		m_buttonTexture = buttonTexture;
		m_sliderTexture = sliderTexture;
		m_backgroundTexture = backgroundTexture;

		m_minSliderHeight = 10;
		m_currentLine = 1;
		m_linesPerScroll = 1;
		m_scrollFrequency = 5.0;
		m_scrollTimer = 0.0;
		m_lineCount = 1;

		m_dimensions = dimensions;
		m_dimensions.x = (m_dimensions.x < 0.0) ? 0.0 : std::floor( m_dimensions.x );
		m_dimensions.y = (m_dimensions.y < 0.0) ? 0.0 : std::floor( m_dimensions.y );

	}

	UIScrollBar::~UIScrollBar()
	{
	}

	void UIScrollBar::_OnAttached()
	{

		Kiwi::Scene* scene = m_entity->GetScene();
		Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
		assert( transform != 0 && scene != 0 );
		if( transform == 0 || scene == 0 )
		{
			return;
		}

		Kiwi::Vector3d currentPos = transform->GetPosition();
		currentPos.x = std::floor( currentPos.x );
		currentPos.y = std::floor( currentPos.y );

		//transform->SetScale( Kiwi::Vector3( m_dimensions.x, m_dimensions.y, 1.0 ) );
		//transform->SetPosition( Kiwi::Vector3( currentPos.x, currentPos.y, currentPos.z) );

		Kiwi::Entity* button1Entity = scene->CreateEntity( m_objectName + L"/button1" );
		if( button1Entity != 0 )
		{
			Kiwi::Vector2d button1Dimensions( (int)m_dimensions.x, (int)m_dimensions.x );
			Kiwi::Vector3d button1Pos( currentPos.x, (int)(currentPos.y + ((m_dimensions.y / 2.0) - ceil( button1Dimensions.y / 2.0 ))), currentPos.z + 0.2 );
			m_button1 = new Kiwi::UIButton( L"Button1", button1Dimensions, L"", L"Lato_20pt", m_buttonTexture );
			m_button1->AddButtonEventListener( this );

			Kiwi::Transform* b1eTransform = button1Entity->FindComponent<Kiwi::Transform>();
			if( b1eTransform != 0 )
			{
				b1eTransform->SetPosition( button1Pos );
			}
			button1Entity->AttachComponent( m_button1 );
			m_entity->AttachChild( button1Entity );
		}

		Kiwi::Entity* button2Entity = scene->CreateEntity( m_objectName + L"/button2" );
		if( button2Entity != 0 )
		{
			Kiwi::Vector2d button2Dimensions( (int)m_dimensions.x, (int)m_dimensions.x );
			Kiwi::Vector3d button2Pos( currentPos.x, (int)(currentPos.y - ((m_dimensions.y / 2.0) - ceil( button2Dimensions.y / 2.0 ))), currentPos.z + 0.2 );
			m_button2 = new Kiwi::UIButton( L"Button2", button2Dimensions, L"", L"Lato_20pt", m_buttonTexture );
			m_button2->AddButtonEventListener( this );

			Kiwi::Transform* b2eTransform = button2Entity->FindComponent<Kiwi::Transform>();
			if( b2eTransform != 0 )
			{
				b2eTransform->SetPosition( button2Pos );
				b2eTransform->SetRotation( Kiwi::Quaternion( Kiwi::Vector3::forward(), Kiwi::ToRadians( 180.0f ) ) );
			}
			button2Entity->AttachComponent( m_button2 );
			m_entity->AttachChild( button2Entity );
		}

		m_sliderBarHeight = (int)(m_dimensions.y - (2.0 * m_dimensions.x));
		m_sliderHeight = m_sliderBarHeight;
		m_sliderYMax = currentPos.y + (m_sliderBarHeight / 2.0) - (m_sliderHeight / 2.0);
		m_sliderYMin = currentPos.y - (m_sliderBarHeight / 2.0) + (m_sliderHeight / 2.0);
		m_sliderY = m_sliderYMax;

		Kiwi::Entity* sliderEntity = scene->CreateEntity( m_objectName + L"/slider" );
		if( sliderEntity != 0 )
		{
			Kiwi::Vector2d barDim( m_dimensions.x, m_sliderBarHeight );
			Kiwi::Vector3d barPos( currentPos.x, m_sliderYMax, currentPos.z + 0.1 );
			m_slider = new Kiwi::UIButton( L"Slider", barDim, L"", L"Lato_20pt", m_sliderTexture );
			m_slider->AddButtonEventListener( this );

			sliderEntity->AttachComponent( m_slider );
			Kiwi::UISprite* sliderSprite = m_slider->GetSprite();
			if( sliderSprite != 0 )
			{
				sliderSprite->SetColor( Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );
			}

			Kiwi::Transform* sliderTransform = sliderEntity->FindComponent<Kiwi::Transform>();
			if( sliderTransform )
			{
				sliderTransform->SetPosition( barPos );
			}

			m_entity->AttachChild( sliderEntity );
		}

		Kiwi::Entity* barEntity = scene->CreateEntity( m_objectName + L"/bar" );
		if( barEntity != 0 )
		{
			m_bgSprite = new Kiwi::UISprite( L"Background", m_backgroundTexture );

			Kiwi::Transform* barTransform = barEntity->FindComponent<Kiwi::Transform>();
			if( barTransform )
			{
				barTransform->SetScale( Kiwi::Vector3( m_dimensions.x, m_sliderBarHeight, 1.0 ) );
			}

			barEntity->AttachComponent( m_bgSprite );
			m_bgSprite->SetColor( Kiwi::Color( 0.0, 0.0, 0.0, 1.0 ) );

			m_entity->AttachChild( barEntity );
		}

		m_entity->SetEntityType( Kiwi::Entity::ENTITY_2D );

		this->SetLineCount( 5 );

	}

	void UIScrollBar::_UpdateSlider()
	{
		
		if( m_entity )
		{

			Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
			assert( transform != 0 );

			m_sliderHeight = max( m_sliderBarHeight / (int)m_lineCount, m_minSliderHeight );
			m_sliderYMax = (transform->GetPosition().y + (m_sliderBarHeight / 2.0)) - (m_sliderHeight / 2.0);
			m_sliderYMin = (transform->GetPosition().y - (m_sliderBarHeight / 2.0)) + (m_sliderHeight / 2.0);
			m_scrollStepY = (double)m_sliderBarHeight / (double)m_lineCount;

			if( m_slider != 0 )
			{
				Kiwi::Entity* slider = m_slider->GetEntity();
				if( slider != 0 )
				{
					Kiwi::Transform* sliderTransform = slider->FindComponent<Kiwi::Transform>();
					if( sliderTransform != 0 )
					{
						sliderTransform->SetScale( Kiwi::Vector3d( sliderTransform->GetScale().x, (double)m_sliderHeight, 1.0 ) );
					}
				}
			}
		}

	}

	void UIScrollBar::_UpdateSliderPosition()
	{

		if( m_entity )
		{
			Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
			assert( transform != 0 );

			m_sliderY = m_sliderYMax - ((m_currentLine - 1) * m_scrollStepY);
			Kiwi::clamp( m_sliderY, m_sliderYMin, m_sliderYMax );

			if( m_slider != 0 )
			{
				Kiwi::Entity* slider = m_slider->GetEntity();
				if( slider != 0 )
				{
					Kiwi::Transform* sliderTransform = slider->FindComponent<Kiwi::Transform>();
					if( sliderTransform != 0 )
					{
						sliderTransform->SetPosition( Kiwi::Vector3d( sliderTransform->GetPosition().x, m_sliderY, transform->GetPosition().z + 0.1 ) );
					}
				}
			}
		}

	}

	void UIScrollBar::SetBackgroundColor( const Kiwi::Color& color )
	{

		if( m_bgSprite )
		{
			m_bgSprite->SetColor( color );
		}

	}

	void UIScrollBar::SetSliderColor( const Kiwi::Color& color )
	{

		if( m_slider && m_slider->GetSprite() )
		{
			m_slider->GetSprite()->SetColor( color );
		}

	}

	void UIScrollBar::SetButton1Color( const Kiwi::Color& color )
	{

		if( m_button1 && m_button1->GetSprite() )
		{
			m_button1->GetSprite()->SetColor( color );
		}

	}

	void UIScrollBar::SetButton2Color( const Kiwi::Color& color )
	{

		if( m_button2 && m_button2->GetSprite() )
		{
			m_button2->GetSprite()->SetColor( color );
		}

	}

	void UIScrollBar::SetButton1Texture( std::wstring textureName )
	{

		if( m_button1 )
		{
			m_button1->SetTexture( textureName );
		}

	} 

	void UIScrollBar::SetButton2Texture( std::wstring textureName )
	{

		if( m_button2 )
		{
			m_button2->SetTexture( textureName );
		}

	}

	void UIScrollBar::SetSliderTexture( std::wstring textureName )
	{

		if( m_slider )
		{
			m_slider->SetTexture( textureName );
		}

	}

	void UIScrollBar::SetBackgroundTexture( std::wstring textureName )
	{

		if( m_bgSprite )
		{
			m_bgSprite->SetTexture( textureName );
		}

	}

	void UIScrollBar::SetLineCount( int lineCount )
	{
		
		if( lineCount < 1 ) lineCount = 1;

		m_lineCount = lineCount;

		this->_UpdateSlider();
		this->_UpdateSliderPosition();

	}

	void UIScrollBar::SetMinimumSliderHeight( int height )
	{

		m_minSliderHeight = height;

	}

	void UIScrollBar::ScrollToLine( unsigned int position )
	{

		m_currentLine = position;
		Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

		this->_UpdateSliderPosition();

	}

	void UIScrollBar::_OnButtonPress( const Kiwi::UIButtonEvent& evt )
	{

		Kiwi::UIButton* button = evt.GetSource();
		if( button == m_button1 ) //up / left button
		{
			if( m_currentLine > 1 )
			{
				m_currentLine -= m_linesPerScroll;
				Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

				this->_UpdateSliderPosition();

				this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, -1.0, m_currentLine, 1.0, m_lineCount ) );
			}

		}else if( button == m_button2 ) //down / right button
		{
			if( m_currentLine < m_lineCount )
			{
				m_currentLine += m_linesPerScroll;
				Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

				this->_UpdateSliderPosition();

				this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, 1.0, m_currentLine, 1.0, m_lineCount ) );
			}

		}

	}

	void UIScrollBar::_OnButtonHeld( const Kiwi::UIButtonEvent& evt )
	{

		if( m_entity && m_entity->GetScene() )
		{
			Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
			assert( transform != 0 );

			Kiwi::UIButton* button = evt.GetSource();

			double deltaTime = m_entity->GetScene()->GetEngine()->GetGameTimer()->GetDeltaTime();
			m_scrollTimer += deltaTime;

			if( m_scrollTimer > 1.0 / (double)m_scrollFrequency )
			{
				m_scrollTimer -= 1.0 / (double)m_scrollFrequency;

				if( button == m_button1 )
				{
					if( m_currentLine > 1 )
					{
						m_currentLine -= m_linesPerScroll;
						Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

						this->_UpdateSliderPosition();

						this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, -1.0, m_currentLine, 1.0, m_lineCount ) );
					}

				} else if( button == m_button2 )
				{
					if( m_currentLine < m_lineCount )
					{
						m_currentLine += m_linesPerScroll;
						Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

						this->_UpdateSliderPosition();

						this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, 1.0, m_currentLine, 1.0, m_lineCount ) );
					}
				}

			} else if( button == m_slider )
			{
				Kiwi::Vector2 mDelta = m_entity->GetScene()->GetRenderWindow()->GetInput().GetMouseDeltaPosition();
				mDelta.y /= m_entity->GetScene()->GetRenderWindow()->GetInput().GetMouseSpeed().y;

				m_sliderY -= mDelta.y;
				Kiwi::clamp( m_sliderY, m_sliderYMin, m_sliderYMax );
				m_currentLine = (unsigned int)ceil( (m_sliderYMax - round( m_sliderY )) / m_scrollStepY );
				Kiwi::clamp( m_currentLine, (unsigned int)1, m_lineCount );

				if( m_slider != 0 )
				{
					Kiwi::Entity* slider = m_slider->GetEntity();
					if( slider != 0 )
					{
						Kiwi::Transform* sliderTransform = slider->FindComponent<Kiwi::Transform>();
						if( sliderTransform != 0 )
						{
							sliderTransform->SetPosition( Kiwi::Vector3d( sliderTransform->GetPosition().x, m_sliderY, sliderTransform->GetPosition().z ) );
						}
					}
				}

				this->BroadcastScrollBarEvent( Kiwi::UIScrollBarEvent( this, mDelta.y, m_currentLine, 1.0, m_lineCount ) );
			}
		}

	}

	void UIScrollBar::_OnButtonRelease( const Kiwi::UIButtonEvent& evt )
	{

		m_scrollTimer = 0.0;

	}

}