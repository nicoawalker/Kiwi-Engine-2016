#ifndef _KIWI_TEXT_H_
#define _KIWI_TEXT_H_

#include "Font.h"
#include "Color.h"

#include "..\Core\Entity.h"
#include "../Core/Component.h"

namespace Kiwi
{

	class Text:
		public Kiwi::Component
	{
	protected:

		//the font used for rendering text
		Kiwi::Font* m_font;
		Kiwi::Mesh* m_textMesh;

		Kiwi::Vector2d m_dimensions;
		Kiwi::Vector2d m_maxDimensions;

		Font::ALIGNMENT m_textAlignment;

		Kiwi::Color m_textColor;

		//maximum number of characters
		int m_maxCharacters;

		std::wstring m_text;
		std::wstring m_fontName;
		std::wstring m_shaderName;

	protected:

		void _OnAttached();

		void _GenerateTextMesh( std::wstring text );

	public:

		Text( std::wstring name, std::wstring fontName, const Kiwi::Vector2d& maxDimensions );
		virtual ~Text();

		virtual void SetText( std::wstring text );

		void SetAlignment( Font::ALIGNMENT alignment );

		virtual void SetMaxCharacters( unsigned int maxChars ) { m_maxCharacters = maxChars; }

		virtual void SetColor( const Kiwi::Color& color );

		void SetShader( std::wstring shaderName );

		virtual Kiwi::Vector2d GetDimensions()const { return m_dimensions; }

		virtual std::wstring GetText()const { return m_text; }

		virtual unsigned int GetMaxCharacters()const { return m_maxCharacters; }

		virtual Kiwi::Color GetColor()const { return m_textColor; }

		double GetCharacterHeight()const;
	};
}

#endif