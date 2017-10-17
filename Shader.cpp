#include "Shader.h"

#include <fstream>
#include <iostream>

Shader::~Shader()
{
  glDeleteShader(handle_);
}

void Shader::destroy()
{
  glDeleteShader(handle_);
}

void Shader::loadFromFile(const std::string &filename, const GLenum &target)
{
  handle_ = glCreateShader(target);
  const std::string shaderString = getShaderSource(filename);
  const GLchar *shaderCStr = static_cast<const GLchar *>(shaderString.c_str());
  glShaderSource(handle_, 1, &shaderCStr, NULL);
  glCompileShader(handle_);

  GLint shaderStatus;
  glGetShaderiv(handle_, GL_COMPILE_STATUS, &shaderStatus);
  if (shaderStatus != GL_TRUE) {
    std::cerr << "Shader failed to compile!\n";
    GLint len;
    glGetShaderiv(handle_, GL_INFO_LOG_LENGTH, &len);
    char *buffer = new char[len];
    glGetShaderInfoLog(handle_, len, NULL, buffer);
    std::cerr << "Error Log for shader shader:\n\n" << buffer << std::endl;
    delete[] buffer;
  }
}

std::string Shader::getShaderSource(const std::string &filename)
{
  std::ifstream shaderStream(filename);
  std::string shaderString;
  shaderString.assign(std::istreambuf_iterator<char>(shaderStream),
                      std::istreambuf_iterator<char>());
  shaderStream.close();
  return shaderString;
}
