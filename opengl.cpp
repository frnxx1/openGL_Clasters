#include "opengl.hpp"
#include <iostream>
#include <algorithm>

static const char* vertexShaderSource = "#version 460 core\n"
"layout (location = 0) in vec3 aPos;\n"
"layout (location = 1) in vec3 aColor;\n"
"out vec3 Color;\n"
"uniform mat4 model;\n"
"uniform mat4 view;\n"
"uniform mat4 proj;\n"
"void main()\n"
"{\n"
"gl_Position = proj * view * model * vec4(aPos, 1.0);\n"
"Color = aColor;\n"
"}\0";

static const char* fragmentShaderSource = "#version 460 core\n"
"in vec3 Color;\n"
"out vec4 FragColor;\n"
"void main()\n"
"{\n"
"vec2 center = vec2(0.5f,0.5f);\n"
"float dist = distance(gl_PointCoord,center);\n"
"if (dist > 0.5f) discard;\n"
"FragColor = vec4(Color, 0.4f);\n"
"}\0";

Window::Window(std::vector<Point3D>& point)
{
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(WIDTH, HEIGTH, "Window", NULL, NULL);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    glfwSetWindowUserPointer(window, this);
    glfwSetCursorPosCallback(window, mouse_callback);

    glfwMakeContextCurrent(window);
    gladLoadGL();
    glViewport(0, 0, WIDTH, HEIGTH);

    shaderProcess();
    updatePoints(point);
    vertexProcess();
}

void Window::mouse_callback(GLFWwindow* window, double xPos, double yPos)
{
    Window* self = (Window*)glfwGetWindowUserPointer(window);
    if (!self) return;
    float xpos = xPos;
    float ypos = yPos;

    if (self->firstMouse)
    {
        self->lastX = xpos;
        self->lastY = ypos;
        self->firstMouse = false;
    }

    float xoffset = xpos - self->lastX;
    float yoffset = self->lastY - ypos;
    self->lastX = xpos;
    self->lastY = ypos;

    float sensitivity = 0.1f;
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    self->yaw += xoffset;
    self->pitch += yoffset;

    
    if (self->pitch > 89.0f)
        self->pitch = 89.0f;
    if (self->pitch < -89.0f)
        self->pitch = -89.0f;

    glm::vec3 front;
    front.x = cos(glm::radians(self->yaw)) * cos(glm::radians(self->pitch));
    front.y = sin(glm::radians(self->pitch));
    front.z = sin(glm::radians(self->yaw)) * cos(glm::radians(self->pitch));
    self->cameraFront = glm::normalize(front);
}

void Window::updatePoints(std::vector<Point3D>& point)
{
    pointCount = point.size();
    color.reserve(pointCount * 3);
    vertices.reserve(pointCount * 3);
    
    for (const auto& p : point)
    {
        vertices.push_back(p.x);
        vertices.push_back(p.y);
        vertices.push_back(p.z);

        color.push_back(p.r);
        color.push_back(p.g);
        color.push_back(p.b);
    }
    
    interleaved.reserve(pointCount * 6);
    for (int i = 0; i < pointCount; ++i)
    {
        interleaved.push_back(vertices[i * 3 + 0]);
        interleaved.push_back(vertices[i * 3 + 1]);
        interleaved.push_back(vertices[i * 3 + 2]);
        interleaved.push_back(color[i * 3 + 0]);
        interleaved.push_back(color[i * 3 + 1]);
        interleaved.push_back(color[i * 3 + 2]);
    }

}

void Window::drawCluster()
{
    glUseProgram(shaderProgram);
    glBindVertexArray(VAO);

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    const float cameraSpeed = 0.1f;
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) cameraPos += cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) cameraPos -= cameraSpeed * cameraFront;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) cameraPos -= glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) cameraPos += glm::normalize(glm::cross(cameraFront, cameraUp)) * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) cameraPos += cameraUp * cameraSpeed;
    if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS) cameraPos -= cameraUp * cameraSpeed;

    glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, -2.0f));;
    /*model *= glm::rotate(glm::mat4(1.0f), (float)glfwGetTime()*0.4f, glm::vec3(1.0f, 2.0f, 0.0f));*/
    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);
    glm::mat4 proj = glm::perspective(glm::radians(70.0f), (float)WIDTH / HEIGTH, 0.1f, 300.0f);
    int uni_model = glGetUniformLocation(shaderProgram, "model");
    int uni_view = glGetUniformLocation(shaderProgram, "view");
    int uni_proj = glGetUniformLocation(shaderProgram, "proj");
    glUniformMatrix4fv(uni_model, 1, GL_FALSE, &model[0][0]);
    glUniformMatrix4fv(uni_view, 1, GL_FALSE, &view[0][0]);
    glUniformMatrix4fv(uni_proj, 1, GL_FALSE, &proj[0][0]);

    glPointSize(5.0f);
    glDrawArrays(GL_POINTS, 0, pointCount);
}

void Window::windowColor()
{
    glClearColor(0.200f, 0.2f, 0.2f, 0.1f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Window::shaderProcess()
{
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void Window::vertexProcess()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glEnable(GL_PROGRAM_POINT_SIZE);
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glEnable(GL_DEPTH_TEST);

    glBufferData(GL_ARRAY_BUFFER, interleaved.size() * sizeof(GLfloat), interleaved.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

GLFWwindow* Window::getWindow()
{
    return window;
}

void Window::deleteShaders()
{
    glDeleteVertexArrays(1, &VAO);
    glDeleteBuffers(1, &VBO);
    glDeleteProgram(shaderProgram);
    glfwDestroyWindow(window);
    glfwTerminate();
}
