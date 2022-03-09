#include "MainApp.hpp"

WindowData   MainApp::window_data;
CameraData   MainApp::camera_data_;
Camera       MainApp::camera_;
MouseData    MainApp::mouse_data_;
Clock        MainApp::clock_;
FPScounter   MainApp::fps_(clock_);
FogData      MainApp::fog_;
ColoringData MainApp::colors_;
Sphere PlanetsController::sphere_;

MainApp::MainApp(GLFWwindow* window)
    :window(window),
    day_night_cycle_(60)
{
    glEnable(GL_DEPTH_TEST);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    camera_.SetNewData(camera_data_.beginingPostion, camera_data_.bYaw, camera_data_.bPitch);
}

void MainApp::framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    window_data.height = height;
    window_data.width = width;
    glViewport(0, 0, width, height);
}

void MainApp::processInput()
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        camera_.ProcessKeyboard(FORWARD, clock_.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        camera_.ProcessKeyboard(BACKWARD, clock_.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        camera_.ProcessKeyboard(LEFT, clock_.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        camera_.ProcessKeyboard(RIGHT, clock_.deltaTime);
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        fog_.density += fog_.fog_step;
        fog_.density = std::min(fog_.density, fog_.fog_max);
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        fog_.density -= fog_.fog_step;
        fog_.density = std::max(fog_.density, fog_.fog_min);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
        spaceship_.rotateCasterLight(SpaceshipController::LEFT);
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
        spaceship_.rotateCasterLight(SpaceshipController::RIGHT);
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
        spaceship_.rotateCasterLight(SpaceshipController::UP);
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
        spaceship_.rotateCasterLight(SpaceshipController::DOWN);

    if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS)
        spaceship_.resizeCasterLight(SpaceshipController::BIGGER);
    if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS)
        spaceship_.resizeCasterLight(SpaceshipController::SMALLER);
}

void MainApp::key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (key == GLFW_KEY_P && action == GLFW_RELEASE)
        colors_.gouraud = !colors_.gouraud;
    if (key == GLFW_KEY_B && action == GLFW_RELEASE)
        colors_.blinn = !colors_.blinn;
    if (key == GLFW_KEY_K && action == GLFW_RELEASE) {
        switch (camera_data_.camera_type)
        {
        case CameraData::STATIC:
            camera_data_.camera_type = CameraData::FOLLOWING;
            camera_.Position = camera_data_.followingPostion;
            break;
        case CameraData::FOLLOWING:
            camera_data_.camera_type = CameraData::MOVING_WITH;
            camera_.SetNewData(camera_data_.beginingPostion, camera_data_.sYaw, camera_data_.sPitch);
            break;
        case CameraData::MOVING_WITH:
            camera_data_.camera_type = CameraData::STATIC;
            camera_.SetNewData(camera_data_.beginingPostion, camera_data_.bYaw, camera_data_.bPitch);
            break;
        }
    }
}

void MainApp::update()
{
    clock_.update();
    fps_.update();
    coordinates_.update(camera_, window_data.width, window_data.height);
    spaceship_.update(clock_.deltaTime);
    day_night_cycle_.update(clock_.deltaTime);
    colors_.background = day_night_cycle_.getColor();
    updateCamera();
    glfwSetWindowTitle(window, (window_data.title + " FPS:" + std::to_string(fps_.getFPS())).c_str());
}

