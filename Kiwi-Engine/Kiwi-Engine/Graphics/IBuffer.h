#ifndef _KIWI_IBUFFER_H_
#define _KIWI_IBUFFER_H_

#include <string>

struct ID3D11Buffer;
struct ID3D11Device;
struct ID3D11DeviceContext;

namespace Kiwi
{

	class Renderer;

	class IBuffer
	{
	protected:

		Kiwi::Renderer* m_renderer;

		ID3D11Buffer* m_buffer;

		long m_elementCount;
		//stores the maximum number of elements that can fit in the buffer
		long m_elementCapacity;
		//stores the size, in bytes of the buffer
		long m_bufferSize;

	public:

		IBuffer( Kiwi::Renderer& renderer, long elementCapacity );
		virtual ~IBuffer() = 0;

		virtual ID3D11Buffer* GetD3DBuffer()const { return m_buffer; }

		//returns the number of elements the buffer can hold
		virtual long GetCapacity()const { return m_elementCapacity; }
		//returns the current number of elements in the buffer
		virtual long GetElementCount()const { return m_elementCount;  }
		//returns the size of the buffer in bytes
		virtual long GetSize()const { return m_bufferSize;  }

	};
};

#endif