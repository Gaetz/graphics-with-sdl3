//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef SCENE_HPP
#define SCENE_HPP

class Renderer;

class Scene {
public:
    virtual ~Scene() {}
    void virtual Load() = 0;
    bool virtual Update(float dt) = 0;
    void virtual Draw(Renderer& renderer) = 0;
    void virtual Unload() = 0;
};

#endif //SCENE_HPP
