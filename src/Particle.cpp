#include "Ball.hpp"
#include <SDL.h>
#include "Particle.hpp"
#include "Canis/Canis.hpp"
#include <cmath>

using namespace glm;

void Particle::Start() 
{
    name = "Particle";
    size = 5.0f;
    scale = vec3(size, size, 0.0f);
    lifetime = 2.0f;
    locationAlreadySet = false;
}

void Particle::Update(float deltaTime)
{
    lifetime -= deltaTime;
    size += 0.3f; 
    scale = vec3(size, size, 0.0f);
    if (lifetime <= 0.0f) 
    {
        world->Destroy(this);
        return;
    }

    if (locationAlreadySet) return;

    Ball* ball = world->FindByName<Ball>("Ball"); 
    if (ball != nullptr) {
        position = ball->GetBallPos();
        locationAlreadySet = true;
    }
}

void Particle::Draw() {
    mat4 transform = mat4(1.0f);
    transform = translate(transform, position);
    transform = glm::scale(transform, scale);

    shader.SetVec4("COLOR", color);
    shader.SetMat4("TRANSFORM", transform);
    shader.SetBool("useTexture", false);
    shader.SetInt("texture1", 0);
    shader.SetBool("isScrolling", false);
}

void Particle::OnDestroy() {
}