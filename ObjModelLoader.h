#pragma once

#include "Material.h"
#include "Model.h"
#include "Texture.h"

#include "extern/tiny_obj_loader.h"

#include <string>
#include <unordered_map>
#include <vector>

class Renderer;

struct TinyObjLoadData
{
   bool status_ = false;
   std::string err_;
   tinyobj::attrib_t attrib_;
   std::vector<tinyobj::shape_t> shapes_;
   std::vector<tinyobj::material_t> materials_;

   bool loadObj(const std::string& aFilename, const std::string& aBasePath);

   void logError() const;

   void emplaceMaterialsBackToVector(std::vector<Material>& aMaterials) const;
};

class ObjModelLoader
{
public:
   ObjModelLoader(Renderer& aRenderer, const std::string& aFilename, const std::string& aBasePath);

   Model getModel();

private:

   // Loaders
   void convertShapeToRenderable();
   void loadDiffuseTextures();

   // Logging
   void logTinyObjInfo() const;
   void logTinyObjLoadError(const std::string& aError, const bool& aStatus) const;

   Renderer& renderer_;
   std::string filename_;
   std::string basePath_;

   // The generated model
   Model model_;
   TinyObjLoadData data_;
};
