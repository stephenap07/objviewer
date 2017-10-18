#include "Renderer.h"

#include "AssetHandler.h"
#include "Camera.h"
#include "Image.h"
#include "Shader.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "Util.h"
#include "Vertices.h"

#include <GL/glew.h>
#include <SDL2/SDL.h>

#include <fstream>
#include <iostream>
#include <limits>
#include <utility>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/normal.hpp>

//==============================================================================
BoundingBox::BoundingBox()
{
   bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
   bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
}

//==============================================================================
Renderer::Renderer()
{
}

//==============================================================================
Renderer::~Renderer()
{
}

//==============================================================================
void Renderer::render()
{
   // Clear the screen to black
   glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

   for (auto& m : renderEntities_)
   {
      objProgram_.bind();
      objProgram_.setUniformMatrix4fv("model", m.matrix);

      for (auto r : modelSurfaces_[m.modelSurfaceId].renderables)
      {
         textures_[materials_[renderables_[r].materialId].diffuseTexname].bind();
         glBindVertexArray(renderables_[r].vao);
         glDrawArrays(GL_TRIANGLES, 0, 3 * renderables_[r].numTriangles);
      }
   }

   objProgram_.setUniformMatrix4fv("view", camera_.viewMatrix());
   objProgram_.setUniformMatrix4fv("projection", camera_.projectionMatrix());
}

//==============================================================================
void Renderer::initialize()
{
   initializeShaders();
   //modelSurfaces_.push_back(loadObjAndConvert("models/cube.obj", "models/"));
   modelSurfaces_.push_back(loadObjAndConvert("models/capsule.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/bunny.obj", "models/"));
   //modelSurfaces_.push_back(loadObjAndConvert("models/cornell_box.obj", "models/"));

   // Push back default material.
   materials_.push_back(Material());

   for (std::size_t i = 0; i < modelSurfaces_.size(); ++i)
   {
      auto& m = modelSurfaces_[i];
      auto e = m.bb.maxExtent();
      glm::mat4 model;
      auto s = glm::scale(model, glm::vec3(m.bb.maxExtent()) / 2.0f);
      auto r = glm::rotate(model, glm::radians(0.0f), glm::vec3(0, 1, 0));
      auto t = glm::translate(model, m.bb.centerTranslate());
      renderEntities_.push_back(RenderEntity{ i, t * r * s });
   }
}

//==============================================================================
void Renderer::initializeShaders()
{
   objProgram_.create();
   objProgram_.loadVertexShader("shader/obj/objShader.vert");
   objProgram_.loadFragmentShader("shader/obj/objShader.frag");
   objProgram_.link();
}

