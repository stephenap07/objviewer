#include "Texture.h"

#include "Image.h"

#include <GL/glew.h>

//==============================================================================
Texture::Texture()
   : handle_(0)
   , target_(GL_TEXTURE_2D)
{
}

//==============================================================================
Texture::~Texture()
{
   glDeleteTextures(1, &handle_);
}

//==============================================================================
void Texture::create()
{
   glGenTextures(1, &handle_);
   target_ = GL_TEXTURE_2D;
}

//==============================================================================
void Texture::bind() const
{
   glBindTexture(target_, handle_);
}

//==============================================================================
bool Texture::isValid() const
{
   return handle_ != -1 && target_ != 0;
}

//==============================================================================
void Texture::setImage(Image& image)
{
   glActiveTexture(GL_TEXTURE0);
   glBindTexture(target_, handle_);
   glTexImage2D(target_, 0, image.mode(), image.width(), image.height(), 0, image.mode(), GL_UNSIGNED_BYTE, image.data());
}

//==============================================================================
void Texture::setImageFromFile(const std::string& filename)
{
   create();
   Image image;
   image.loadFromFile(filename);
   setImage(image);
}

//==============================================================================
void Texture::setFilteringLinear()
{
   bind();
   glTexParameteri(target_, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
   glTexParameteri(target_, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
   glTexParameteri(target_, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
   glTexParameteri(target_, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
}
