#pragma once

#include "BoundingBox.h"

#include <vector>

class Model
{
public:
   BoundingBox boundingBox;
   std::vector<int> renderables;
};
