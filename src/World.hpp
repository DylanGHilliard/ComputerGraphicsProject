#pragma once

#include <vector>

#include <SDL.h>
#include <GL/glew.h>

#include "Entity.hpp"

class World {
public:
    unsigned int VAO;
    Canis::Window *window;
    Canis::InputManager *inputManager;

    std::vector<Entity*> entities = {};

    template<typename T>
    T* Instantiate() {
        T* entity = new T;
        entity->window = window;
        entity->inputManager;
        entity->world = this;
        entity->Start();
        entities.push_back((Entity*)entity);
        return entity;
    }

    void Update(glm::mat4 _view, glm::mat4 _projection, float _dt) {
        for(Entity* e : entities)
        {
            

            e->shader.Use();
            e->shader.SetFloat("TIME", SDL_GetTicks() / 1000.0f);
            e->shader.SetMat4("PROJECTION", _projection);
            e->shader.SetMat4("VIEW", _view);

            e->Draw();

            glBindVertexArray(VAO);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
            glBindVertexArray(0);
            e->shader.UnUse();

            e->Update(_dt);
        }
    }

    void Destroy(Entity *_entity) {
        _entity->OnDestroy();

        for(int i = 0; i < entities.size(); i++)
        {
            if (_entity == entities[i])
            {
                entities.erase(entities.begin() + i);
                break;
            }
        }

        delete _entity;
    }

    template<typename T>
    T* FindByName(std::string _name) {
        for(Entity* e : entities)
            if (e->name == _name)
                return (T*)e;

        return (T*)nullptr;
    }

    Entity* FindEntityByName(std::string _name) {
        return FindByName<Entity>(_name);
    }
};