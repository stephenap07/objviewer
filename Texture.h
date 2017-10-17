#pragma once

#include <GL/glew.h>

#include <string>

class Image;

class Texture
{
public:

  Texture();

  ~Texture();

  Texture(Texture&&) = default;
  Texture(const Texture &) = delete;
  Texture &operator=(const Texture &) = delete;

  void create();
  bool isValid() const;
  void bind() const;

  void setImage(Image& image);
  void setImageFromFile(const std::string& value);
  void setFilteringLinear();

  void setHandle(const GLuint& value) { handle_ = value; }
  void setTarget(const GLuint& value) { target_ = value; }

  const GLuint& handle() const { return handle_; }
  const GLenum& target() const { return target_; }

private:
  GLuint handle_ = 0;
  GLenum target_ = 0;
};