//==============================================================================
ModelSurface Renderer::loadObjAndConvert(const std::string& filename,
   const std::string& basedir)
{
   std::size_t matIndex(materials_.size());

   tinyobj::attrib_t attrib;
   std::vector<tinyobj::shape_t> shapes;
   std::vector<tinyobj::material_t> materials;
   std::string err;
   bool ok = tinyobj::LoadObj(&attrib, &shapes, &materials, &err, filename.c_str(), basedir.c_str());

   // Default material
   materials.push_back(tinyobj::material_t());

   if (!err.empty())
   {
      std::cerr << err << std::endl;
   }

   if (!ok)
   {
      std::cerr << "Failed to load " << filename << std::endl;
   }

   std::cout << "# of verticies " << attrib.vertices.size() / 3 << '\n';
   std::cout << "# of normals " << attrib.normals.size() / 3 << '\n';
   std::cout << "# of texcoords " << attrib.texcoords.size() / 2 << '\n';
   std::cout << "# of materials " << materials.size() << '\n';
   std::cout << "# of shapes " << shapes.size() << '\n';

   ModelSurface model;

   std::cout << "Found " << materials.size() - 1 << " materials\n";

   // Load diffuse textures.
   for (const auto& m : materials)
   {
      materials_.push_back(m);
      if (m.diffuse_texname.length())
      {
         std::cout << "Loading diffuse " << m.diffuse_texname << '\n';
         Texture texture;
         texture.setImageFromFile(m.diffuse_texname);
         texture.setFilteringLinear();
         textures_.insert(std::make_pair<std::string, Texture>(
            std::string(m.diffuse_texname), std::move(texture)));
      }
      else if (m.name.size())
      {
         std::cout << "Loading material " << m.name << '\n';
      }
   }

   std::size_t shapeIndex = 0;
   for (const auto& s : shapes)
   {
      Renderable r;
      std::vector<float> vb;

      for (std::size_t f = 0; f < s.mesh.indices.size() / 3; ++f)
      {
         tinyobj::index_t idx0 = s.mesh.indices[3 * f + 0];
         tinyobj::index_t idx1 = s.mesh.indices[3 * f + 1];
         tinyobj::index_t idx2 = s.mesh.indices[3 * f + 2];

         int currentMaterialId = s.mesh.material_ids[f];
         if (currentMaterialId < 0 || currentMaterialId >= static_cast<int>(materials.size()))
         {
            // Set the material id to the default material.
            currentMaterialId = materials.size() - 1;
         }

         glm::vec3 diffuse;
         for (std::size_t i = 0; i < 3; ++i)
         {
            diffuse[i] = materials[currentMaterialId].diffuse[i];
         }

         float tc[3][2];
         if (attrib.texcoords.size())
         {
            assert(attrib.texcoords.size() > 2 * idx0.texcoord_index + 1);
            assert(attrib.texcoords.size() > 2 * idx1.texcoord_index + 1);
            assert(attrib.texcoords.size() > 2 * idx2.texcoord_index + 1);
            tc[0][0] = attrib.texcoords[2 * idx0.texcoord_index];
            tc[0][1] = 1.0f - attrib.texcoords[2 * idx0.texcoord_index + 1];
            tc[1][0] = attrib.texcoords[2 * idx1.texcoord_index];
            tc[1][1] = 1.0f - attrib.texcoords[2 * idx1.texcoord_index + 1];
            tc[2][0] = attrib.texcoords[2 * idx2.texcoord_index];
            tc[2][1] = 1.0f - attrib.texcoords[2 * idx2.texcoord_index + 1];
         }
         else
         {
            tc[0][0] = 0.0f;
            tc[0][1] = 0.0f;
            tc[1][0] = 0.0f;
            tc[1][1] = 0.0f;
            tc[2][0] = 0.0f;
            tc[2][1] = 0.0f;
         }

         // Vertices
         glm::vec3 v[3];
         for (int k = 0; k < 3; k++)
         {
            int f0 = idx0.vertex_index;
            int f1 = idx1.vertex_index;
            int f2 = idx2.vertex_index;

            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);

            v[0][k] = attrib.vertices[3 * f0 + k];
            v[1][k] = attrib.vertices[3 * f1 + k];
            v[2][k] = attrib.vertices[3 * f2 + k];

            model.bb.bmin[k] = std::min(v[0][k], model.bb.bmin[k]);
            model.bb.bmin[k] = std::min(v[1][k], model.bb.bmin[k]);
            model.bb.bmin[k] = std::min(v[2][k], model.bb.bmin[k]);
            model.bb.bmax[k] = std::max(v[0][k], model.bb.bmax[k]);
            model.bb.bmax[k] = std::max(v[1][k], model.bb.bmax[k]);
            model.bb.bmax[k] = std::max(v[2][k], model.bb.bmax[k]);
         }

         // Normals
         glm::vec3 n[3];
         if (attrib.normals.size() > 0)
         {
            int f0 = idx0.normal_index;
            int f1 = idx1.normal_index;
            int f2 = idx2.normal_index;

            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);

            for (int k = 0; k < 3; k++)
            {
               n[k] = glm::vec3(attrib.normals[3 * f0 + k],
                  attrib.normals[3 * f1 + k],
                  attrib.normals[3 * f2 + k]);
            }
         }
         else
         {
            n[0] = glm::normalize(glm::triangleNormal(v[0], v[1], v[2]));
            n[1] = n[0];
            n[2] = n[0];
         }

         for (int i = 0; i < 3; ++i)
         {
            vb.push_back(v[i][0]);
            vb.push_back(v[i][1]);
            vb.push_back(v[i][2]);

            vb.push_back(n[i][0]);
            vb.push_back(n[i][1]);
            vb.push_back(n[i][2]);

            vb.push_back(diffuse[0]);
            vb.push_back(diffuse[1]);
            vb.push_back(diffuse[2]);

            vb.push_back(tc[i][0]);
            vb.push_back(tc[i][1]);
         }
      }

      r.vb = 0;
      r.numTriangles = 0;

      constexpr int defaultMaterialId{ 0 };

      if (s.mesh.material_ids.size() > 0 && s.mesh.material_ids.size() > shapeIndex)
      {
         r.materialId = s.mesh.material_ids[shapeIndex] + matIndex;
      }
      else
      {
         r.materialId = defaultMaterialId;
      }

      if (vb.size() > 0)
      {
         glGenVertexArrays(1, &r.vao);
         glBindVertexArray(r.vao);

         glGenBuffers(1, &r.vb);
         glBindBuffer(GL_ARRAY_BUFFER, r.vb);
         glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(float), &vb.at(0), GL_STATIC_DRAW);
         const std::size_t numElements(11);
         r.numTriangles = vb.size() / numElements / 3;

         const auto stride = util::stride<GLfloat>(numElements);
         objProgram_.enableVertexAttrib("position", 3, GL_FLOAT, stride, 0);
         objProgram_.enableVertexAttrib("normal", 3, GL_FLOAT, stride, util::toGlPtr<GLfloat>(3));
         objProgram_.enableVertexAttrib("diffuse", 3, GL_FLOAT, stride, util::toGlPtr<GLfloat>(6));
         objProgram_.enableVertexAttrib("texcoord", 2, GL_FLOAT, stride, util::toGlPtr<GLfloat>(9));

         glEnableVertexAttribArray(0);
         glBindVertexArray(0);
      }

      renderables_.emplace_back(std::move(r));
      model.renderables.push_back(renderables_.size() - 1);
      ++shapeIndex;

      std::cout << "bmin = " << model.bb.bmin[0] << ", " << model.bb.bmin[1] << ", " << model.bb.bmin[2] << '\n';
      std::cout << "bmax = " << model.bb.bmax[0] << ", " << model.bb.bmax[1] << ", " << model.bb.bmax[2] << '\n';
		std::cout << "maxextent = " << model.bb.maxExtent() << '\n';
   }

   return model;
}
