#include "ShaderProgram.h"

#include "Texture.h"

#include <SDL2/SDL_opengl.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <fstream>
#include <iostream>
#include <sstream>

//==============================================================================
ShaderProgram::ShaderProgram()
   : handle_(0)
{
}

//==============================================================================
ShaderProgram::~ShaderProgram()
{
}

//==============================================================================
void ShaderProgram::create()
{
}

//==============================================================================
void ShaderProgram::destroy()
{
   glDeleteProgram(handle_);
}

//==============================================================================
void ShaderProgram::link()
{
   if (handle_)
   {
      glDeleteProgram(handle_);
   }

   handle_ = glCreateProgram();

   glAttachShader(handle_, vertexShader_.handle);
   glAttachShader(handle_, fragmentShader_.handle);

   glLinkProgram(handle_);
   glUseProgram(handle_);

   glDetachShader(handle_, vertexShader_.handle);
   glDeleteShader(vertexShader_.handle);
   glDetachShader(handle_, fragmentShader_.handle);
   glDeleteShader(fragmentShader_.handle);
}

//==============================================================================
void ShaderProgram::bind()
{
   glUseProgram(handle_);
}

//==============================================================================
void ShaderProgram::recompile()
{
   loadVertexShader(vertexShader_.filename);
   loadFragmentShader(fragmentShader_.filename);
   link();
}

//==============================================================================
void ShaderProgram::setUniformMatrix4fv(const std::string& uniformName, const glm::mat4& value)
{
   glUseProgram(handle_);
   GLint uniHandle = glGetUniformLocation(handle_, uniformName.c_str());
   if (uniHandle == -1)
   {
      std::cerr << "Error: no uniform named " << uniformName << " for shader " << handle_ << '\n';
      return;
   }
   glUniformMatrix4fv(uniHandle, 1, GL_FALSE, glm::value_ptr(value));
}

//==============================================================================
void ShaderProgram::setUniform1f(const std::string& uniformName, const float& value)
{
   glUseProgram(handle_);
   GLint uniHandle = glGetUniformLocation(handle_, uniformName.c_str());
   if (uniHandle == -1)
   {
      std::cerr << "Error: no uniform named " << uniformName << " for shader " << handle_ << '\n';
      return;
   }
   glUniform1f(uniHandle, value);
}

//==============================================================================
void ShaderProgram::setTexture(int index, const std::string& name, const Texture& texture)
{
   glUseProgram(handle_);
   glActiveTexture(GL_TEXTURE0 + index);
   texture.bind();
   auto attr = glGetUniformLocation(handle_, name.c_str());
   if (attr == -1)
   {
      std::cerr << "Error: no uniform named " << name << " for shader " << handle_ << '\n';
      return;
   }
   glUniform1i(attr, index);
}

//==============================================================================
void ShaderProgram::enableVertexAttrib(const std::string& name, std::size_t count, GLenum type, int stride, util::glPtr ptr)
{
   glUseProgram(handle_);
   GLint attr = glGetAttribLocation(handle_, name.c_str());
   if (attr == -1)
   {
      std::cerr << "Error, could not find attrib loc for '" << name << "'\n";
   }
   glVertexAttribPointer(attr, count, type, GL_FALSE, stride, ptr);
   glEnableVertexAttribArray(attr);
}

//==============================================================================
void ShaderProgram::loadVertexShader(const std::string& filename)
{
   vertexShader_.filename = filename;
   vertexShader_.handle = createShader(filename, GL_VERTEX_SHADER);
}

//==============================================================================
void ShaderProgram::loadFragmentShader(const std::string& filename)
{
   fragmentShader_.filename = filename;
   fragmentShader_.handle = createShader(filename, GL_FRAGMENT_SHADER);
}

//==============================================================================
GLuint ShaderProgram::createShader(const std::string& filename, GLenum type)
{
   GLuint shader = glCreateShader(type);
   const std::string shaderString = getShaderSource(filename);
   if (shaderString.empty())
   {
      std::cerr << "Error, shader " << filename << " is empty\n";
   }
   const GLchar* shaderCStr = static_cast<const GLchar*>(shaderString.c_str());

   glShaderSource(shader, 1, &shaderCStr, NULL);
   glCompileShader(shader);

   GLint shaderStatus;
   glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);
   if (shaderStatus != GL_TRUE)
   {
      std::cerr << "Shader failed to compile!\n";
      GLint len;
      glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
      char* buffer = new char[len];
      glGetShaderInfoLog(shader, len, NULL, buffer);
      std::cerr << "Error Log for shader shader:\n\n"
                << buffer << std::endl;
      delete[] buffer;
   }

   return shader;
}

//==============================================================================
std::string ShaderProgram::getShaderSource(const std::string& filename)
{
   std::ifstream shaderStream(filename);
   std::string shaderString;
   shaderString.assign(std::istreambuf_iterator<char>(shaderStream),
      std::istreambuf_iterator<char>());
   shaderStream.close();
   return shaderString;
}
