#include "UIButton.h"
#include "UISprite.h"

#include "..\Renderable2D.h"
#include "../RenderWindow.h"
#include "..\Text.h"

#include "..\..\Core\Scene.h"
#include "../../Core/RawInputWrapper.h"

namespace Kiwi
{

	UIButton::UIButton( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring textureName ) :
		Kiwi::Component( name )
	{

		m_textureName = textureName;
		m_dimensions = dimensions;
		m_label = 0;
		m_buttonSprite = 0;

	}

	UIButton::UIButton( std::wstring name, const Kiwi::Vector2d& dimensions, std::wstring label, std::wstring labelFont, std::wstring textureName ) :
		Kiwi::Component( name )
	{

		m_labelFont = labelFont;
		m_labelText = label;
		m_textureName = textureName;
		m_dimensions = dimensions;
		m_label = 0;
		m_buttonSprite = 0;

	}

	UIButton::~UIButton()
	{
	}

	void UIButton::_OnAttached()
	{

		Kiwi::Scene* scene = m_entity->GetScene();
		assert( scene != 0 );
		if( scene == 0 )
		{
			return;
		}

		m_entity->SetEntityType( Kiwi::Entity::ENTITY_2D );

		m_buttonSprite = new Kiwi::UISprite( m_objectName + L"/sprite", m_textureName );
		//m_entity->FindComponent<Kiwi::Transform>()->SetPosition( Kiwi::Vector3d( m_entity->FindComponent<Kiwi::Transform>()->GetPosition().x, m_entity->FindComponent<Kiwi::Transform>()->GetPosition().y, m_entity->FindComponent<Kiwi::Transform>()->GetPosition().z ) );
		m_entity->FindComponent<Kiwi::Transform>()->SetScale( Kiwi::Vector3d( m_dimensions.x, m_dimensions.y, 1.0 ) );
		m_entity->AttachComponent( m_buttonSprite );

		if( m_labelText.size() > 0 )
		{
			if( m_label == 0 )
			{
				Kiwi::Entity* labelEntity = scene->CreateEntity( m_objectName + L"/label" );
				if( labelEntity != 0 )
				{
					m_label = new Kiwi::Text( m_objectName + L"/label", m_labelFont, m_dimensions );
					if( m_label != 0 )
					{
						m_label->SetAlignment( Kiwi::Font::ALIGN_CENTRE );
						m_label->SetText( m_labelText );
						labelEntity->FindComponent<Kiwi::Transform>()->SetPosition( Kiwi::Vector3( 0.0, 0.0, m_entity->FindComponent<Kiwi::Transform>()->GetPosition().z + 20.0 ) );
						labelEntity->AttachComponent( m_label );
						m_entity->AttachChild( labelEntity );
					}
				}
			}
		}

		m_mouseOver = false;

		Kiwi::RenderWindow* rw = scene->GetRenderWindow();
		assert( rw != 0 );
		if( rw != 0 )
		{
			rw->AddMouseListener( this );
		}

	}

	void UIButton::_OnShutdown()
	{

		if( m_entity != 0 )
		{
			Kiwi::Scene* scene = m_entity->GetScene();
			Kiwi::RenderWindow* rw = scene->GetRenderWindow();
			assert( rw != 0 );
			rw->RemoveMouseListener( this );
		}

	}

	void UIButton::OnMouseEnter()
	{

		

	}

	void UIButton::OnMouseHover()
	{



	}

	void UIButton::OnMouseExit()
	{



	}

	void UIButton::OnMousePress( const Kiwi::MouseEvent& evt )
	{
		
		if( !m_isActive ) return;

		if( m_mouseOver && evt.GetButton() == Kiwi::MOUSE_BUTTON::MOUSE_LEFT )
		{
			m_pressed = true;
			this->BroadcastButtonEvent( Kiwi::UIButtonEvent( this, UIButtonEvent::BUTTON_PRESS, Kiwi::Vector2d( evt.GetPos().x, evt.GetPos().y) ) );
		}

	}

	void UIButton::OnMouseRelease( const Kiwi::MouseEvent& evt )
	{

		if( !m_isActive || !m_pressed ) return;

		if( evt.GetButton() == Kiwi::MOUSE_BUTTON::MOUSE_LEFT )
		{
			m_pressed = false;
			this->BroadcastButtonEvent( Kiwi::UIButtonEvent( this, UIButtonEvent::BUTTON_RELEASE, Kiwi::Vector2d( evt.GetPos().x, evt.GetPos().y ) ) );
		}

	}

	void UIButton::OnMouseHeld( const Kiwi::MouseEvent& evt )
	{

		if( !m_isActive ) return;

		if( m_mouseOver && m_pressed )
		{
			this->BroadcastButtonEvent( Kiwi::UIButtonEvent( this, UIButtonEvent::BUTTON_HELD, Kiwi::Vector2d( evt.GetPos().x, evt.GetPos().y ) ) );
		}

	}

	void UIButton::OnMouseMove( const Kiwi::MouseEvent& evt )
	{

		if( !m_isActive ) return;

		Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
		assert( transform != 0 );
		if( transform == 0 )
		{
			return;
		}
		
		Kiwi::Vector2 mousePos = evt.GetPos();

		Kiwi::Vector3d pos = transform->GetPosition();
		double width = transform->GetScale().x;
		double height = transform->GetScale().y;

		if( mousePos.x > (pos.x - (width / 2.0)) && mousePos.x < (pos.x + (width/2.0)) &&
			mousePos.y < (pos.y + (height/2.0)) && mousePos.y > (pos.y - (height/2.0)) )
		{//mouse within button
			if( m_mouseOver )
			{
				this->OnMouseHover();

			} else
			{
				m_mouseOver = true;
				this->OnMouseEnter();
			}

		} else
		{
			if( m_mouseOver )
			{
				m_mouseOver = false;
				this->OnMouseExit();
			}
		}

	}

	void UIButton::SetTexture( std::wstring textureName )
	{

		if( m_buttonSprite )
		{
			m_buttonSprite->SetTexture( textureName );
		}

	}

	void UIButton::SetShader( std::wstring shaderName )
	{

		m_shaderName = shaderName;

		if( m_label != 0 )
		{
			m_label->SetShader( shaderName );
		}

	}

}