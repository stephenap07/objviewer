#pragma once

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Entity
{
public:
   Entity();

   glm::mat4 srt() const;

   const glm::vec3& scale() const { return scale_; }
   void setScale(const glm::vec3 aValue) { scale_ = aValue; }

   const glm::vec3& rotate() const { return rotate_; }
   void setRotate(const glm::vec3 aValue) { rotate_ = aValue; }

   const glm::vec3& translate() const { return translate_; }
   void setTranslate(const glm::vec3 aValue) { translate_ = aValue; }

private:
   glm::vec3 scale_;
   glm::vec3 rotate_;
   glm::vec3 translate_;
};
