#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <string>
#include "Model.hpp"
#include "Shader.hpp"
#include "Camera.hpp"
#include "Utilities.hpp"
#include "DataContainers.hpp"
#include "SpaceshipController.hpp"
#include "AsteroidsController.hpp"
#include "PlanetsController.hpp"

class MainApp
{
public:
    MainApp(GLFWwindow* window);
    void mainLoop();

    static WindowData window_data;

private:
    GLFWwindow* window;

    AsteroidsController asteroids_;
    PlanetsController planets_;
    SpaceshipController spaceship_;

    ShadersPack shaders_;

    static ColoringData colors_;
    static FogData fog_;
    static Camera camera_;
    static CameraData camera_data_;
    static MouseData mouse_data_;
    static Clock clock_;
    static FPScounter fps_;
    DayNightCycleController day_night_cycle_;
    CoordinatesData coordinates_;

    void processInput();
    void update();
    void updateCamera();
    void updateShaders();
    void basicShaderUpdate(Shader& s);

    static void framebuffer_size_callback(GLFWwindow* window, int width, int height);
    static void mouse_callback(GLFWwindow* window, double xposIn, double yposIn);
    static void scroll_callback(GLFWwindow* window, double xoffset, double yoffset);
    static void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
};

