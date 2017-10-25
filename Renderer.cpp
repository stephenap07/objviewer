#include "Renderer.h"

#include "AssetHandler.h"
#include "Camera.h"
#include "Image.h"
#include "ObjModelLoader.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Util.h"
#include "Vertices.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <cmath>
#include <fstream>
#include <iostream>
#include <limits>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

//==============================================================================
void Renderable::destroy()
{
   glDeleteBuffers(1, &vb);
   glDeleteVertexArrays(1, &vao);
}

//==============================================================================
Renderable::Renderable()
   : vb(0)
   , vao(0)
   , numTriangles(0)
   , materialId(0)
   , shaderProgramId(0)
{
}

//==============================================================================
Renderer::Renderer()
{
}

//==============================================================================
Renderer::~Renderer()
{
   // Clean up all OpenGL resources here.
   for (auto& texturePair : textures_)
   {
      texturePair.second.destroy();
   }

   for (auto& renderable : renderables_)
   {
      renderable.destroy();
   }

   for (auto& shaderProgram : shaderPrograms_)
   {
      shaderProgram.destroy();
   }
}

//==============================================================================
void Renderer::render()
{
   // Clear the screen to black
   glClearColor(0.0f, 1.0f, 1.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   for (auto& m : renderEntities_)
   {
      for (auto& r : modelSurfaces_[m.modelSurfaceId].renderables)
      {
         // TODO: Some of these calls are redundant.
         auto& renderable{ renderables_[r] };
         auto& shader = shaderPrograms_[renderable.shaderProgramId];
         shader.bind();
         shader.setUniformMatrix4fv("model", m.matrix);
         shader.setUniformMatrix4fv("view", camera_.viewMatrix());
         //shader->setUniformMatrix4fv("projection", camera_.orthographicMatrix());
         shader.setUniformMatrix4fv("projection", camera_.projectionMatrix());
         if (renderable.shaderProgramId == 1)
         {
            shader.setTexture(0, "tex", textures_[materials_[renderable.materialId].diffuseTexname]);
         }

         glBindVertexArray(renderable.vao);
         glDrawArrays(GL_TRIANGLES, 0, 3 * renderable.numTriangles);
      }
   }
}

//==============================================================================
void Renderer::initialize()
{
   initializeShaders();
   //modelSurfaces_.push_back(loadObjAndConvert("models/cube.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/capsule.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/bunny.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/cornell_box.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/catmark_torus_creases0.obj", "models/"));
   modelSurfaces_.push_back(loadObjAndConvert("models/sponza.obj", "models/"));

   for (std::size_t i = 0; i < modelSurfaces_.size(); ++i)
   {
      auto& m = modelSurfaces_[i];
      glm::mat4 model;
      auto s = glm::scale(model, 1.0f / glm::vec3(m.boundingBox.maxExtent()));
      auto r = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
      auto t = glm::translate(model, 1.0f / m.boundingBox.centerTranslate());
      renderEntities_.push_back(RenderEntity{ i, t * r * s });
   }
}

//==============================================================================
void Renderer::initializeShaders()
{
   shaderPrograms_.resize(2);

   // Shader that does plain polygon shading, no textures.
   shaderPrograms_[0].create();
   shaderPrograms_[0].loadVertexShader("shader/obj/objShader.vert");
   shaderPrograms_[0].loadFragmentShader("shader/obj/objShader.frag");
   shaderPrograms_[0].link();

   // Shader that supports one texture
   shaderPrograms_[1].create();
   shaderPrograms_[1].loadVertexShader("shader/obj/objShader.vert");
   shaderPrograms_[1].loadFragmentShader("shader/obj/objShaderWithTexture.frag");
   shaderPrograms_[1].link();
}

//==============================================================================
Model Renderer::loadObjAndConvert(const std::string& filename,
   const std::string& basedir)
{
   ObjModelLoader theLoader(*this, filename, basedir);
   return theLoader.getModel();
}
