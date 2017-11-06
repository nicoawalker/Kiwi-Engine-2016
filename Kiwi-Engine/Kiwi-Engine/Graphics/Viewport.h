#ifndef _KIWI_VIEWPORT_H_
#define _KIWI_VIEWPORT_H_

#include "../Core/Vector2.h"

#include "DirectX.h"

#include <string>
#include <vector>

namespace Kiwi
{

	class Camera;

	class Viewport
	{
	protected:

		Kiwi::Camera* m_camera;

		Kiwi::Vector2 m_position;
		Kiwi::Vector2 m_dimensions;

		std::vector<std::wstring> m_renderGroups;

		bool m_useDefaultRenderGroup;

		float m_minDepth;
		float m_maxDepth;

		std::wstring m_name;

		D3D11_VIEWPORT m_viewport;

	public:

		Viewport( std::wstring name, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, float minDepth, float maxDepth );
		Viewport( std::wstring name, Kiwi::Camera* camera, const Kiwi::Vector2& position, const Kiwi::Vector2& dimensions, float minDepth, float maxDepth );
		virtual ~Viewport() {}

		void AttachCamera(Kiwi::Camera* camera);

		void AddRenderGroup( std::wstring renderGroup );

		/*tells the viewport to use the default render groups*/
		void UseDefaultRenderGroup( bool useDefaults ) { m_useDefaultRenderGroup = useDefaults; }

		Kiwi::Camera* GetCamera()const { return m_camera; }

		float GetMinDepth()const { return m_minDepth; }
		float GetMaxDepth()const { return m_maxDepth; }

		bool UsingDefaultRenderGroup()const { return m_useDefaultRenderGroup; }

		std::vector<std::wstring>& GetRenderGroupList() { return m_renderGroups; }

		const Kiwi::Vector2& GetDimensions()const { return m_dimensions; }
		const Kiwi::Vector2& GetPosition()const { return m_position; }

		D3D11_VIEWPORT& GetD3DViewport() { return m_viewport; }

	};
};

#endif