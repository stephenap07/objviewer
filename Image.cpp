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
   data_ = stbi_load(filename.c_str(), &width_, &height_, &channels_, STBI_default);

   if (channels_ == 3)
   {
      mode_ = GL_RGB;
   }
   else if (channels_ == 4)
   {
      mode_ = GL_RGBA;
   }
   else
   {
      std::cerr << "Channels not supported: " << channels_ << '\n';
   }

   if (!data_)
   {
      std::cerr << "Failed to load image: " << filename << '\n';
   }
   else
   {
      //std::cout << "Loaded image " << filename << " width " << width_ << " height " << height_ << " channels " << channels_ << '\n';
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
