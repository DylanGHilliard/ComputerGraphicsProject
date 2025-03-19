#include "Paddle.hpp"

#include "World.hpp"

using namespace glm;

void Paddle::Start() {
    scale = vec3(20.0f, 100.0f, 0.0f);
    isHit = false;
}

void Paddle::Update(float _dt) {
    vec2 dir;

    if (name == "LeftPaddle")
    {
        dir.y += inputManager->GetKey(SDL_SCANCODE_W);
        dir.y += inputManager->GetKey(SDL_SCANCODE_S) * -1;
    }
    else if (name == "RightPaddle")
    {
        dir.y += inputManager->GetKey(SDL_SCANCODE_UP);
        dir.y += inputManager->GetKey(SDL_SCANCODE_DOWN) * -1;
    }

    position.y += dir.y * speed * _dt;

    if (position.y > window->GetScreenHeight() - (scale.y * 0.5f))
        position.y = window->GetScreenHeight() - (scale.y * 0.5f);
    if (position.y < scale.y * 0.5f)
        position.y = scale.y * 0.5f;

    if (isHit)
    {
        float bounceTime = 0.6f;  
        hitTime += _dt;
        vec3 orgScale = {20.0f, 100.0f, 0.0f};
        
        if (hitTime <= bounceTime)
        {
            float progress = hitTime / bounceTime;
            float bounce = EaseOutCircle(progress);
            float scaleMultiplier = 1.0f + (0.2f * (1.0f - bounce)); 
            this->scale = orgScale * scaleMultiplier;
        }
        else
        {
            isHit = false;
            hitTime = 0.0f;
            scale = orgScale;
        }
    }
}

void Paddle::Draw() {
    mat4 transform = mat4(1.0f);
    transform = translate(transform, position);
    transform = glm::scale(transform, scale);


    // set shader variables
    shader.SetVec4("COLOR", color);
    shader.SetMat4("TRANSFORM", transform);
    shader.SetBool("useTexture", false);
    shader.SetBool("isScrolling", false);
}

void Paddle::OnDestroy() {
    
}


float Paddle::EaseOutCircle(float x)
{
    float c1 = 1.70158f;
    float c2 = c1 * 1.525f;

    float x1 = x - 1.0f;
    return x1 * x1 * ((c2 + 1.0f) * x1 + c2) + 1.0f;
}
