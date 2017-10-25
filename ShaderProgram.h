#pragma once

#include "Util.h"

#include <GL/glew.h>

#include <glm/glm.hpp>

#include <string>
#include <unordered_map>

class Texture;

class ShaderProgram
{
public:
  struct VertexShader {
    std::string filename;
    GLuint handle = 0;
  };

  struct FragmentShader {
    std::string filename;
    GLuint handle = 0;
  };

  ShaderProgram();
  ShaderProgram(const ShaderProgram &) = delete;
  ShaderProgram(ShaderProgram&&) = default;
  ~ShaderProgram();
  ShaderProgram &operator=(const ShaderProgram &) = delete;

  void create();
  void destroy();
  void link();
  void bind();
  void recompile();

  void setUniformMatrix4fv(const std::string &uniformName, const glm::mat4 &value);
  void setUniform1f(const std::string &uniformName, const float &value);

  void setTexture(int index, const std::string& name, const Texture& texture);

  void enableVertexAttrib(const std::string& name, std::size_t count, GLenum type, int stride, util::glPtr ptr);

  void loadVertexShader(const std::string& filename);
  void loadFragmentShader(const std::string& filename);

  void setHandle(const GLuint &value) { handle_ = value; }
  const GLuint &handle() const { return handle_; }

private:
  GLuint createShader(const std::string &filename, GLenum type);
  std::string getShaderSource(const std::string &filename);

  GLuint handle_;

  VertexShader vertexShader_;
  FragmentShader fragmentShader_;
};
