#include "Scene.h"
#include "Utilities.h"
#include "Exception.h"
#include "Assert.h"
#include "EntityManager.h"
#include "AssetManager.h"
#include "SceneLoader.h"
#include "EngineRoot.h"
#include "ITerrain.h"
#include "Console.h"

#include "../Graphics/Renderer.h"
#include "../Graphics/IShader.h"
#include "../Graphics/Mesh.h"
#include "../Graphics/InstancedMesh.h"
#include "../Graphics/Renderable.h"
#include "../Graphics/RenderTargetManager.h"
#include "../Graphics/Camera.h"
#include "../Graphics/D3D11Interface.h"
#include "../Graphics/DepthStencil.h"
#include "../Graphics/RenderQueue.h"

namespace Kiwi
{

	Scene::Scene( Kiwi::EngineRoot* engine, std::wstring name, Kiwi::Renderer* renderer ) :
		m_entityManager( *this )
	{

		assert( engine != 0 );

		if( renderer == 0 )
		{
			throw Kiwi::Exception( L"Scene", L"Invalid renderer" );
		}

		m_sceneLoader = 0;
		m_playerEntity = 0;
		m_terrain = 0;
		m_renderer = 0;
		m_engine = engine;
		m_physicsSystem = m_engine->GetPhysicsSystem();
		m_shutdown = false;
		m_isActive = false;

		try
		{
			m_name = name;

			m_renderer = renderer;

			m_diffuseLight = Kiwi::Vector4( 1.0f, 1.0f, 1.0f, 1.0f );
			m_diffuseDirection = Kiwi::Vector4( 1.0f, -1.0f, 0.0f, 0.0f );
			m_ambientLight = Kiwi::Vector4( 0.4f, 0.4f, 0.4f, 1.0f );

			m_sceneLoader = new Kiwi::SceneLoader( this );

			//add the back buffer render target to the render target manager
			Kiwi::RenderTarget* backBuffer = m_renderer->GetBackBuffer();
			m_renderTargetManager.AddRenderTarget( backBuffer );

		} catch( const Kiwi::Exception& e )
		{
			throw e;
		}

	}

	Scene::~Scene()
	{

		m_renderer = 0;

		m_shaderContainer.DestroyAll();

		SAFE_DELETE( m_sceneLoader );
		SAFE_DELETE( m_terrain );

	}

	void Scene::Update()
	{

		if( m_sceneLoader != 0 )
		{
			m_sceneLoader->OnUpdate();
		}

		if( m_isActive )
		{
			//if( m_playerEntity ) m_playerEntity->Update();
			m_entityManager.Update();
		}

		this->_OnUpdate();

	}

	void Scene::FixedUpdate()
	{

		if( m_isActive )
		{
			//if( m_playerEntity ) m_playerEntity->FixedUpdate();

			m_entityManager.FixedUpdate();
		}

		this->_OnFixedUpdate();

	}

	void Scene::_Render()
	{

		if( m_isActive )
		{
			std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

			assert( m_renderer != 0 );
			assert( m_engine != 0 );

			this->_OnPreRender();

			m_entityManager.ClearRenderQueue();
			Kiwi::RenderQueue* renderQueue = m_entityManager.GenerateRenderQueue();

			//get a list of all render targets, sorted by lowest to highest priority (back to front), and render them all
			Kiwi::RenderTargetList rtList = m_renderTargetManager.GetRenderTargetList();
			for( auto itr = rtList.targets.begin(); itr != rtList.targets.end(); itr++ )
			{
				m_renderer->SetRenderTarget( *itr );
				m_renderer->ClearRenderTarget();

				m_renderer->Render( renderQueue );
			}

			m_renderer->Present();

			this->_OnPostRender();
		}

	}

	void Scene::_AttachConsole( Kiwi::Console* console )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		if( m_console )
		{
			if( console == m_console )
			{
				return;
			}
		}

		m_console = console;

		if( m_console && this->GetRenderWindow() )
		{
			Kiwi::Vector2d dim( this->GetRenderWindow()->GetClientSize().x - 40, (int)((double)this->GetRenderWindow()->GetClientSize().y / 2.16) );
			Kiwi::Vector3d pos( 0.0, ((double)this->GetRenderWindow()->GetClientSize().y / 2.0) - ((dim.y / 2) + 20.0), 10000.0 );
			m_console->InitializeInterface( *this, L"Lato_20pt", dim, pos );
		}

	}

	void Scene::Shutdown()
	{

		this->_OnShutdown();

		m_entityManager.ShutdownAll();

		m_shutdown = true;

	}

	void Scene::AddEntity( Kiwi::Entity* entity )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		if( entity == 0 ) return;

		try
		{
			m_entityManager.AddEntity( entity );

			if( m_physicsSystem != 0 )
			{
				Kiwi::Transform* entityTransform = entity->FindComponent<Kiwi::Transform>();
				if( entityTransform )
				{
					m_physicsSystem->AddRigidbody( entityTransform->FindComponent<Kiwi::Rigidbody>() );
				}
			}

		} catch( ... )
		{
			throw;
		}

	}

	void Scene::AddAsset( Kiwi::IAsset* asset )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );
		try
		{
			m_assetManager.AddAsset( asset );

		} catch( ... )
		{
			throw;
		}

	}

	void Scene::AddShader( Kiwi::IShader* shader )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );
		if( shader )
		{
			m_shaderContainer.Add( shader->GetName(), shader );
		}

	}

	Kiwi::Entity* Scene::CreateEntity( std::wstring name )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		return m_entityManager.CreateEntity( name );

	}

	Kiwi::Camera* Scene::CreateCamera( std::wstring name )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		Kiwi::Camera* camera = new Kiwi::Camera( name, *this );
		m_entityManager.AddEntity( camera );

		return camera;

	}

	Kiwi::Camera* Scene::CreateCamera( std::wstring name, float FOV, float aspectRatio, float nearClip, float farClip )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		Kiwi::Camera* camera = new Kiwi::Camera( name, *this, FOV, aspectRatio, nearClip, farClip );
		m_entityManager.AddEntity( camera );

		return camera;

	}

	bool Scene::CastRay( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits )
	{

		m_entityManager.Raytrace( origin, direction, maxDepthFromOrigin, hits );

		return (hits.size() > 0) ? true : false;

	}

	void Scene::SetPlayerEntity( Kiwi::Entity* playerEntity )
	{

		std::lock_guard<std::recursive_mutex> guard( m_sceneMutex );

		if( playerEntity == 0 ) return;

		if( m_playerEntity != 0 )
		{
			m_playerEntity->Shutdown();
			m_playerEntity = 0;
		}

		m_playerEntity = playerEntity;
		this->AddEntity( playerEntity );

	}

	Kiwi::RenderWindow* Scene::GetRenderWindow()
	{
		
		if( m_renderer == 0 )
		{
			return 0;
		}

		return m_renderer->GetRenderWindow();

	}

	Kiwi::Entity* Scene::FindEntityWithName( std::wstring name )
	{

		return m_entityManager.FindWithName( name );

	}

	std::vector<Kiwi::Entity*> Scene::FindEntitiesWithTag( std::wstring tag )
	{

		return m_entityManager.FindAllWithTag( tag );

	}

	Kiwi::RenderTarget* Scene::FindRenderTargetWithName( std::wstring name )
	{

		return m_renderTargetManager.FindRenderTargetWithName( name );

	}

};