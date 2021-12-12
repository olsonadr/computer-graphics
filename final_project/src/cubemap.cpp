// cubemap.cpp
// source: https://ogldev.org/www/tutorial25/tutorial25.html

#include <cubemap.hpp>

// New using BmpToTexture
bool CubemapTexture::Load()
{
    glGenTextures(1, &m_textureObj);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);

    unsigned char *texture;
    int w, h;

    for (unsigned int i = 0; i < CM_N_FILENAMES; i++)
    {
        texture = BmpToTexture(m_fileNames[i].c_str(), &w, &h);

        glTexImage2D(types[i], 0, GL_RGB, w, h, 0, GL_RGB,
                     GL_UNSIGNED_BYTE, texture);

        delete texture;
    }

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

    return true;
}

// // Old using ImageMagick
// bool CubemapTexture::Load()
// {
//     glGenTextures(1, &m_textureObj);
//     glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);

//     Magick::Image *pImage = NULL;
//     Magick::Blob blob;

//     for (unsigned int i = 0; i < CM_N_FILENAMES; i++)
//     {
//         pImage = new Magick::Image(m_fileNames[i]);

//         try
//         {
//             pImage->write(&blob, "RGBA");
//         }
//         catch (Magick::Error &Error)
//         {
//             std::cout << "Error loading texture '" << m_fileNames[i] << "': " << Error.what() << std::endl;
//             delete pImage;
//             return false;
//         }

//         glTexImage2D(types[i], 0, GL_RGB, pImage->columns(), pImage->rows(), 0, GL_RGBA,
//                      GL_UNSIGNED_BYTE, blob.data());
//         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
//         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
//         glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

//         delete pImage;
//     }

//     return true;
// }

void CubemapTexture::Bind(GLenum TextureUnit)
{
    glActiveTexture(TextureUnit);
    glBindTexture(GL_TEXTURE_CUBE_MAP, m_textureObj);
}

CubemapTexture::CubemapTexture(const std::string &Directory,
                               const std::string &PosXFilename,
                               const std::string &NegXFilename,
                               const std::string &PosYFilename,
                               const std::string &NegYFilename,
                               const std::string &PosZFilename,
                               const std::string &NegZFilename)
{
    const std::string strs[CM_N_FILENAMES] = {PosXFilename, NegXFilename, PosYFilename,
                                              NegYFilename, PosZFilename, NegZFilename};
    for (int i = 0; i < CM_N_FILENAMES; i++) {
        m_fileNames[i] = Directory + "/" + strs[i];
    }
}

CubemapTexture::~CubemapTexture()
{
    return;
}