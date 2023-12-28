#pragma once

#include <vector>
#include <GL/glew.h>

struct VertexBufferElement
{
	unsigned int type;
	unsigned int count;
	unsigned char normalized;

	static unsigned int GetSizeOfType(unsigned int type);
};

class VertexBufferLayout
{
public:
	VertexBufferLayout();

	void AddFloat(unsigned int count);
	void AddUnsignedInt(unsigned int count);
	void AddUnsignedByte(unsigned int count);

	const std::vector<VertexBufferElement> GetElements() const;;
	unsigned int GetStride() const;;

private:
	void Push(unsigned int type, unsigned int count, unsigned char normalized);;

private:
	unsigned int m_Stride;
	std::vector<VertexBufferElement> m_Elements;
};