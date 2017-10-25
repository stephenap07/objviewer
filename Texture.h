#pragma once

#include <GL/glew.h>

#include <string>

class Image;

class Texture
{
public:
   Texture();
   Texture(const std::string& aFilename);
   ~Texture();

   Texture(Texture&&) = default;
   Texture(Texture const&) = delete;
   void operator=(Texture const&) = delete;

   void create();
   void destroy();
   bool isValid() const;
   void bind() const;

   void setImage(Image& aImage);
   bool setImageFromFile(const std::string& aFilename);
   void setFilteringLinear();

   void setHandle(const GLuint& aValue) { handle_ = aValue; }
   void setTarget(const GLuint& aValue) { target_ = aValue; }

   const GLuint& handle() const { return handle_; }
   const GLenum& target() const { return target_; }

private:
   GLuint handle_ = 0;
   GLenum target_ = 0;
};
