//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE02TRIANGLE_HPP
#define SCENE02TRIANGLE_HPP
#include "Scene.hpp"

class Scene02Triangle : public Scene {
    void Load() override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload() override;
};



#endif //SCENE02TRIANGLE_HPP
