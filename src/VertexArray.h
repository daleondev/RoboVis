#pragma once

#include "Buffer.h"

#include <GLES3/gl3.h>

#include <memory>
#include <vector>

class VertexArray 
{
public:
    VertexArray();
    ~VertexArray();

    void bind() const;
    void release() const;

    void addVertexBuffer(const std::shared_ptr<VertexBuffer>& vertexBuffer);
    void setIndexBuffer(const std::shared_ptr<IndexBuffer>& indexBuffer);

    inline const std::vector<std::shared_ptr<VertexBuffer>>& getVertexBuffers() const { return m_vertexBuffers; }
    inline const std::shared_ptr<IndexBuffer>& getIndexBuffer() const { return m_indexBuffer; }

private:
    std::vector<std::shared_ptr<VertexBuffer>> m_vertexBuffers;
    std::shared_ptr<IndexBuffer> m_indexBuffer;
    uint32_t m_array;
};