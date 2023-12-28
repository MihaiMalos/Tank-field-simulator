#include "VertexBufferLayout.h"

unsigned int VertexBufferElement::GetSizeOfType(unsigned int type)
{
	switch (type)
	{
	case GL_FLOAT: return sizeof(GLfloat);
	case GL_UNSIGNED_INT: return sizeof(GLuint);
	case GL_UNSIGNED_BYTE: return sizeof(GLbyte);
	}
	return 0;
}

VertexBufferLayout::VertexBufferLayout() :
	m_Stride(0)
{

}

void VertexBufferLayout::AddFloat(unsigned int count)
{
	Push(GL_FLOAT, count, GL_FALSE);
}

void VertexBufferLayout::AddUnsignedInt(unsigned int count)
{
	Push(GL_UNSIGNED_INT, count, GL_FALSE);
}

void VertexBufferLayout::AddUnsignedByte(unsigned int count)
{
	Push(GL_UNSIGNED_BYTE, count, GL_TRUE);
}

const std::vector<VertexBufferElement> VertexBufferLayout::GetElements() const
{
	return m_Elements;
}

unsigned int VertexBufferLayout::GetStride() const
{
	return m_Stride;
}

void VertexBufferLayout::Push(unsigned int type, unsigned int count, unsigned char normalized)
{
	struct VertexBufferElement vbe = { type, count, normalized };
	m_Elements.push_back(vbe);
	m_Stride += count * VertexBufferElement::GetSizeOfType(type);
}
