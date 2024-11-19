//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#include "Scene02Triangle.hpp"
#include "Renderer.hpp"

void Scene02Triangle::Load() {

}

bool Scene02Triangle::Update(float dt) {
    return ManageInput();
}

void Scene02Triangle::Draw(Renderer& renderer) {
    renderer.Begin();

    renderer.End();
}

void Scene02Triangle::Unload() {

}