#ifndef _KIWI_SCENEMANAGER_H_
#define _KIWI_SCENEMANAGER_H_

#include "IFrameEventListener.h"

#include <string>
#include <unordered_map>

namespace Kiwi
{

	class Scene;
	class Renderer;
	class RenderTarget;
	class EngineRoot;
	class GraphicsCore;

	class SceneManager:
		public Kiwi::IFrameEventListener
	{
	protected:

		Kiwi::EngineRoot* m_engine;

		std::unordered_map<std::wstring, Kiwi::RenderTarget*> m_renderTargets;

		std::unordered_map<std::wstring, Kiwi::Scene*> m_scenes;

	protected:

		virtual void OnUpdate();
		virtual void OnFixedUpdate();

	public:

		SceneManager( Kiwi::EngineRoot* engine );
		virtual ~SceneManager();

		void Shutdown();

		void Render();

		void AddScene( Kiwi::Scene* scene );

		Kiwi::Scene* FindSceneWithName( std::wstring name );

		void DestroyScene( std::wstring name );

	};
};

#endif