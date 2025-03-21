#include "Ball.hpp"
#include "Paddle.hpp"
#include <SDL.h>

using namespace glm;

extern int leftScore;
extern int rightScore;


void Ball::Start() 
{
    name = "Ball";
    position = vec3(window->GetScreenWidth() * 0.5f, window->GetScreenHeight() * 0.5f, 0.0f);


    // scale of the ball is defined here
    scale = vec3(15.0f, 15.0f, 0.0f);
}

void Ball::Update(float _dt) 
{
    if (isRaining) 
    {
        position.y -= fallSpeed * _dt; // Fall downward
        fallSpeed += 20.0f * _dt; // Gradually increase speed

        if (position.y < 0) 
        {
            world->Destroy(this); // Remove the ball when it reaches the bottom
        }
        return;
    }

    if (dir == vec2(0.0f))
    {
        isMoving = false;
        if (inputManager->GetKey(SDL_SCANCODE_SPACE))
        {
            isMoving = true;
            vec2 directions[] = {vec2(1.0f, 1.0f), vec2(1.0f, -1.0f), vec2(-1.0f, 1.0f), vec2(-1.0f, -1.0f)};
            dir = directions[rand()%4];
        }
    }
    
    if (position.y > window->GetScreenHeight() - (scale.y * 0.5f)) {
        position.y = window->GetScreenHeight() - (scale.y * 0.5f);
        dir.y = abs(dir.y) * -1.0f;
    }
    if (position.y < scale.y * 0.5f) {
        position.y = scale.y * 0.5f;
        dir.y = abs(dir.y);
    }

    // detect score
    if (position.x > window->GetScreenWidth() - (scale.x * 0.5f)) {
        position = vec3(window->GetScreenWidth()*0.5f, window->GetScreenHeight()*0.5f, 0.0f);
        dir = vec2(0.0f);
        rightScore++;
        Ball *ball = world ->FindByName<Ball>("Ball");
        ball->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //ball back to white
        ball->speed = 100.0f; //changes  ball speed back to 100.0f

        if (rightScore >= 5) {
            for (int i = 0; i < 20; i++) { // Spawn 20 balls
                Ball *newBall = world->Instantiate<Ball>();
                newBall->position = vec3(rand() % window->GetScreenWidth(), window->GetScreenHeight(), 0.0f);
                newBall->dir = vec2(0.0f, -1.0f); // Falling direction
                newBall->speed = 200.0f; // Falling speed
                newBall->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red (right player won)
            }
        }
    }
    
    if (position.x < scale.x * 0.5f) {
        position = vec3(window->GetScreenWidth()*0.5f, window->GetScreenHeight()*0.5f, 0.0f);
        dir = vec2(0.0f);
        leftScore++;
        Ball *ball = world ->FindByName<Ball>("Ball");
        ball->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f); //ball back to white
        ball->speed = 100.0f; //changes  ball speed back to 100.0f
        if (leftScore >= 5) {
            for (int i = 0; i < 20; i++) { // Spawn 20 balls
                Ball *newBall = world->Instantiate<Ball>();
                newBall->position = vec3(rand() % window->GetScreenWidth(), window->GetScreenHeight(), 0.0f);
                newBall->dir = vec2(0.0f, -1.0f); // Falling direction
                newBall->speed = 200.0f; // Falling speed
                newBall->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue (left player won)
            }
        }
    }

    // detect if ball hits left paddle
    Paddle* leftPaddle = world->FindByName<Paddle>("LeftPaddle"); 
    if (EntityOverlap2D(*this ,*leftPaddle)) {
        dir.x = abs(dir.x);
        leftPaddle->isHit = true;
        Ball *ball = world ->FindByName<Ball>("Ball");
        ball->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f);
        ball->speed = speed * 1.2f; //increases balls speed each time it hits a paddle
    }

    // detect if ball hits right paddle
    Paddle* rightPaddle = world->FindByName<Paddle>("RightPaddle"); 
    if (EntityOverlap2D(*this ,*rightPaddle)) {
        dir.x = abs(dir.x) * -1.0f;
        rightPaddle->isHit = true;
        Ball *ball = world ->FindByName<Ball>("Ball");
        ball->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f);
        ball->speed = speed * 1.2f; //increases balls speed each time it hits a paddle
    }
    //Canis::Log(std::to_string(speed));

    if (dir != vec2(0.0f))
        position += vec3(dir.x, dir.y, 0.0f) * speed * _dt;
}


void Ball::Draw() {mat4 transform = mat4(1.0f);
    transform = translate(transform, position);
    transform = glm::scale(transform, scale);

    // set shader variables
    shader.SetVec4("COLOR", color);
    shader.SetMat4("TRANSFORM", transform);
    shader.SetBool("useTexture", false);
    shader.SetInt("texture1", 0);
    shader.SetBool("isScrolling", false);
}

void Ball::OnDestroy() {
    
    
}

vec3 Ball::GetBallPos() {
    return position;
}
bool Ball::GetIsMoving() {
    return isMoving;
}
float Ball::GetBallSpeed() {
    return speed;
}