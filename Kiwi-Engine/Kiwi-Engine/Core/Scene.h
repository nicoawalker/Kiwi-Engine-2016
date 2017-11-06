#ifndef _KIWI_SCENE_H_
#define _KIWI_SCENE_H_

#include "EntityManager.h"
#include "IThreadSafe.h"
#include "Assert.h"
#include "AssetManager.h"
#include "Vector4.h"
#include "IFrameEventListener.h"
#include "ComponentContainer.h"

#include "Events\IGlobalEventBroadcaster.h"

#include "../Graphics/RenderTargetManager.h"
#include "../Graphics/IShader.h"

#include <string>
#include <vector>
#include <mutex>

namespace Kiwi
{

	class Renderer;
	class Entity;
	class SceneLoader;
	class IAsset;
	class Camera;
	class RenderTarget;
	class SceneManager;
	class ITerrain;
	class PhysicsSystem;
	class RenderWindow;
	class Console;

	class Scene :
		public Kiwi::IThreadSafe,
		public Kiwi::IGlobalEventBroadcaster
	{
	friend class SceneManager;

	protected:

		Kiwi::EngineRoot* m_engine;

		std::wstring m_name;

		Kiwi::Renderer* m_renderer;

		Kiwi::PhysicsSystem* m_physicsSystem;

		Kiwi::RenderTargetManager m_renderTargetManager;
		Kiwi::EntityManager m_entityManager;
		Kiwi::AssetManager m_assetManager;

		Kiwi::SceneLoader* m_sceneLoader;

		//stores the player's entity for easy retrieval
		Kiwi::Entity* m_playerEntity;

		//contains all of the scene's shaders, sorted by name
		Kiwi::ComponentContainer<std::wstring, Kiwi::IShader> m_shaderContainer;

		Kiwi::Console* m_console;

		Kiwi::ITerrain* m_terrain;

		Kiwi::Vector4 m_diffuseLight;
		Kiwi::Vector4 m_diffuseDirection;
		Kiwi::Vector4 m_ambientLight;

		std::recursive_mutex m_sceneMutex;

		bool m_shutdown;
		bool m_isActive;

	protected:

		/*renders the back buffer*/
		void _Render();

		void _AttachConsole( Kiwi::Console* console );

		virtual void _OnUpdate() {}
		virtual void _OnFixedUpdate() {}
		virtual void _OnShutdown() {}
		virtual void _OnPreRender() {}
		virtual void _OnPostRender() {}

	public:

		Scene( Kiwi::EngineRoot* engine, std::wstring name, Kiwi::Renderer* renderer);
		virtual ~Scene();

		void Shutdown();

		void Update();
		void FixedUpdate();

		virtual void Load() {}

		virtual void AddEntity( Kiwi::Entity* entity );
		virtual void AddAsset( Kiwi::IAsset* asset );
		virtual void AddShader( Kiwi::IShader* shader );

		Kiwi::Entity* CreateEntity( std::wstring name );

		Kiwi::Camera* CreateCamera( std::wstring name );
		Kiwi::Camera* CreateCamera( std::wstring name, float FOV, float aspectRatio, float nearClip, float farClip );

		bool CastRay( const Kiwi::Vector3d& origin, const Kiwi::Vector3d& direction, double maxDepthFromOrigin, std::vector<Kiwi::Entity*>& hits );

		void SetPlayerEntity( Kiwi::Entity* playerEntity );

		void SetTerrain( Kiwi::ITerrain* terrain ) { m_terrain = terrain; }

		void SetActive( bool isActive ) { m_isActive = isActive; }

		std::wstring GetName()const { return m_name; }

		Kiwi::EngineRoot* GetEngine()const { return m_engine; }
		Kiwi::Renderer* GetRenderer()const { return m_renderer; }
		Kiwi::RenderWindow* GetRenderWindow();

		const Kiwi::Vector4& GetDiffuseLight()const { return m_diffuseLight; }
		const Kiwi::Vector4& GetDiffuseLightDirection()const { return m_diffuseDirection; }
		const Kiwi::Vector4& GetAmbientLight()const { return m_ambientLight; }

		Kiwi::Entity* GetPlayerEntity()const { return m_playerEntity; }

		Kiwi::PhysicsSystem* GetPhysicsSystem()const { return m_physicsSystem; }

		bool IsShutdown()const { return m_shutdown; }
		bool IsActive()const { return m_isActive; }

		virtual Kiwi::Entity* FindEntityWithName( std::wstring name );
		virtual std::vector<Kiwi::Entity*> FindEntitiesWithTag( std::wstring tag );

		Kiwi::RenderTarget* FindRenderTargetWithName( std::wstring name );

		template<class TerrainType>
		TerrainType GetTerrain()const
		{ 
			return dynamic_cast<TerrainType>(m_terrain);
		}

		template<class AssetType>
		AssetType* FindAsset( std::wstring name )
		{
			return dynamic_cast<AssetType*>(m_assetManager.FindAsset( name ));
		}

		template<>
		Kiwi::IShader* FindAsset<Kiwi::IShader>( std::wstring name )
		{
			return m_shaderContainer.Find( name );
		}

	};

};

#endif