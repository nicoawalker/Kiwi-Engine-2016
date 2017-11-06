#ifndef _KIWI_RENDERABLEMANAGER_H_
#define _KIWI_RENDERABLEMANAGER_H_

#include <unordered_map>
#include <vector>

namespace Kiwi
{

	class Renderable;
	class Scene;
	class Entity;
	class RenderQueue;

	class RenderableManager
	{
	public:

		//struct RenderableListPair
		//{
		//	std::vector<Kiwi::Renderable*> opaque_renderables;
		//	std::vector<Kiwi::Renderable*> transparent_renderables;
		//};

		//struct RenderableMeshMap
		//{
		//	//stores the name of a mesh, and a vector of renderable pairs that store the opaque and transparent objects using that mesh
		//	std::unordered_map<std::wstring, RenderableListPair> meshMap;
		//};

		//struct RenderableShaderMap
		//{
		//	//stores the name of a shader, and a RenderableMeshMap of renderables that use the shader
		//	std::unordered_map<std::wstring, RenderableMeshMap> shaderMap;

		//	//stores the shaders for the 2d renderables using this render target
		//	std::unordered_map<std::wstring, RenderableMeshMap> shaderMap2D;
		//};

		//struct RenderableMap
		//{
		//	//stores the name of a render target and a RenderableShaderMap of renderables that should be drawn onto that render target
		//	std::unordered_map<std::wstring, RenderableShaderMap> rtMap;
		//};

		//struct RenderTargetQueue
		//{
		//	std::wstring renderTarget;

		//	//opaque objects are sorted by shader and then by mesh
		//	std::vector<std::vector<std::vector<Kiwi::Renderable*>>> opaque3D;
		//	std::vector<std::vector<std::vector<Kiwi::Renderable*>>> opaque2D;

		//	//transparent objects are sorted by mesh and then by shader
		//	std::vector<std::vector<std::vector<Kiwi::Renderable*>>> transparent3D;
		//	std::vector<std::vector<std::vector<Kiwi::Renderable*>>> transparent2D;
		//};

		//struct RenderQueue
		//{
		//	//a vector of RenderTargetQueues, sorted by draw order
		//	std::vector<RenderTargetQueue> renderTargets;
		//};

	protected:

		Kiwi::Scene* m_parentScene;

		std::unordered_map<std::wstring, Kiwi::Renderable*> m_renderables;

		//RenderableMap m_renderables;

		//std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::unordered_map<std::wstring, std::vector<Kiwi::Renderable*>>>> m_renderableMap;



		//RenderQueue m_renderQueue;

	public:

		RenderableManager( Kiwi::Scene& scene );
		~RenderableManager();

		void FixedUpdate();

		void AddRenderable( Kiwi::Renderable* renderable );

		/*generates a render queue from all active renderables*/
		Kiwi::RenderQueue* GenerateRenderQueue();

		/*given an entity, adds all renderables belonging to the entity and from any children the entity may have*/
		void AddFromEntity( Kiwi::Entity* entity );

		void ShutdownWithName( std::wstring name );

		void ShutdownAll();

		Kiwi::Renderable* FindWithName( std::wstring name );
		//std::vector<Kiwi::Renderable*> FindAllWithTag( std::wstring tag );

		//RenderableManager::RenderableMap* GetRenderables() { return &m_renderables; }

	};
}

#endif