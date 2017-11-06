#include "UITextBox.h"
#include "UIScrollBar.h"
#include "UISprite.h"

#include "../Texture.h"

#include "../../Core/Utilities.h"
#include "../../Core/Scene.h"

namespace Kiwi
{
	
	UITextBox::UITextBox( std::wstring name, TEXTBOX_TYPE textboxType, std::wstring textFont, const Kiwi::Vector2d& dimensions ) :
		Kiwi::Component( name )
	{

		m_dimensions = dimensions;
		m_textFont = textFont;
		m_textboxType = textboxType;
		m_updateText = false;
		m_suppressNew = false;
		m_lineHeight = 0;
		m_visibleLineCount = 0;
		m_textAreaHeight = 0;

		m_backgroundSprite = 0;
		m_scrollBar = 0;
		m_textField = 0;

	}

	UITextBox::~UITextBox()
	{
	}

	void UITextBox::_OnAttached()
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

		/*transform->SetScale( Kiwi::Vector3d( m_dimensions.x, m_dimensions.y, 1.0 ) );
		transform->SetPosition( Kiwi::Vector3d( currentPos.x, currentPos.y, currentPos.z ) );*/

		Kiwi::Font* font = scene->FindAsset<Kiwi::Font>( m_textFont );
		if( font == 0 )
		{
			throw Kiwi::Exception( L"UITextBox", L"Font '" + m_textFont + L"' does not exist" );
		}

		Kiwi::Entity* textEntity = scene->CreateEntity( m_objectName + L"/textentity" );
		if( textEntity != 0 )
		{
			m_textField = new Kiwi::Text( m_objectName + L"/textfield", m_textFont, Kiwi::Vector2d( m_dimensions.x - (m_padding.x + m_padding.z), m_dimensions.y - (m_padding.y + m_padding.w) ) );
			if( m_textField != 0 )
			{
				m_textField->SetAlignment( m_textAlignment );

				textEntity->AttachComponent( m_textField );
			}
			m_entity->AttachChild( textEntity );
		}

		m_lineHeight = font->GetCharacterHeight();
		m_textAreaHeight = m_dimensions.y;

		Kiwi::Entity* bgSpriteEntity = scene->CreateEntity( m_objectName + L"/sprite" );
		if( bgSpriteEntity != 0 )
		{
			m_backgroundSprite = new Kiwi::UISprite(L"sprite" );

			Kiwi::Transform* spriteTransform = bgSpriteEntity->FindComponent<Kiwi::Transform>();
			if( spriteTransform != 0 )
			{
				spriteTransform->SetScale( Kiwi::Vector3d( m_dimensions.x, m_dimensions.y, 1.0 ) );
			}
			bgSpriteEntity->AttachComponent( m_backgroundSprite );
			m_backgroundSprite->SetColor( Kiwi::Color( 0.0, 0.0, 0.0, 1.0 ) );

			m_entity->AttachChild( bgSpriteEntity );
		}

		m_visibleLineCount = (unsigned int)((m_dimensions.y - m_padding.y) - m_padding.w) / m_lineHeight;

		if( m_textboxType == TEXTBOX_TYPE::MULTILINE_SCROLLABLE )
		{

			Kiwi::Entity* scrollBarEntity = scene->CreateEntity( m_objectName + L"/scrollbar" );
			if( scrollBarEntity != 0 )
			{
				Kiwi::Vector2d scrollDim( 16.0, m_dimensions.y );
				Kiwi::Vector3d scrollPos( (int)((m_dimensions.x / 2.0) - (scrollDim.x / 2.0)), 1.0, 1.0 );
				m_scrollBar = new Kiwi::UIScrollBar( m_objectName + L"scrollbar", scrollDim );
				if( m_scrollBar )
				{
					scrollBarEntity->AttachComponent( m_scrollBar );
					m_scrollBar->AddScrollBarEventListener( this );
					m_entity->AttachChild( scrollBarEntity );
				}
			}

		} else
		{
			m_scrollBar = 0;

			if( m_textboxType != TEXTBOX_TYPE::MULTILINE )
			{
				m_visibleLineCount = 1;
			}
		}

