#pragma once

#include "Entity.hpp"
#include "World.hpp"

class Particle : public Entity {
    public:
        void Start() override;
        void Update(float deltaTime) override;
        void Draw() override;
        void OnDestroy() override;
    
    private:
        float size;
        float lifetime;
        bool locationAlreadySet;
    };