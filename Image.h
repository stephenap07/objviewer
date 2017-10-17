#pragma once

#include <GL/glew.h>

#include <string>

class Image
{
public:
   Image();

   ~Image();

   void loadFromFile(const std::string filename);
   void freeImage();

   void setWidth(const int& value) { width_ = value; }
   void setHeight(const int& value) { height_ = value; }
   void setChannels(const int& value) { channels_ = value; }
   void setMode(const GLenum& value) { mode_ = value; }
   void setData(unsigned char* value) { data_ = value; }

   const int& width() const { return width_; }
   const int& height() const { return height_; }
   const int& channels() const { return channels_; }
   const GLenum& mode() const { return mode_; }
   unsigned char* data() { return data_; }

private:
   int width_;
   int height_;
   int channels_;
   GLenum mode_;
   unsigned char* data_;
};
