#include "IBuffer.h"
#include "Renderer.h"
#include "DirectX.h"

#include "../Core/Utilities.h"
#include "../Core/Exception.h"

namespace Kiwi
{

	IBuffer::IBuffer(Kiwi::Renderer& renderer, long elementCapacity)
	{

		m_elementCapacity = elementCapacity;
		m_renderer = &renderer;

		m_buffer = 0;

	}

	IBuffer::~IBuffer()
	{

		SAFE_RELEASE(m_buffer);

	}

};