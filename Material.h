#pragma once

#include <glm/glm.hpp>

#include "extern/tiny_obj_loader.h"

#include <string>

class Material
{
public:

   Material() = default;
   Material(const Material&) = default;
   Material(Material&&) = default;

   Material(const tinyobj::material_t& m);

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
