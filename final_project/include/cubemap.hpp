// cubemap.hpp
// source: https://ogldev.org/www/tutorial25/tutorial25.html
#ifndef CUBEMAP_HPP // include guard
#define CUBEMAP_HPP

#include <string>
#include <cstring>
#include <iostream>
#include <GL/glew.h>
#include "osu/bmp.hpp"
// #include "ImageMagick-6/Magick++.h"

#define CM_N_FILENAMES 6

#define ARRAY_SIZE_IN_ELEMENTS(a)              \
    ((sizeof(a) / sizeof(*(a))) / \
     static_cast<size_t>(!(sizeof(a) % sizeof(*(a)))))

static const GLenum types[6] = {GL_TEXTURE_CUBE_MAP_POSITIVE_X,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
                                GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
                                GL_TEXTURE_CUBE_MAP_NEGATIVE_Z};

class CubemapTexture {
public:
    CubemapTexture(const std::string &Directory,
                   const std::string &PosXFilename,
                   const std::string &NegXFilename,
                   const std::string &PosYFilename,
                   const std::string &NegYFilename,
                   const std::string &PosZFilename,
                   const std::string &NegZFilename);

    ~CubemapTexture();

    bool Load();

    void Bind(GLenum TextureUnit);

private:
    std::string m_fileNames[CM_N_FILENAMES];
    GLuint m_textureObj;
};

#endif // CUBEMAP_HPP