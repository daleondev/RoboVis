#include "pch.h"

#include "FrameBuffer.h"

FrameBuffer::FrameBuffer(const uint16_t width, const uint16_t height)
    : m_width(width), m_height(height)
{
    glGenFramebuffers(1, &m_buffer);
    resize(width, height);
}

FrameBuffer::~FrameBuffer()
{
    glDeleteFramebuffers(1, &m_buffer);
}

void FrameBuffer::resize(const uint16_t width, const uint16_t height)
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);

    glGenTextures(1, &m_colorAttachment);
    glBindTexture(GL_TEXTURE_2D, m_colorAttachment);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);	
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, nullptr);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorAttachment, 0);
    assert(glCheckFramebufferStatus(GL_FRAMEBUFFER) == GL_FRAMEBUFFER_COMPLETE && "Framebuffer incomplete");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void FrameBuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_buffer);
}

void FrameBuffer::release() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}