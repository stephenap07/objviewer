
#include "BoundingBox.h"

//==============================================================================
BoundingBox::BoundingBox()
{
   bmin[0] = bmin[1] = bmin[2] = std::numeric_limits<float>::max();
   bmax[0] = bmax[1] = bmax[2] = -std::numeric_limits<float>::max();
}

//==============================================================================
glm::vec3 BoundingBox::extent() const
{
   return (bmax - bmin) / 2.0f;
}

//==============================================================================
float BoundingBox::maxExtent() const
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

//==============================================================================
glm::vec3 BoundingBox::centerTranslate() const
{
   return 0.5f * glm::vec3(bmax[0] + bmin[0], bmax[1] + bmin[1], bmax[2] + bmin[2]);
}
