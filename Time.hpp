//
// Created by Gaëtan Blaise-Cazalet on 19/11/2024.
//

#ifndef TIME_HPP
#define TIME_HPP

/*
 * Hold time related functions.
 * In charge of computing the delta time and ensure smooth game ticking.
 */
class Time {
public:
    // Compute delta time as the number of milliseconds since last frame
    float ComputeDeltaTime();

    // Wait if the game run faster than the decided FPS
    void DelayTime();

private:
    const static int FPS = 60;
    const static int frameDelay = 1000 / FPS;

    // Time in milliseconds when frame starts
    unsigned int frameStart { 0 };

    // Last frame start time in milliseconds
    unsigned int lastFrame { 0 };

    // Time it tooks to run the loop. Used to cap framerate.
    unsigned int frameTime { 0 };
};



#endif //TIME_HPP
