#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


struct Point3D
{
    float x, y, z;
    int clusterId;
    float r, g, b;
};



class Window {
private:
    GLFWwindow* window;
    GLuint VAO, VBO;
    GLuint shaderProgram;
    int pointCount = 0;
    int WIDTH = 1024;
    int HEIGTH = 800;
    std::vector<float> vertices;
    std::vector<float> color;
    std::vector<float> interleaved;
    glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 1.0f);;
    glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
    glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

    bool firstMouse = true;
    float pitch = 0;
    float yaw = -90;
    float lastX = WIDTH / 2.0;
    float lastY = HEIGTH / 2.0;
public:
    Window(std::vector<Point3D>& point);
    void updatePoints(std::vector<Point3D>& point);
    void drawCluster();
    void windowColor();
    GLFWwindow* getWindow();
    void deleteShaders();

private:
    void shaderProcess();
    void vertexProcess();
    static void mouse_callback(GLFWwindow* window, double xPos, double yPos);
};

