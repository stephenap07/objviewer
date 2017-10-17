#include "Entity.h"

#include <glm/glm.hpp>

Entity::Entity()
   : scale_(glm::vec3(1, 1, 1))
   , rotate_(glm::vec3(0, 1, 0))
   , translate_(glm::vec3(0, 0, 0))
{
}

glm::mat4 Entity::srt() const
{
   return glm::mat4();
}
