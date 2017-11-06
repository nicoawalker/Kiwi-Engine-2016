#include "Utilities.h"
#include "Exception.h"
#include "SceneManager.h"
#include "Scene.h"
#include "EngineRoot.h"

//#include "../Graphics/GraphicsCore.h"
#include "../Graphics/Renderer.h"
#include "../Graphics/RenderWindow.h"
#include "../Graphics/RenderTarget.h"

namespace Kiwi
{

	SceneManager::SceneManager( Kiwi::EngineRoot* engine )
	{

		m_engine = engine;
		if( m_engine == 0 )
		{ 
			throw Kiwi::Exception( L"SceneManager", L"Invalid engine pointer" ); 
		}

		m_engine->AddListener( this );

	}

	SceneManager::~SceneManager()
	{
		
		if( m_engine ) m_engine->RemoveListener( this );

		auto it = m_scenes.begin();
		for(; it != m_scenes.end(); )
		{
			SAFE_DELETE(it->second);
			it = m_scenes.erase(it);
		}

	}

	void SceneManager::Shutdown()
	{

		auto it = m_scenes.begin();
		for( ; it != m_scenes.end(); it++ )
		{
			it->second->Shutdown();
		}

	}

	void SceneManager::Render()
	{

		auto it = m_scenes.begin();
		for( ; it != m_scenes.end(); it++ )
		{
			it->second->_Render();
		}

	}

	void SceneManager::AddScene( Kiwi::Scene* scene )
	{

		if( scene )
		{

			Kiwi::Scene* existingScene = this->FindSceneWithName( scene->GetName() );

			if( existingScene )
			{
				this->DestroyScene( scene->GetName() );
			}

			m_scenes[scene->GetName()] = scene;

		}

	}

	Kiwi::Scene* SceneManager::FindSceneWithName( std::wstring name )
	{

		auto sceneItr = m_scenes.find( name );
		if( sceneItr != m_scenes.end() )
		{
			return sceneItr->second;
		}

		return 0;

	}

	void SceneManager::DestroyScene( std::wstring name )
	{

		auto itr = m_scenes.find( name );
		if( itr != m_scenes.end() )
		{
			itr->second->Shutdown();
			SAFE_DELETE( itr->second );
			itr = m_scenes.erase( itr );
		}

	}

	void SceneManager::OnUpdate()
	{

		auto it = m_scenes.begin();
		for( ; it != m_scenes.end(); it++ )
		{
			it->second->Update();
		}

	}

	void SceneManager::OnFixedUpdate()
	{

		auto it = m_scenes.begin();
		for( ; it != m_scenes.end(); it++ )
		{
			it->second->FixedUpdate();
		}

		auto sceneItr = m_scenes.begin();
		for( ; sceneItr != m_scenes.end(); )
		{
			if( sceneItr->second == 0 || sceneItr->second->IsShutdown() == true )
			{
				SAFE_DELETE( sceneItr->second );
				sceneItr = m_scenes.erase( sceneItr );
				continue;
			}

			sceneItr++;
		}

	}

};