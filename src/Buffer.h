#pragma once

#include <GLES3/gl3.h>

#include <vector>
#include <cassert>
#include <cstdint>
#include <string_view>

enum class ShaderDataType : uint8_t {
    None = 0,
    Bool,
    Float,
    Float2,
    Float3,
    Float4,
    Float2x2,
    Float3x3,
    Float4x4,
    Int,
    Int2,
    Int3,
    Int4,
    Int2x2,
    Int3x3,
    Int4x4
};

static size_t shaderDataTypeSize(const ShaderDataType type)
{
    switch(type) {
        case ShaderDataType::Bool:       return sizeof(bool);
        case ShaderDataType::Float:      return sizeof(float);
        case ShaderDataType::Float2:     return sizeof(float)*2;
        case ShaderDataType::Float3:     return sizeof(float)*3;
        case ShaderDataType::Float4:     return sizeof(float)*4;
        case ShaderDataType::Float2x2:   return sizeof(float)*2*2;
        case ShaderDataType::Float3x3:   return sizeof(float)*3*3;
        case ShaderDataType::Float4x4:   return sizeof(float)*4*4;
        case ShaderDataType::Int:        return sizeof(int);
        case ShaderDataType::Int2:       return sizeof(int)*2;
        case ShaderDataType::Int3:       return sizeof(int)*3;
        case ShaderDataType::Int4:       return sizeof(int)*4;
        case ShaderDataType::Int2x2:     return sizeof(int)*2*2;
        case ShaderDataType::Int3x3:     return sizeof(int)*3*3;
        case ShaderDataType::Int4x4:     return sizeof(int)*4*4;
        case ShaderDataType::None:       return 0;
    }

    assert(false && "Unknown shader data type");
    return 0;
}

struct BufferElement {
    ShaderDataType type;
    std::string_view name;     
    size_t size;
    size_t offset;  
    bool normalized;

    BufferElement() = default;
    BufferElement(ShaderDataType type, const std::string_view& name, bool normalized = false)
        : type(type), name(name), size(shaderDataTypeSize(type)), offset(0), normalized(normalized)
    {       
    } 

    size_t getComponentCount() const
    {
        switch(type) {
            case ShaderDataType::Bool:       return 1;
            case ShaderDataType::Float:      return 1;
            case ShaderDataType::Float2:     return 2;
            case ShaderDataType::Float3:     return 3;
            case ShaderDataType::Float4:     return 4;
            case ShaderDataType::Float2x2:   return 2*2;
            case ShaderDataType::Float3x3:   return 3*3;
            case ShaderDataType::Float4x4:   return 4*4;
            case ShaderDataType::Int:        return 1;
            case ShaderDataType::Int2:       return 2;
            case ShaderDataType::Int3:       return 3;
            case ShaderDataType::Int4:       return 4;
            case ShaderDataType::Int2x2:     return 2*2;
            case ShaderDataType::Int3x3:     return 3*3;
            case ShaderDataType::Int4x4:     return 4*4;
            case ShaderDataType::None:       return 0;
        }

        assert(false && "Unknown shader data type");
        return 0;
    }
};

class BufferLayout 
{
public:
    typedef std::vector<BufferElement>::iterator BufferLayoutIterator;
    typedef std::vector<BufferElement>::const_iterator BufferLayoutConstIterator;

    BufferLayout() = default;
    BufferLayout(const std::initializer_list<BufferElement>& elements)
        : m_elements(elements), m_stride(0)
    {     
        calculateOffsetsAndStride();
    }

    inline const std::vector<BufferElement>& getElements() const { return m_elements; }
    inline size_t getStride() const { return m_stride; }

    inline BufferLayoutIterator begin() { return m_elements.begin(); }
    inline BufferLayoutIterator end() { return m_elements.end(); }
    inline BufferLayoutConstIterator begin() const { return m_elements.cbegin(); }
    inline BufferLayoutConstIterator end() const { return m_elements.cend(); }

private:
    void calculateOffsetsAndStride()
    {
        size_t offset = 0;
        m_stride = 0;
        for (auto& element : m_elements) {
            element.offset = offset;
            offset += element.size;
            m_stride += element.size;
        }
    }

    std::vector<BufferElement> m_elements;
    size_t m_stride;
};

class VertexBuffer 
{
public:
    VertexBuffer();
    ~VertexBuffer();

    void allocate(const float* vertices, const size_t count);

    void bind() const;
    void release() const;

    inline void setLayout(const BufferLayout& layout) { m_layout = layout; }
    inline const BufferLayout& getLayout() const { return m_layout; }

private: 
    uint32_t m_buffer;
    BufferLayout m_layout;
};

class IndexBuffer  
{
public:
    IndexBuffer();
    ~IndexBuffer();

    void allocate(const uint16_t* indices, const size_t count);

    void bind() const;
    void release() const;

    inline size_t getCount() const { return m_count; }

private: 
    uint32_t m_buffer;
    size_t m_count;
};