void MainApp::mainLoop()
{
    while (!glfwWindowShouldClose(window))
    {
        processInput();
        update();
        updateShaders();

        glClearColor(colors_.background.r,
                     colors_.background.g,
                     colors_.background.b,
                     1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        Shader& spaceshipShader = shaders_.getSpaceshipShader(colors_.gouraud);
        spaceshipShader.use();
        spaceship_.Draw(spaceshipShader);

        Shader& sphereShader = shaders_.getSphereShader(colors_.gouraud);
        sphereShader.use();
        planets_.Draw(sphereShader);

        Shader& asteroidShader = shaders_.getAsteroidShader(colors_.gouraud);
        asteroidShader.use();
        asteroids_.Draw(asteroidShader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

void MainApp::updateCamera()
{
    if(camera_data_.camera_type == CameraData::FOLLOWING)
        camera_.Front = spaceship_.getCenterPosition() -  camera_.Position;
    else if(camera_data_.camera_type == CameraData::MOVING_WITH)
        camera_.Position = spaceship_.getCameraPosition();
}

void MainApp::updateShaders()
{
    basicShaderUpdate(shaders_.spaceshipShader);
    basicShaderUpdate(shaders_.sphereShader);
    basicShaderUpdate(shaders_.asteroidShader);

    basicShaderUpdate(shaders_.spaceshipShader_g);
    basicShaderUpdate(shaders_.sphereShader_g);
    basicShaderUpdate(shaders_.asteroidShader_g);
}

void MainApp::basicShaderUpdate(Shader& s)
{
    s.use();
    s.setMat4("projection", coordinates_.projection);
    s.setMat4("view", coordinates_.view);
    s.setFloat("fog_density", fog_.density);
    s.setVec3("skyColor", colors_.background);
    s.setVec3("viewPos", camera_.Position);
    s.setBool("blinn", colors_.blinn);

    PointLight* lightPoints = planets_.getLightPoints();
    int count = PlanetsController::lightSpotsCount;
    for(int i=0; i<count; ++i)
    {
        std::string prefix = "pointLights[" + std::to_string(i) + "].";
        s.setVec3(prefix+"position", lightPoints[i].position);
        s.setVec3(prefix+"ambient", lightPoints[i].ambient);
        s.setVec3(prefix+"diffuse", lightPoints[i].diffuse);
        s.setVec3(prefix+"specular", lightPoints[i].specular);
        s.setFloat(prefix+"constant", lightPoints[i].constant);
        s.setFloat(prefix+"linear", lightPoints[i].linear);
        s.setFloat(prefix+"quadratic", lightPoints[i].quadratic);
    }

    CasterLight& caster = spaceship_.getCasterLight();
    std::string prefix = "casterLight.";
    s.setVec3(prefix + "position", caster.position);
    s.setVec3(prefix + "direction", caster.direction);
    s.setFloat(prefix + "cutOff", glm::cos(glm::radians(caster.cutOff)));
    s.setFloat(prefix + "outerCutOff", glm::cos(glm::radians(caster.outerCutOff)));
    s.setVec3(prefix + "ambient", caster.ambient);
    s.setVec3(prefix + "diffuse", caster.diffuse);
    s.setVec3(prefix + "specular", caster.specular);
    s.setFloat(prefix + "constant", caster.constant);
    s.setFloat(prefix + "linear", caster.linear);
    s.setFloat(prefix + "quadratic", caster.quadratic);

    prefix = "directionalLight.";
    s.setVec3(prefix + "direction", colors_.sun_direction);
    s.setVec3(prefix + "ambient", colors_.ambient_strength * colors_.background);
    s.setVec3(prefix + "diffuse", colors_.diffuse_strength * colors_.background);
    s.setVec3(prefix + "specular", colors_.specular_strength * colors_.background);
}


void MainApp::mouse_callback(GLFWwindow* window, double xposIn, double yposIn)
{
    float xpos = static_cast<float>(xposIn);
    float ypos = static_cast<float>(yposIn);

    if (mouse_data_.firstMove)
    {
        mouse_data_.previousPositionX = xpos;
        mouse_data_.previousPositionY = ypos;
        mouse_data_.firstMove = false;
    }

    float xoffset = xpos - mouse_data_.previousPositionX;
    float yoffset = mouse_data_.previousPositionY - ypos;

    mouse_data_.previousPositionX = xpos;
    mouse_data_.previousPositionY = ypos;

    if(camera_data_.camera_type != CameraData::FOLLOWING)
        camera_.ProcessMouseMovement(xoffset, yoffset);
}

void MainApp::scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera_.ProcessMouseScroll(static_cast<float>(yoffset));
}