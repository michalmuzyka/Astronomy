#pragma once
#include <GLFW/glfw3.h>

class Clock
{
public:
    float deltaTime = 0.0f;
    float lastFrame = 0.0f;

    void update()
    {
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
    }
};

class FPScounter
{
public:
    FPScounter(const Clock& clock)
        :clock(clock)
    {
        previous_time = clock.lastFrame;
    }

    void update()
    {
        float actual = static_cast<float>(glfwGetTime());
        float elapsed = actual - previous_time;

        if (elapsed >= 1.0f)
        {
            fps = frames_count / elapsed;
            previous_time = actual;
            frames_count = 0;
        }

        frames_count++;
    }

    int getFPS()
    {
        return fps;
    }

private:
    Clock clock;
    float previous_time;
    int frames_count = 0;
    int fps = 0;
};

class DayNightCycleController
{
public:
    DayNightCycleController(float duration)
    {
        partial_duration = duration / stages.size();
        calc_step();
    }

    void update(float delta_time)
    {
        color += step * delta_time;
        elapsed += delta_time;

        if(elapsed > partial_duration)
        {
            elapsed = 0;
            stage++;
            calc_step();
        }
    }

    glm::vec3 getColor()
    {
        return color;
    }

private:
    std::vector<glm::vec3> stages = {
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
        glm::vec3(1.0f, 1.0f, 1.0f),
    };

    void calc_step()
    {
        if (stage > stages.size())
            stage = 0;

        color = stages[stage%stages.size()];
        step = stages[(stage+1)%stages.size()] - color;
        step /= partial_duration;
    }

    glm::vec3 color;
    glm::vec3 step;
    int stage = 0;
    float partial_duration;
    float elapsed = 0;
};