#ifdef _WIN32
#include <windows.h>
#endif

#include <iostream>

#include <SDL.h>
#include <glm/glm.hpp>
#include <GL/glew.h>

#include "Canis/Canis.hpp"
#include "Canis/IOManager.hpp"
#include "Canis/FrameRateManager.hpp"

#include "Entity.hpp"
#include "Ball.hpp"
#include "Paddle.hpp"
#include "Background.hpp"
#include "Particle.hpp"

// git restore .
// git fetch
// git pull

// score tracking variables
int leftScore = 0; 
int rightScore = 0;
float rainStartTime = -1.0f;

// move out to external class
unsigned int vertexShader;
unsigned int VBO, VAO, EBO;

void InitModel();

#ifdef _WIN32
#define main SDL_main
extern "C" int main(int argc, char *argv[])
#else
int main(int argc, char *argv[])
#endif
{
    Canis::Init();
    Canis::Window window;

    // *** CHANGES - UPDATED to dynamically change window size based on display size so it works with my 1440p
    SDL_DisplayMode displayMode;
    if (SDL_GetCurrentDisplayMode(0, &displayMode) == 0) {

        float windowSize = 800;
        float windowHeight = (windowSize * ((float)displayMode.h / 1080.0f));
        float windowWidth  = (windowSize * ((float)displayMode.w / 1920.0f));

        window.Create("Computer Graphics 2025", windowWidth, windowHeight, 0);

    }
    // *** END OF CHANGES


    Canis::InputManager inputManager;
    Canis::FrameRateManager frameRateManager;
    frameRateManager.Init(60.0f);
    float deltaTime = 0.0f;
    float fps = 0.0f;

    Canis::Shader spriteShader;
    spriteShader.Compile("assets/shaders/sprite.vs", "assets/shaders/sprite.fs");
    spriteShader.AddAttribute("aPos");
    spriteShader.AddAttribute("aUV");
    spriteShader.Link();

    Canis::Shader circleShader;
    circleShader.Compile("assets/shaders/sprite.vs", "assets/shaders/circle.fs");
    circleShader.AddAttribute("aPos");
    circleShader.AddAttribute("aUV");
    circleShader.Link();

    InitModel();

    //Canis::GLTexture texture = Canis::LoadImageGL("assets/textures/ForcePush.png", true);
    Canis::GLTexture texture1 = Canis::LoadImageGL("assets/textures/TilePattern.png", true);

    int textureSlots = 0;

    glGetIntegerv(GL_MAX_TEXTURE_IMAGE_UNITS, &textureSlots);

    Canis::Log(std::to_string(textureSlots));

    spriteShader.SetInt("texture1", 0);
    spriteShader.SetInt("texture2", 1);

    //glActiveTexture(GL_TEXTURE0 + 0);
    //glBindTexture(GL_TEXTURE_2D, texture.id);

    glActiveTexture(GL_TEXTURE1);                    
    glBindTexture(GL_TEXTURE_2D, texture1.id);


    World world;
    world.VAO = VAO;
    world.window = &window;
    world.inputManager = &inputManager;

    {
        Background *background = world.Instantiate<Background>();
        background->shader = spriteShader;
        background->texture = texture1;
        background->shader.SetBool("useTexture", true);
        background->shader.SetBool("isScrolling", true);
    
    }
    
    {
        Ball *ball = world.Instantiate<Ball>();
        ball->shader = circleShader;
        ball->name = "Ball";
        ball->color = glm::vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }

    {
        Paddle *paddle = world.Instantiate<Paddle>();
        paddle->shader = spriteShader;
        //paddle->texture = texture;
        paddle->name = "RightPaddle";
        paddle->position = glm::vec3(window.GetScreenWidth() - (10.0f*0.5f), window.GetScreenHeight() * 0.5f, 0.0f);
        paddle->color = glm::vec4(1.0f, 0.0f, 0.0f, 1.0f); // Red
        paddle->shader.SetBool("isScrolling", false);
    }

    {
        Paddle *paddle = world.Instantiate<Paddle>();
        paddle->shader = spriteShader;
        //paddle->texture = texture;
        paddle->name = "LeftPaddle";
        paddle->position = glm::vec3(10.0f*0.5f, window.GetScreenHeight() * 0.5f, 0.0f);
        paddle->color = glm::vec4(0.0f, 0.0f, 1.0f, 1.0f); // Blue
        paddle->shader.SetBool("isScrolling", false);
    }

    Uint32 lastParticleSpawnTime = SDL_GetTicks();
    while (inputManager.Update(window.GetScreenWidth(), window.GetScreenHeight()))
    {
        deltaTime = frameRateManager.StartFrame();
        glClearColor( 1.0f, 1.0f, 1.0f, 1.0f);

        glClear(GL_COLOR_BUFFER_BIT);

        using namespace glm;

        mat4 projection = ortho(0.0f, (float)window.GetScreenWidth(), 0.0f, (float)window.GetScreenHeight(), 0.001f, 100.0f);
        
        mat4 view = mat4(1.0f);
        view = translate(view, vec3(0.0f, 0.0f, 0.5f));
        view = inverse(view);

        world.Update(view, projection, deltaTime);

        // update the window title bar with the current score
        window.SetWindowName("Pong - Score: Blue " + std::to_string(rightScore) + " | Red " + std::to_string(leftScore));

        // Particle spawn every 0.4 second
        //################################################



        Ball* ball = world.FindByName<Ball>("Ball");

        if (ball) {
            bool theBallIsMoving = ball->GetIsMoving();
            // i dont like having another nested if statement but best fix i can think 
            // of for segmentation fault error with theBallIsMoving being initalized right after the ball pointer assignment
            if (theBallIsMoving) { 
                float ballSpeed = ball->GetBallSpeed();
                float spawnInterval = 40000.0f / ballSpeed;
        
                Uint32 currentTime = SDL_GetTicks();
                if (currentTime - lastParticleSpawnTime >= spawnInterval) {
                    Particle *particle = world.Instantiate<Particle>();
                    particle->shader = circleShader;
                    particle->color = glm::vec4(1.0f, 1.0f, 1.0f, 0.3f);
                    lastParticleSpawnTime = currentTime;
                }
            }
        }

        //################################################

        if (leftScore == 5 || rightScore == 5) 
        {
            const float SPAWN_INTERVAL = 0.1f; // CONTROLS TIME BALLS SPAWN BETWEEN EACH OTHER
            static float lastSpawnTime = 0.0f; 
        
            float currentTime = SDL_GetTicks() / 1000.0f; // the divide converts the miliseconds to seconds
        
            if (rainStartTime < 0) 
                rainStartTime = currentTime; // start timer
        
            Ball* gameBall = world.FindByName<Ball>("Ball");
            if (gameBall)
                world.Destroy(gameBall); // remove main ball
                
            static bool paddlesRemoved = false; // Ensure paddles are removed only once

            if (!paddlesRemoved) {
                Paddle* leftPaddle = world.FindByName<Paddle>("LeftPaddle");
                Paddle* rightPaddle = world.FindByName<Paddle>("RightPaddle");

                if (leftPaddle) world.Destroy(leftPaddle);
                if (rightPaddle) world.Destroy(rightPaddle);

                paddlesRemoved = true;
    }

            
           
            if (currentTime - lastSpawnTime >= SPAWN_INTERVAL) // logic for the delay
            {
                lastSpawnTime = currentTime; // this will reset the timer
        
                // random fields
                Ball* rainBall = world.Instantiate<Ball>();
                const float MAX_SCALE = 20.0f;
                const float MIN_SCALE = 10.0f;
                const float MAX_SCREEN_HEIGHT = window.GetScreenHeight();
                const float MAX_SCREEN_WIDTH = window.GetScreenWidth();
                const float HEIGHT_OFFSET = 0;
        
                float getRandomNumberForScales =           
                       MIN_SCALE + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MAX_SCALE - MIN_SCALE)));
        
                float getRandomNumberForHeightPositions =  
                       (MAX_SCREEN_HEIGHT - HEIGHT_OFFSET) + static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / (MAX_SCREEN_HEIGHT - (MAX_SCREEN_HEIGHT - HEIGHT_OFFSET))));
        
                float getRandomNumberForWidthPositions =   
                       static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / MAX_SCREEN_WIDTH));
        
                rainBall->shader = circleShader;
                rainBall->name = "RainBall";
                rainBall->position = glm::vec3(getRandomNumberForWidthPositions, getRandomNumberForHeightPositions, 0.0f);
                rainBall->scale = glm::vec3(getRandomNumberForScales, getRandomNumberForScales, 0.0f);
                rainBall->isRaining = true;
                rainBall->fallSpeed = 100.0f;
                rainBall->color = (leftScore == 5) ? glm::vec4(1, 0, 0, 1) : glm::vec4(0, 0, 1, 1); 
            }
        }

        if (rainStartTime > 0 && (SDL_GetTicks() / 1000.0f - rainStartTime) >= 10.0f) 
        {
            return 0; // exit the program after 10 seconds
        }

        window.SwapBuffer();

        fps = frameRateManager.EndFrame();
    }

    return 0;
}

void InitModel()
{
    float vertices[] = {
        // position         // uv
        0.5f, 0.5f, 0.0f, 1.0f, 1.0f,   // top right
        0.5f, -0.5f, 0.0f, 1.0f, 0.0f,  // bottom right
        -0.5f, -0.5f, 0.0f, 0.0f, 0.0f, // bottom left
        -0.5f, 0.5f, 0.0f, 0.0f, 1.0f,  // top left
    };

    unsigned int indices[] = {
        0, 1, 3,
        1, 2, 3};

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void *)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}