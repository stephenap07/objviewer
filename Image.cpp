#include "Image.h"

#define STB_IMAGE_IMPLEMENTATION
#include "extern/stb_image.h"

#include <iostream>

//==============================================================================
Image::Image()
   : width_(0)
   , height_(0)
   , channels_(0)
   , mode_(GL_RGBA)
   , data_(nullptr)
{
}

//==============================================================================
Image::~Image()
{
   freeImage();
}

//==============================================================================
void Image::loadFromFile(const std::string filename)
{
   data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, 0);
   if (channels_ == 4)
   {
      mode_ = GL_RGBA;
   }
   if (!data_)
   {
      std::cerr << "Failed to load image: " << filename << '\n';
   }
}

//==============================================================================
void Image::freeImage()
{
   if (data_)
   {
      stbi_image_free(data_);
   }
}
