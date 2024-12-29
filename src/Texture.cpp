#include "Texture.h"

Texture::Texture()
    : m_texture{ 0 }
{
    glEnable(GL_TEXTURE_2D);
}

Texture::~Texture()
{
    glDeleteTextures(1, &m_texture);
}

void Texture::create()
{
    glGenTextures(1, &m_texture);
}

void Texture::bind(const GLuint slot) const
{
    glActiveTexture(GL_TEXTURE0 + slot);
    glBindTexture(GL_TEXTURE_2D, m_texture);
}

void Texture::release() const
{
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setMinificationFilter(const GLuint type)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, type);
}

void Texture::setMagnificationFilter(const GLuint type)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, type);
}

void Texture::setWrapMode(const GLuint mode)
{
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, mode);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, mode);
}

void Texture::update(const GLubyte* data, const GLuint width, const GLuint height)
{
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
}
