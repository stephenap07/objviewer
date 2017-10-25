#include "Material.h"

//==============================================================================
Material::Material(const tinyobj::material_t& m)
   : ambient(glm::vec3(m.ambient[0], m.ambient[1], m.ambient[2]))
   , diffuse(glm::vec3(m.diffuse[0], m.diffuse[1], m.diffuse[2]))
   , specular(glm::vec3(m.specular[0], m.specular[1], m.specular[2]))
   , transmittance(glm::vec3(
        m.transmittance[0], m.transmittance[1], m.transmittance[2]))
   , emission(glm::vec3(m.emission[0], m.emission[1], m.emission[2]))
   , shininess(m.shininess)
   , ior(m.ior)
   , dissolve(m.dissolve)
   , ambientTexname(m.ambient_texname)
   , diffuseTexname(m.diffuse_texname)
{
}
