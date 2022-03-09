#pragma once
#include "IDrawable.hpp"
#include "DataContainers.hpp"

class SpaceshipController :public IDrawable
{
public:
    enum ReflectorDirection
    {
        LEFT,
        RIGHT,
        UP,
        DOWN
    };

    enum ReflectorSize
    {
        BIGGER,
        SMALLER
    };

    SpaceshipController()
        :model("models/rocket/12216_rocket_v1_l2.obj"), Yaw(YAW), Pitch(PITCH)
    {
        auto min_max = model.getMinMax();
        auto center = model.getCenter();
        lightBeginPos = glm::vec3(center.x, center.y, min_max.second.z);
    }

    void update(float delta_time)
    {
        position += speed * delta_time;
        position.y = sin(glfwGetTime())*10;
        position.x = cos(glfwGetTime())*10;
    }

    CasterLight& getCasterLight()
    {
        light.position = (lightBeginPos + position) * scale_factor;
        return light;
    }

    void rotateCasterLight(ReflectorDirection dir)
    {
        float x = 0, y = 0;

        switch (dir)
            {
            case RIGHT: x = 0.1f; break;
            case LEFT: x = -0.1f; break;
            case UP: y = -0.1f; break;
            case DOWN: y = 0.1f; break;
        }

        Yaw += x;
        Pitch += y;

        glm::vec3 front;
        front.x = cos(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        front.y = sin(glm::radians(Pitch));
        front.z = sin(glm::radians(Yaw)) * cos(glm::radians(Pitch));
        light.direction = -glm::normalize(front);
    }

    void resizeCasterLight(ReflectorSize size)
    {
        float diff = 0.5;
        if (size == BIGGER && light.cutOff < 45.0f)
        {
            light.cutOff += diff;
            light.outerCutOff += diff;
        }
        else if(size == SMALLER && light.cutOff > diff)
        {
            light.cutOff -= diff;
            light.outerCutOff -= diff;
        }
    }

    void Draw(Shader& shader) override
    {
        auto center = model.getCenter();

        glm::mat4 m = glm::mat4(1.0f);

        m = glm::translate(m, position*scale_factor);

        m = glm::translate(m, glm::vec3(center.x * scale_factor, 0, 0));
        m = glm::rotate(m, (float)glfwGetTime()/10, glm::vec3(0.0f, 0.0f, 1.0f));
        m = glm::translate(m, glm::vec3(-center.x * scale_factor, 0, 0));
        m = glm::scale(m, glm::vec3(scale_factor, scale_factor, scale_factor));
        
        shader.setMat4("model", m);
        model.Draw(shader);
    }

    glm::vec3 getCenterPosition()
    {
        auto center = model.getCenter();
        return (center + position) * scale_factor;
    }

    glm::vec3 getCameraPosition()
    {
        auto min_max = model.getMinMax();
        auto center = model.getCenter();
        return (glm::vec3(center.x, min_max.second.y, center.z) + position)*scale_factor;
    }

private:
    Model model;
    glm::vec3 position = glm::vec3(0,0,0);
    glm::vec3 speed = glm::vec3(0, 0, 20);
    glm::vec3 lightBeginPos;
    float scale_factor = 0.01f;
    float Yaw;
    float Pitch;

    const float cutOffDiff = 10.0f;
    const float cutOff = 12.5;

    CasterLight light = {
        glm::vec3(0,0,0),
        glm::vec3(0,0,1),
        cutOff,
        cutOff + cutOffDiff,
        glm::vec3(0.2,0.2,0.2),
        glm::vec3(1,1,1),
        glm::vec3(1,1,1),
        1.0f,
        0.014f,
        0.0007f
    };
};