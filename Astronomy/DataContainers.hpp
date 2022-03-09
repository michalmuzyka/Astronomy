#pragma once
#include <glad/glad.h>
#include <glm/glm.hpp>
#include "Camera.hpp"

struct CoordinatesData
{
    glm::mat4 projection;
    glm::mat4 view;

    void update(Camera camera, int screen_w, int screen_h)
    {
        view = camera.GetViewMatrix();
        projection = glm::perspective(glm::radians(camera.Zoom), static_cast<float>(screen_w) / static_cast<float>(screen_h), 0.1f, 100.0f);
    }
};

struct WindowData
{
    int width = 1280;
    int height = 720;
    std::string title = "Astronomy";
};

struct ColoringData
{
    bool gouraud = false;
    bool blinn = false;
    glm::vec3 background = glm::vec3(0.1f, 0.1f, 0.1f);
    glm::vec3 sun_direction = glm::vec3(0.0f, -1.0f, 0.0f);
    float ambient_strength = 0.05f;
    float diffuse_strength = 0.4f;
    float specular_strength = 0.5f;
};

struct FogData
{
    float density = 0.05f;
    float fog_step = 0.001f;
    float fog_max = 1.0f;
    float fog_min = 0.0f;
};

struct MouseData
{
    float previousPositionX = 0;
    float previousPositionY = 0;
    bool firstMove = true;
};

struct CameraData
{
    enum TYPE
    {
        STATIC = 0,
        FOLLOWING = 1,
        MOVING_WITH = 2,
    };

    TYPE camera_type = STATIC;
    int cameras_count = 3;
    glm::vec3 beginingPostion = glm::vec3(-5.74f,-0.4f,5.56f);
    glm::vec3 followingPostion = glm::vec3(-17.8f,-1.2f,0.74f);
    float bYaw = 70.3;
    float bPitch = 1.5;
    float sYaw = 89.4;
    float sPitch = 6.2;
};

struct PointLight
{
    glm::vec3 position;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct CasterLight {
    glm::vec3 position;
    glm::vec3 direction;
    float cutOff;
    float outerCutOff;

    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;

    float constant;
    float linear;
    float quadratic;
};

struct ShadersPack
{
    Shader sphereShader;
    Shader asteroidShader;
    Shader spaceshipShader;

    Shader sphereShader_g;
    Shader asteroidShader_g;
    Shader spaceshipShader_g;

    ShadersPack()
        :sphereShader("object.vert", "sphere.frag"),
        asteroidShader("object.vert", "asteroid.frag"),
        spaceshipShader("object.vert", "spaceship.frag"),
        sphereShader_g("sphere_g.vert", "object_g.frag"),
        asteroidShader_g("asteroid_g.vert", "object_g.frag"),
        spaceshipShader_g("spaceship_g.vert", "object_g.frag")
    {
        
    }

    Shader& getAsteroidShader(bool gouraud)
    {
        if (gouraud)
            return asteroidShader_g;
        
        return asteroidShader;
    }

    Shader& getSpaceshipShader(bool gouraud)
    {
        if (gouraud)
            return spaceshipShader_g;
        
        return spaceshipShader;
    }

    Shader& getSphereShader(bool gouraud)
    {
        if (gouraud)
            return sphereShader_g;

        return sphereShader;
    }
};
