#include "Camera.h"

#include <SDL2/SDL.h>

#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

//==============================================================================
Camera::Camera()
    : position_(1.2f, 1.2f, 1.2f)
    , target_(0.0f)
    , fieldOfView_(glm::radians(60.0f))
    , nearClip_(0.1f)
    , farClip_(512.0f)
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
  return glm::perspective(fieldOfView_, 800.0f / 600.0f, nearClip_, farClip_);
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
void Camera::orbitController(const SDL_Event &e)
{
  glm::vec3 translation = target_;
  position_ = position_ - translation;
  target_ = target_ - translation;

  switch (e.type)
  {
  case SDL_MOUSEMOTION:
    if (e.motion.state & SDL_BUTTON(1))
    {
      auto a1   = glm::radians(e.motion.xrel * -0.5f);
      auto a2   = glm::radians(e.motion.yrel * 0.5f);
      position_ = glm::rotate(glm::mat4(), a1, glm::vec3(0, 1, 0)) * glm::vec4(position_, 0.0f);
      auto axis = glm::normalize(glm::cross(position_ - target_, glm::vec3(0, 1, 0)));
      position_ = glm::rotate(glm::mat4(), a2, axis) * glm::vec4(position_, 0.0f);
      //auto q = glm::angleAxis(angle, axis);
    }
    break;
  case SDL_MOUSEWHEEL:
    position_ = position_ + -static_cast<float>(e.wheel.y) * 0.5f * glm::normalize(position_);
    break;
  }

  position_ = position_ + translation;
  target_ = target_ + translation;
}
