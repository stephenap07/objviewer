#pragma once

#include <glm/glm.hpp>

class BoundingBox
{
public:
   BoundingBox();

   glm::vec3 extent() const;
   float maxExtent() const;
   glm::vec3 centerTranslate() const;

   glm::vec3 bmin;
   glm::vec3 bmax;
};
