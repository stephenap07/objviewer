#include "ObjModelLoader.h"

#include "Renderer.h"

#include <glm/glm.hpp>
#include <glm/gtx/normal.hpp>

#include <algorithm>
#include <iostream>

//==============================================================================
ObjModelLoader::ObjModelLoader(Renderer& aRenderer, const std::string& aFilename, const std::string& aBasePath)
   : renderer_(aRenderer)
   , filename_(aFilename)
   , basePath_(aBasePath)
{
}

//==============================================================================
Model ObjModelLoader::getModel()
{
   // Default material
   data_.loadObj(filename_, basePath_);
   data_.emplaceMaterialsBackToVector(renderer_.materials_);
   renderer_.materials_.emplace_back(tinyobj::material_t());
   loadDiffuseTextures();
   convertShapeToRenderable();
   //std::cout << "maxextent = " << model.boundingBox.maxExtent() << '\n';
   return model_;
}

//==============================================================================
void ObjModelLoader::convertShapeToRenderable()
{
   for (const auto& shape : data_.shapes_)
   {
      Renderable renderable;
      std::vector<float> vb;
      int numMaterialsPerShape(0);

      for (std::size_t f = 0; f < shape.mesh.indices.size() / 3; ++f)
      {
         tinyobj::index_t idx0 = shape.mesh.indices[3 * f + 0];
         tinyobj::index_t idx1 = shape.mesh.indices[3 * f + 1];
         tinyobj::index_t idx2 = shape.mesh.indices[3 * f + 2];

         int currentMaterialId = shape.mesh.material_ids[f];
         if (currentMaterialId < 0 || currentMaterialId >= static_cast<int>(renderer_.materials_.size()))
         {
            // Set the material id to the default material.
            currentMaterialId = renderer_.materials_.size() - 1;
         }
         else if (numMaterialsPerShape == 0)
         {
            numMaterialsPerShape = 1;
         }

         glm::vec3 diffuse;
         for (std::size_t i = 0; i < 3; ++i)
         {
            diffuse[i] = renderer_.materials_[currentMaterialId].diffuse[i];
         }

         // Get the number of materials per renderable object.
         if (renderable.materialId != renderer_.materials_.size() - 1 && renderable.materialId != currentMaterialId)
         {
            ++numMaterialsPerShape;
         }

         renderable.materialId = currentMaterialId;

         float tc[3][2];
         if (data_.attrib_.texcoords.size())
         {
            assert(data_.attrib_.texcoords.size() > 2 * idx0.texcoord_index + 1);
            assert(data_.attrib_.texcoords.size() > 2 * idx1.texcoord_index + 1);
            assert(data_.attrib_.texcoords.size() > 2 * idx2.texcoord_index + 1);

            // Flip the Y coordinate
            tc[0][0] = data_.attrib_.texcoords[2 * idx0.texcoord_index];
            tc[0][1] = 1.0f - data_.attrib_.texcoords[2 * idx0.texcoord_index + 1];
            tc[1][0] = data_.attrib_.texcoords[2 * idx1.texcoord_index];
            tc[1][1] = 1.0f - data_.attrib_.texcoords[2 * idx1.texcoord_index + 1];
            tc[2][0] = data_.attrib_.texcoords[2 * idx2.texcoord_index];
            tc[2][1] = 1.0f - data_.attrib_.texcoords[2 * idx2.texcoord_index + 1];
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

            v[0][k] = data_.attrib_.vertices[3 * f0 + k];
            v[1][k] = data_.attrib_.vertices[3 * f1 + k];
            v[2][k] = data_.attrib_.vertices[3 * f2 + k];

            model_.boundingBox.bmin[k] = std::min(v[0][k], model_.boundingBox.bmin[k]);
            model_.boundingBox.bmin[k] = std::min(v[1][k], model_.boundingBox.bmin[k]);
            model_.boundingBox.bmin[k] = std::min(v[2][k], model_.boundingBox.bmin[k]);
            model_.boundingBox.bmax[k] = std::max(v[0][k], model_.boundingBox.bmax[k]);
            model_.boundingBox.bmax[k] = std::max(v[1][k], model_.boundingBox.bmax[k]);
            model_.boundingBox.bmax[k] = std::max(v[2][k], model_.boundingBox.bmax[k]);
         }

         // Normals
         glm::vec3 n[3];
         if (data_.attrib_.normals.size() > 0)
         {
            int f0 = idx0.normal_index;
            int f1 = idx1.normal_index;
            int f2 = idx2.normal_index;

            assert(f0 >= 0);
            assert(f1 >= 0);
            assert(f2 >= 0);

            for (int k = 0; k < 3; k++)
            {
               n[k] = glm::vec3(data_.attrib_.normals[3 * f0 + k],
                                data_.attrib_.normals[3 * f1 + k],
                                data_.attrib_.normals[3 * f2 + k]);
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

      renderable.vb = 0;
      renderable.numTriangles = 0;

      // Right now per-face material rendering isn't supported.
      // It can possibly be supported through some kind of surface soup? So group vertices up by material?
      //
      /*
      if (s.mesh.material_ids.size() > 0 && s.mesh.material_ids.size() > shapeIndex && s.mesh.material_ids[shapeIndex] > -1)
      {
         renderable.materialId = s.mesh.material_ids[shapeIndex] + matIndex;
      }
      else
      {
         // Set to default material
         renderable.materialId = materials_.size() - 1 + matIndex;
      }
      */

      if (vb.size() > 0)
      {
         glGenVertexArrays(1, &renderable.vao);
         glBindVertexArray(renderable.vao);

         glGenBuffers(1, &renderable.vb);
         glBindBuffer(GL_ARRAY_BUFFER, renderable.vb);
         glBufferData(GL_ARRAY_BUFFER, vb.size() * sizeof(float), &vb.at(0), GL_STATIC_DRAW);
         const std::size_t numElements(11);
         renderable.numTriangles = vb.size() / numElements / 3;

         const auto stride = util::stride<GLfloat>(numElements);

         // Determine the shader based on the material
         if (renderer_.textures_.find(renderer_.materials_[renderable.materialId].diffuseTexname) == renderer_.textures_.end())
         {
            // Use the polygon shader
            renderable.shaderProgramId = 0;
         }
         else
         {
            // Use the texture shader
            renderable.shaderProgramId = 1;
         }

         auto& shader = renderer_.shaderPrograms_[renderable.shaderProgramId];
         shader.enableVertexAttrib("position", 3, GL_FLOAT, stride, 0);
         shader.enableVertexAttrib("normal", 3, GL_FLOAT, stride, util::toGlPtr<GLfloat>(3));
         shader.enableVertexAttrib("diffuse", 3, GL_FLOAT, stride, util::toGlPtr<GLfloat>(6));
         shader.enableVertexAttrib("texcoord", 2, GL_FLOAT, stride, util::toGlPtr<GLfloat>(9));

         glEnableVertexAttribArray(0);
         glBindVertexArray(0);
      }

      renderer_.renderables_.emplace_back(renderable);
      model_.renderables.push_back(renderer_.renderables_.size() - 1);
   }
}

//==============================================================================
void ObjModelLoader::logTinyObjInfo() const
{
   std::cout << "# of verticies " << data_.attrib_.vertices.size() / 3 << '\n';
   std::cout << "# of normals " << data_.attrib_.normals.size() / 3 << '\n';
   std::cout << "# of texcoords " << data_.attrib_.texcoords.size() / 2 << '\n';
   std::cout << "# of materials " << renderer_.materials_.size() << '\n';
   std::cout << "# of shapes " << data_.shapes_.size() << '\n';
}

//==============================================================================
void ObjModelLoader::logTinyObjLoadError(const std::string& aError, const bool& aStatus) const
{
   if (!aError.empty())
   {
      std::cerr << aError << '\n';
   }
   if (!aStatus)
   {
      std::cerr << "Failed to load " << filename_ << '\n';
   }
}

//==============================================================================
void ObjModelLoader::loadDiffuseTextures()
{
   for (const auto& material : renderer_.materials_)
   {
      //materials_.emplace_back(m);
      if (material.diffuseTexname.length())
      {
         std::string texturePath = material.diffuseTexname;
         std::replace(texturePath.begin(), texturePath.end(), '\\', '/');
         std::cout << "Inserting texture " << basePath_ + texturePath << '\n';
         renderer_.textures_.emplace(material.diffuseTexname, Texture(basePath_ + texturePath));
      }
   }
}

//==============================================================================
bool TinyObjLoadData::loadObj(const std::string& aFilename, const std::string& aBasePath)
{
   status_ = tinyobj::LoadObj(&attrib_, &shapes_, &materials_, &err_, aFilename.c_str(), aBasePath.c_str());
   logError();
   return status_;
}

//==============================================================================
void TinyObjLoadData::logError() const
{
   if (!err_.empty())
   {
      std::cerr << err_ << '\n';
   }
   if (!status_)
   {
      std::cerr << "Failed to load obj\n";
   }
}

//==============================================================================
void TinyObjLoadData::emplaceMaterialsBackToVector(std::vector<Material>& aMaterials) const
{
   for (const auto& material : materials_)
   {
      aMaterials.emplace_back(material);
   }
}
