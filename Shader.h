#pragma once

#include <GL/glew.h>

#include <string>

class Shader
{
public:
  Shader() = default;
  Shader(const Shader &) = default;
  Shader(Shader &&) = default;
  Shader &operator=(const Shader &) = default;

  ~Shader();

  void loadFromFile(const std::string &filename, const GLenum &target);

  void destroy();
  void setHandle(const GLuint &value) { handle_ = value; }
  const GLuint& handle() const { return handle_; }

private:
  std::string getShaderSource(const std::string &filename);

  GLuint handle_;
};

class ShaderVertex : public Shader
{
public:
  struct HandlePolicy
  {
    using HandleType = ShaderVertex;

    static void loadFromFile(HandleType handle, const std::string &filename)
    {
      handle.loadFromFile(filename, GL_VERTEX_SHADER);
    }
    static void close(HandleType handle) { handle.destroy(); }
    static HandleType getNull() { return HandleType(); }
    static bool isNull(HandleType handle) { return handle.handle() == 0; }
  };
};

class ShaderFragment : public Shader
{
public:
  struct HandlePolicy
  {
    using HandleType = ShaderFragment;

    static void loadFromFile(HandleType handle, const std::string &filename)
    {
      handle.loadFromFile(filename, GL_FRAGMENT_SHADER);
    }
    static void close(HandleType handle) { handle.destroy(); }
    static HandleType getNull() { return HandleType(); }
    static bool isNull(HandleType handle) { return handle.handle() == 0; }
  };
};
