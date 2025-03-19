#pragma once

#include "Entity.hpp"

#include "World.hpp"

class Paddle : public Entity {
public:
    void Start();
    void Update(float _dt);
    void Draw();
    void OnDestroy();
    float EaseOutCircle(float _time);

    float speed = 500.0f;
    glm::vec4 color = glm::vec4(1.0f);
    bool isHit = false;

    float hitTime = 0.0f;
};