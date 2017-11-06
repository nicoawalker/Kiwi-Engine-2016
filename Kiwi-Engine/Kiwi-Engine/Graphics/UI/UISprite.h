#ifndef _KIWI_UISPRITE_H_
#define _KIWI_UISPRITE_H_

#include "../Color.h"

#include "../../Core/Component.h"

#include <string>

namespace Kiwi
{

	class Texture;
	//class SpriteAnimationSet;

	class UISprite :
		public Kiwi::Component
	{
	protected:

		Kiwi::Texture* m_spriteTexture;
		std::wstring m_textureName;

	protected:

		virtual void _OnAttached();

	public:

		UISprite( std::wstring name, std::wstring textureName = L"" );
		~UISprite();

		void SetTexture( std::wstring textureName );
		void SetColor( const Kiwi::Color& color );

		Kiwi::Texture* GetTexture()const { return m_spriteTexture; }
		std::wstring GetTextureName()const { return m_textureName; }

	};
}

#endif