//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef INPUTSTATE_HPP
#define INPUTSTATE_HPP



enum class DirectionalKey {
    Up,
    Down,
    Left,
    Right
};

struct InputState {
    bool IsUp(DirectionalKey key) {
        switch (key) {
            case DirectionalKey::Up:
                return !up;
            case DirectionalKey::Down:
                return !down;
            case DirectionalKey::Left:
                return !left;
            case DirectionalKey::Right:
                return !right;
        }
    }

    bool IsDown(DirectionalKey key) {
        switch (key) {
            case DirectionalKey::Up:
                return up;
            case DirectionalKey::Down:
                return down;
            case DirectionalKey::Left:
                return left;
            case DirectionalKey::Right:
                return right;
        }
    }

    bool IsPressed(DirectionalKey key) {
        switch (key) {
            case DirectionalKey::Up:
                return up && !previousUp;
            case DirectionalKey::Down:
                return down && !previousDown;
            case DirectionalKey::Left:
                return left && !previousLeft;
            case DirectionalKey::Right:
                return right && !previousRight;
        }
    }

    bool IsReleased(DirectionalKey key) {
        switch (key) {
            case DirectionalKey::Up:
                return !up && previousUp;
            case DirectionalKey::Down:
                return !down && previousDown;
            case DirectionalKey::Left:
                return !left && previousLeft;
            case DirectionalKey::Right:
                return !right && previousRight;
        }
    }

    bool previousLeft { false };
    bool left { false };

    bool previousRight { false };
    bool right { false };

    bool previousUp { false };
    bool up { false };

    bool previousDown { false };
    bool down { false };
};

#endif //INPUTSTATE_HPP
