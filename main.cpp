#define GLEW_STATIC
#include <GL/glew.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_opengl.h>

#include <fstream>
#include <iostream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#include "Camera.h"

namespace util
{
  template <typename T>
  GLsizei stride(std::size_t number)
  {
    return number * sizeof(T);
  }

  template <typename T>
  void *glPtr(std::size_t pos)
  {
    return (void *)(pos * sizeof(T));
  }
}

std::string getShaderSource(const char *filename);

//==============================================================================
GLuint createShader(const char *filename, GLenum type)
{
  GLuint shader = glCreateShader(type);

  const std::string shaderString = getShaderSource(filename);
  const GLchar *shaderCStr = (const GLchar *)shaderString.c_str();
  glShaderSource(shader, 1, &shaderCStr, NULL);
  glCompileShader(shader);

  GLint shaderStatus;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &shaderStatus);

  if (shaderStatus != GL_TRUE) {
    std::cerr << "Shader failed to compile!\n";
    GLint len;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    char *buffer = new char[len];
    glGetShaderInfoLog(shader, len, NULL, buffer);
    std::cerr << "Error Log for shader shader:\n\n" << buffer << std::endl;
    delete[] buffer;
  }

  return shader;
}

//==============================================================================
std::string getShaderSource(const char *filename)
{
  std::ifstream shaderStream(filename);
  std::string shaderString;

  shaderString.assign(std::istreambuf_iterator<char>(shaderStream),
                      std::istreambuf_iterator<char>());

  shaderStream.close();
  return shaderString;
}

//==============================================================================
class Renderer
{
public:
};

