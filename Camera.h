#pragma once

#include <glm/glm.hpp>

union SDL_Event;

class Camera {
public:
  Camera ();

  glm::vec3 direction() const;
  glm::mat4 viewMatrix() const;
  glm::mat4 projectionMatrix() const;
  glm::mat4 viewProjectionMatrix() const;
  void normalizeTarget();
  void orbitController(const SDL_Event& e);

  void setPosition(glm::vec3 aValue) { position_ = aValue; }
  const glm::vec3 &position() const { return position_; }

  void setTarget(glm::vec3 aValue) { target_ = aValue; }
  const glm::vec3 &target() const { return target_; }

  void setFieldOfView(float aValue) { fieldOfView_ = aValue; }
  const float &fieldOfView() const { return fieldOfView_; }

  void setNearClip(float aValue) { nearClip_ = aValue; }
  const float &nearClip() const { return nearClip_; }

  void setFarClip(float aValue) { farClip_ = aValue; }
  const float farClip() const { return farClip_; }

private:
  glm::vec3 position_;
  glm::vec3 target_;
  float fieldOfView_;
  float nearClip_;
  float farClip_;
};
