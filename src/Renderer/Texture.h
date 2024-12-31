#pragma once

class Texture {

public:
    Texture();
    ~Texture();

    void create();

    void bind(const GLuint slot = 0) const;
    void release() const;

    void setMinificationFilter(const GLuint type);
    void setMagnificationFilter(const GLuint type);
    void setWrapMode(const GLuint mode);

    void update(const GLubyte* data, const GLuint width, const GLuint height);

private:
    GLuint m_texture;
};