//==============================================================================
int main(int argc, char *argv[])
{
  // Initialize SDL
  SDL_Init(SDL_INIT_VIDEO);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 1);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_Window *window = SDL_CreateWindow("OpenGL", 100, 100, 800, 600, SDL_WINDOW_OPENGL);
  SDL_GLContext context = SDL_GL_CreateContext(window);

  // Initialize GLEW
  glewExperimental = GL_TRUE;
  glewInit();

  glEnable(GL_DEPTH_TEST);
  glEnable(GL_STENCIL_TEST);

  // Create Vertex Array Object
  GLuint vao;
  glGenVertexArrays(1, &vao);
  glBindVertexArray(vao);

  // clang-format off
  GLfloat vertices[] = {
     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

     -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

     -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,

     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     -0.5f, -0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     -0.5f, -0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

     -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,
     0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,
     -0.5f,  0.5f,  0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,
     -0.5f,  0.5f, -0.5f, 1.0f, 1.0f, 1.0f, 0.0f, 1.0f,

     -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,
      1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f,
      1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
      1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 1.0f,
     -1.0f,  1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f,
     -1.0f, -1.0f, -0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f
  };
  // clang-format on

  // Create a Vertex Buffer Object and copy the vertex data to it
  GLuint vbo;
  glGenBuffers(1, &vbo);
  glBindBuffer(GL_ARRAY_BUFFER, vbo);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  GLuint vertexShader = createShader("vertex.vert", GL_VERTEX_SHADER);
  GLuint fragmentShader = createShader("fragment.frag", GL_FRAGMENT_SHADER);

  GLuint shaderProgram = glCreateProgram();
  glAttachShader(shaderProgram, vertexShader);
  glAttachShader(shaderProgram, fragmentShader);

  glLinkProgram(shaderProgram);
  glUseProgram(shaderProgram);

  auto stride = util::stride<GLfloat>(8);
  GLint posAttrib = glGetAttribLocation(shaderProgram, "position");
  glVertexAttribPointer(posAttrib, 3, GL_FLOAT, GL_FALSE, stride, 0);
  glEnableVertexAttribArray(posAttrib);

  GLint colAttrib = glGetAttribLocation(shaderProgram, "color");
  glEnableVertexAttribArray(colAttrib);
  glVertexAttribPointer(colAttrib, 3, GL_FLOAT, GL_FALSE, stride, util::glPtr<GLfloat>(3));

  GLint texAttrib = glGetAttribLocation(shaderProgram, "texcoord");
  glEnableVertexAttribArray(texAttrib);
  glVertexAttribPointer(texAttrib, 2, GL_FLOAT, GL_FALSE, stride, util::glPtr<GLfloat>(6));

  GLuint textures[2];
  glGenTextures(2, textures);

  // Load the kitty image
  {
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textures[0]);

    int w;
    int h;
    int n;
    unsigned char *kittyImage = stbi_load("kitty.png", &w, &h, &n, 0);

    if (!kittyImage) {
      std::cerr << "Failed to load image: kitty.png\n";
      return -1;
    }

    auto mode = GL_RGB;
    if (n == 4) {
      mode = GL_RGBA;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE,
                 kittyImage);
    stbi_image_free(kittyImage);

    glUniform1i(glGetUniformLocation(shaderProgram, "texKitten"), 0);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  // Load puppy image
  {
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, textures[1]);

    int w;
    int h;
    int n;
    unsigned char *puppyImage = stbi_load("puppy.png", &w, &h, &n, 0);

    auto mode = GL_RGB;
    if (n == 4) {
      mode = GL_RGBA;
    }

    if (!puppyImage) {
      std::cerr << "Failed to load image: puppy.png\n";
      return -1;
    }

    glTexImage2D(GL_TEXTURE_2D, 0, mode, w, h, 0, mode, GL_UNSIGNED_BYTE,
                 puppyImage);
    stbi_image_free(puppyImage);

    glUniform1i(glGetUniformLocation(shaderProgram, "texPuppy"), 1);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  }

  glm::mat4 model;
  Camera camera;

  GLint uniModel      = glGetUniformLocation(shaderProgram, "model");
  GLint uniView       = glGetUniformLocation(shaderProgram, "view");
  GLint uniProjection = glGetUniformLocation(shaderProgram, "projection");
  GLuint uniTime      = glGetUniformLocation(shaderProgram, "time");

  unsigned long currentTime = SDL_GetTicks();
  unsigned long lastTime = currentTime;
  float deltaTime = 0;

  bool quit = false;
  while (!quit) {
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
      if (event.type == SDL_QUIT) {
        quit = true;
      }
      camera.orbitController(event);
    }

    // Clear the screen to black
    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    currentTime = SDL_GetTicks();
    deltaTime = ((float)currentTime - (float)lastTime) / 1000.0f;
    lastTime = SDL_GetTicks();

    // Models
    {
      auto s = glm::scale(model, glm::vec3(1, 1, 1));
      auto r = glm::rotate(model, 0.0f, glm::vec3(0, 1, 0));
      auto t = glm::translate(model, glm::vec3(0, 0, 0));
      auto m = t * r * s;

      glUniform1f(uniTime, 0.0f);
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(m));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    {
      auto s = glm::scale(model, glm::vec3(1, -1, 1));
      auto r = glm::rotate(model, 0.0f, glm::vec3(0, 1, 0));
      auto t = glm::translate(model, glm::vec3(0, 1, 0));
      auto m = t * r * s;

      glUniform1f(uniTime, (GLfloat)currentTime / 1000.0f);
      glUniformMatrix4fv(uniModel, 1, GL_FALSE, glm::value_ptr(m));
      glDrawArrays(GL_TRIANGLES, 0, 36);
    }

    // Camera
    glUniformMatrix4fv(uniView, 1, GL_FALSE, glm::value_ptr(camera.viewMatrix()));
    glUniformMatrix4fv(uniProjection, 1, GL_FALSE, glm::value_ptr(camera.projectionMatrix()));

    SDL_GL_SwapWindow(window);
  }

  glDeleteTextures(2, textures);
  glDeleteShader(vertexShader);
  glDeleteShader(fragmentShader);
  glDeleteVertexArrays(1, &vao);

  SDL_GL_DeleteContext(context);
  SDL_Quit();
  return 0;
}
