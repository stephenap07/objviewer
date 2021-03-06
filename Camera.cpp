#include "Camera.h"

#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/quaternion.hpp>

//==============================================================================
Camera::Camera()
   : position_(0.0f, 0.0f, 3.0f)
   , target_(0.0f)
   , fieldOfView_(glm::radians(45.0f))
   , nearClip_(0.1f)
   , farClip_(512.0f)
   , width_(800.0f)
   , height_(600.0f)
   , orthoSize_(1.0f)
{
}

//==============================================================================
glm::vec3 Camera::direction() const
{
   return glm::normalize(target_ - position_);
}

//==============================================================================
glm::mat4 Camera::viewMatrix() const
{
   return glm::lookAt(position_, target_, glm::vec3(0, 1, 0));
}

//==============================================================================
glm::mat4 Camera::projectionMatrix() const
{
   return glm::perspective(fieldOfView_, width_ / height_, nearClip_, farClip_);
}

//==============================================================================
glm::mat4 Camera::orthographicMatrix() const
{
   return glm::ortho(0.0f, orthoSize_, orthoSize_ * (height_ / width_), 0.0f, nearClip_, farClip_);
}

//==============================================================================
glm::mat4 Camera::viewProjectionMatrix() const
{
   return viewMatrix() * projectionMatrix();
}

//==============================================================================
void Camera::normalizeTarget()
{
   target_ = glm::normalize(position_ + glm::normalize(target_ - position_));
}

//==============================================================================
void Camera::orbitController(const SDL_Event& e)
{
   glm::vec3 translation = target_;
   position_ = position_ - translation;
   target_ = target_ - translation;

   switch (e.type)
   {
   case SDL_MOUSEMOTION:
      if (e.motion.state & SDL_BUTTON(1))
      {
         const auto up = glm::vec3(0.0f, 1.0f, 0.0f);
         const auto dx = glm::radians(e.motion.xrel * -0.5f);
         const auto dy = glm::radians(e.motion.yrel * 0.5f);
         const auto axis = glm::normalize(glm::cross(position_ - target_, up));
         position_ = glm::rotate(glm::angleAxis(dx, up), position_);
         position_ = glm::rotate(glm::angleAxis(dy, axis), position_);
      }
      break;
   case SDL_MOUSEWHEEL:
      position_ = position_ + -static_cast<float>(e.wheel.y) * 0.5f * glm::normalize(position_);
      orthoSize_ = orthoSize_ + -static_cast<float>(e.wheel.y) * 0.5f;
      break;
   }

   position_ = position_ + translation;
   target_ = target_ + translation;
}
