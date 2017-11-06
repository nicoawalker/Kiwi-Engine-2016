#ifndef _KIWI_CONSOLE_H_
#define _KIWI_CONSOLE_H_

#include "Logger.h"

#include "Vector3d.h"
#include "Vector2d.h"

#include "../Graphics/Color.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class UITextBox;
	class EngineRoot;
	class Scene;
	class Texture;

	class Console:
		public Kiwi::Logger
	{
	public:

		struct ConsoleMessage
		{
			Kiwi::Color textColor;
			std::wstring message;
		};

		Kiwi::EngineRoot* m_engine;
		Kiwi::Scene* m_scene;

		std::vector<ConsoleMessage> m_messages;

		Kiwi::UITextBox* m_textBox;

		Kiwi::Texture* m_textBoxBackground;
		Kiwi::Texture* m_textBoxScrollBar;
		Kiwi::Texture* m_textBoxSlider;
		Kiwi::Texture* m_textBoxArrows;

		Kiwi::Color m_textBoxScrollBarColor;
		Kiwi::Color m_textBoxScrollBarSliderColor;
		Kiwi::Color m_textBoxScrollBarArrowColor;

		Kiwi::Color m_textColor;

		std::mutex m_consoleMutex;

		bool m_hidden;
		bool m_debugEnabled;

	protected:

		void _Hide();
		void _Show();
		void _Print( std::wstring message, const Kiwi::Color& color );

	public:

		Console( Kiwi::EngineRoot& engine, std::wstring logFile );
		virtual ~Console();

		virtual void Update();
		virtual void FixedUpdate() {}

		virtual void InitializeInterface( Kiwi::Scene& scene, std::wstring textFont, const Kiwi::Vector2d& dimensions, const Kiwi::Vector3d& position );

		virtual void Shutdown();

		virtual void PrintDebug( std::wstring message );
		virtual void Print( std::wstring message );
		virtual void Print( std::wstring message, const Kiwi::Color& color );

		virtual void Write( std::wstring message );

		void EnableDebug( bool debugEnabled ) { m_debugEnabled = debugEnabled; }

	};

}

#endif