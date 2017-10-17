#pragma once

#include "Camera.h"
#include "ShaderProgram.h"
#include "Texture.h"
#include "extern/tiny_obj_loader.h"

#include <list>
#include <unordered_map>
#include <vector>

struct BoundingBox
{
   BoundingBox();
   float bmin[3];
   float bmax[3];

   float maxExtent() const
   {
      float m = 0.5f * (bmax[0] - bmin[0]);
      if (m < 0.5f * (bmax[1] - bmin[1]))
      {
         m = 0.5f * (bmax[1] - bmin[1]);
      }
      if (m < 0.5f * (bmax[2] - bmin[2]))
      {
         m = 0.5f * (bmax[2] - bmin[2]);
      }
      return m;
   }

   glm::vec3 centerTranslate() const
   {
      return glm::normalize(glm::vec3(-0.5 * (bmax[0] + bmin[0]),
         -0.5 * (bmax[1] + bmin[1]),
         -0.5 * (bmax[2] + bmin[2])));
   }
};

struct Renderable
{
   GLuint vb;
   GLuint vao;
   int numTriangles;
   std::size_t materialId;
};

struct ModelSurface
{
   BoundingBox bb;
   std::vector<int> renderables;
};

struct RenderEntity
{
   std::size_t modelSurfaceId;
   glm::mat4 matrix;
};

struct Material
{
   Material() = default;
   Material(const Material&) = default;
   Material(Material&&) = default;

   Material(const tinyobj::material_t& m)
      : ambient(glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]))
      , diffuse(glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]))
      , specular(glm::vec3(m.specular[0], m.specular[1], m.specular[2]))
      , transmittance(glm::vec3(m.transmittance[0], m.transmittance[1], m.transmittance[2]))
      , emission(glm::vec3(m.emission[0], m.emission[1], m.emission[2]))
      , shininess(m.shininess)
      , ior(m.ior)
      , dissolve(m.dissolve)
      , ambientTexname(m.ambient_texname)
      , diffuseTexname(m.diffuse_texname)
   {
   }

   glm::vec3 ambient;
   glm::vec3 diffuse;
   glm::vec3 specular;
   glm::vec3 transmittance;
   glm::vec3 emission;
   float shininess;
   float ior;
   float dissolve;

   std::string ambientTexname;
   std::string diffuseTexname;
   std::string specularTexname;
   std::string specularHighlightTexname;
   std::string bumpTexname;
   std::string displacementTexname;
   std::string alphaTexname;
};

class Renderer
{
public:
   Renderer();

   ~Renderer();

   void render();

   void initialize();

   ModelSurface loadObjAndConvert(const std::string& filename,
      const std::string& basedir);

   Camera& camera() { return camera_; }

   ShaderProgram& objProgram() { return objProgram_; }

private:
   void initializeShaders();

   Camera camera_;
   ShaderProgram objProgram_;

   std::vector<ModelSurface> modelSurfaces_;
   std::vector<Renderable> renderables_;
   std::vector<Material> materials_;
   std::list<RenderEntity> renderEntities_;
   std::unordered_map<std::string, Texture> textures_;
};