		m_entity->SetEntityType( Kiwi::Entity::ENTITY_2D );

	}

	void UITextBox::_UpdateVisibleText()
	{

		if( m_entity != 0 )
		{

			//Kiwi::Scene* scene = m_entity->GetScene();
			//Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
			//assert( transform != 0 && scene != 0 );

			//m_suppressNew = true; //prevent any errors this operation may generate from being added to avoid an infinite loop

			//int topLineY = (transform->GetPosition().y + (m_textAreaHeight / 2)) - (m_lineHeight / 2); //calculate the y coordinate of the top-most line
			//int bottomLineY = (transform->GetPosition().y - (m_textAreaHeight / 2)) + (m_lineHeight / 2); //same for the bottom-most line

			//m_visibleLineCount = m_textAreaHeight / m_lineHeight;

			////update the visible text meshes if needed
			//if( m_visibleText.size() != m_visibleLineCount )
			//{
			//	int lineCounter = m_visibleText.size();
			//	while( m_visibleText.size() < m_visibleLineCount )
			//	{
			//		Kiwi::Text* newLine = new Kiwi::Text( m_objectName + L"/line" + Kiwi::ToWString( lineCounter ), *scene, m_textFont, 1 );
			//		newLine->SetShader( L"Default2DShader" );
			//		m_visibleText.push_back( newLine );
			//		m_entity->AttachChild( newLine );
			//		lineCounter++;
			//	}

			//	while( m_visibleText.size() > m_visibleLineCount && m_visibleText.size() > 0 )
			//	{
			//		m_visibleText[0]->Shutdown();
			//		m_visibleText.erase( m_visibleText.begin() );
			//	}
			//}

			//unsigned int i = 0;
			//if( m_text.size() > m_visibleLineCount )
			//{
			//	i = m_scrollBar->GetCurrentLine() - 1;
			//}
			//for( unsigned int a = 0; i < m_text.size() && a < m_visibleLineCount; i++, a++ )
			//{
			//	//calculate the position of the line, taking into account the alignment of the text
			//	Kiwi::Vector3d linePos = transform->GetPosition();
			//	if( m_textAlignment == Kiwi::Font::ALIGN_LEFT )
			//	{
			//		linePos.x -= transform->GetScale().x / 2.0;
			//		linePos.x += m_padding.x;

			//	} else if( m_textAlignment == Kiwi::Font::ALIGN_RIGHT )
			//	{
			//		linePos.x += transform->GetScale().x / 2.0;
			//		linePos.x -= m_padding.z;
			//	}
			//	linePos.x = (int)linePos.x;
			//	linePos.y = (int)linePos.y;
			//	linePos.z = transform->GetPosition().z + 1.0; //increase the z position so the text appears above the background

			//	if( m_text[i].lineCount > 1 )
			//	{
			//		m_visibleText[a]->SetMaxCharacters( m_text[i].text.size() + 8 );
			//		m_visibleText[a]->SetText( m_text[i].text + L" (" + Kiwi::ToWString( m_text[i].lineCount ) + L")", m_text[i].alignment );

			//	} else
			//	{
			//		m_visibleText[a]->SetMaxCharacters( m_text[i].text.size() );
			//		m_visibleText[a]->SetText( m_text[i].text, m_text[i].alignment );
			//	}
			//	m_visibleText[a]->SetColor( m_text[i].lineColor );

			//	if( m_text.size() > m_visibleLineCount )
			//	{
			//		linePos.y = topLineY - (a * m_lineHeight);
			//		m_visibleText[a]->FindComponent<Kiwi::Transform>()->SetPosition( linePos );

			//	} else
			//	{
			//		linePos.y = topLineY - (a * m_lineHeight);
			//		m_visibleText[a]->FindComponent<Kiwi::Transform>()->SetPosition( linePos );
			//	}
			//}

			//m_suppressNew = false;
		}

	}

	void UITextBox::_OnFixedUpdate()
	{

		if( m_entity != 0 )
		{
			std::lock_guard<std::mutex> guard( m_textBoxMutex );

			if( m_tempText.size() > 0 )
			{
				//lines have been added
				while( m_tempText.size() > 0 )
				{
					//first go through and combine all duplicate line into a single line
					TextLine currentLine = m_tempText[0];
					m_tempText.erase( m_tempText.begin() );
					for( auto itr = m_tempText.begin(); itr != m_tempText.end(); )
					{
						if( (*itr).text.compare( currentLine.text ) == 0 )
						{//this line matches the current line, increase the line count
							currentLine.lineCount++;
							itr = m_tempText.erase( itr );

						} else
						{
							itr++;
						}
					}

					if( currentLine.text.size() > 0 )
					{//if the current line isnt empty, add it
						currentLine.lineNumber = (m_text.size() == 0) ? 0 : m_text[m_text.size() - 1].lineNumber + 1;
						m_text.push_back( currentLine );
						m_scrollBar->SetLineCount( max( (int)m_text.size() - m_visibleLineCount, 1 ) );
					}
				}

				this->_UpdateVisibleText();

			} else if( m_updateText == true )
			{
				this->_UpdateVisibleText();
			}

			m_updateText = false;
		}

	}

	void UITextBox::AddLine( std::wstring text )
	{

		this->AddLine( text, Kiwi::Color( 1.0, 1.0, 1.0, 1.0 ) );

	}

	void UITextBox::AddLine( std::wstring textLine, const Kiwi::Color& color )
	{

		if( m_suppressNew )
		{
			return;
		}

		std::lock_guard<std::mutex> guard( m_textBoxMutex );

		static std::wstring lastLine;

		if( textLine.compare(lastLine) == 0 )
		{
			if( m_tempText.size() > 0 && m_tempText[m_tempText.size() - 1].lineCount < 1000 )
			{
				//trying to add the same message twice in a row, just update the line count
				m_tempText[m_tempText.size() - 1].lineCount++;
				m_updateText = true;
				return;

			} else if( m_tempText.size() == 0 && m_text[m_text.size() - 1].lineCount < 1000 )
			{
				m_text[m_text.size() - 1].lineCount++;
				m_updateText = true;
				return;
			}
		}

		TextLine newLine;
		newLine.text = textLine;
		newLine.lineColor = color;
		newLine.lineCount = 1;
		newLine.lineNumber = m_tempText.size() + 1;
		newLine.alignment = m_textAlignment;

		m_tempText.push_back( newLine );

		lastLine = textLine;
		m_updateText = true;

	}

	void UITextBox::SetPadding( const Kiwi::Vector4& padding )
	{

		if( m_entity != 0 )
		{
			Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
			assert( transform != 0 );
			std::lock_guard<std::mutex> guard( m_textBoxMutex );

			m_padding = padding;
			m_textAreaHeight = transform->GetScale().y - (padding.y + padding.w);

			m_updateText = true;
		}

	}

	void UITextBox::SetTexture( std::wstring textureName)
	{
		std::lock_guard<std::mutex> guard( m_textBoxMutex );

		if( m_backgroundSprite )
		{
			m_backgroundSprite->SetTexture( textureName );
		}

	}

	void UITextBox::OnScrollEvent( const Kiwi::UIScrollBarEvent& evt )
	{

		m_updateText = true;

	}

	void UITextBox::OnTranslate( const Kiwi::TransformEvent& evt )
	{

		//if( m_entity != 0 )
		//{
		//	Kiwi::Transform* transform = m_entity->FindComponent<Kiwi::Transform>();
		//	assert( transform != 0 );

		//	if( evt.GetSource() == transform ) //only respond to transform events from the textbox's transforms, not from any children
		//	{
		//		if( m_backgroundSprite && m_backgroundSprite->GetEntity() )
		//		{
		//			Kiwi::Entity* bgEntity = m_backgroundSprite->GetEntity();
		//			Kiwi::Vector3d bgPos = bgEntity->FindComponent<Kiwi::Transform>()->GetPosition();
		//			bgPos.z = transform->GetPosition().z;
		//			bgEntity->FindComponent<Kiwi::Transform>()->SetPosition( bgPos );
		//		}

		//		if( m_scrollBar && m_scrollBar->GetEntity() )
		//		{
		//			Kiwi::Entity* scrollBarEntity = m_scrollBar->GetEntity();
		//			Kiwi::Vector3d scrollPos = scrollBarEntity->FindComponent<Kiwi::Transform>()->GetPosition();
		//			scrollPos.z = transform->GetPosition().z + 0.1;
		//			scrollBarEntity->FindComponent<Kiwi::Transform>()->SetPosition( scrollPos );
		//		}

		//		for( unsigned int i = 0; i < m_visibleText.size(); i++ )
		//		{
		//			Kiwi::Vector3d textPos = m_visibleText[i]->FindComponent<Kiwi::Transform>()->GetPosition();
		//			textPos.z = transform->GetPosition().z + 1.0;
		//			m_visibleText[i]->FindComponent<Kiwi::Transform>()->SetPosition( textPos );
		//		}
		//	}
		//}

	}

}