#pragma once

#include "BoundingBox.h"
#include "Camera.h"
#include "Material.h"
#include "Model.h"
#include "ShaderProgram.h"
#include "Texture.h"

#include "extern/tiny_obj_loader.h"

#include <list>
#include <unordered_map>
#include <vector>

struct Renderable
{
   Renderable();

   void destroy();

   GLuint vb;
   GLuint vao;
   int numTriangles;
   std::size_t materialId;
   std::size_t shaderProgramId;
};

struct RenderEntity
{
   std::size_t modelSurfaceId;
   glm::mat4 matrix;
};

class Renderer
{
public:
   Renderer();

   ~Renderer();

   void render();

   void initialize();

   Model loadObjAndConvert(
      const std::string& filename, const std::string& basedir);

   Camera& camera() { return camera_; }

private:
   void initializeShaders();

   Camera camera_;

   std::vector<ShaderProgram> shaderPrograms_;
   std::vector<Model> modelSurfaces_;
   std::vector<Renderable> renderables_;
   std::vector<Material> materials_;
   std::list<RenderEntity> renderEntities_;
   std::unordered_map<std::string, Texture> textures_;

   friend class ObjModelLoader;
};
