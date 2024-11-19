//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE01CLEAR_HPP
#define SCENE01CLEAR_HPP

#include "Scene.hpp"

class Scene01Clear : public Scene {
public:
    void Load(Renderer& renderer) override;
    bool Update(float dt) override;
    void Draw(Renderer& renderer) override;
    void Unload() override;

private:
    InputState inputState;
};



#endif //SCENE01CLEAR_HPP
