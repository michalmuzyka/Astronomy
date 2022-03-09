#include "MainApp.hpp"

int main()
{
    try {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
        glfwWindowHint(GLFW_SAMPLES, 4);

        GLFWwindow* window = glfwCreateWindow(MainApp::window_data.width, MainApp::window_data.height, MainApp::window_data.title.c_str(), nullptr, nullptr);

        if (!window)
            throw std::exception("Failed to create GLFW window");

        glfwMakeContextCurrent(window);

        if (!gladLoadGLLoader(reinterpret_cast<GLADloadproc>(glfwGetProcAddress)))
            throw std::exception("Failed to initialize GLAD");

        MainApp app(window);
        app.mainLoop();
    } catch (std::exception& e) {
        std::cout << "Error: " << e.what() << std::endl;
    }

    glfwTerminate();

    return 0;
}

