#pragma once
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>
#include "Shader.h"
#include "Mesh.h"
#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

enum ECameraMovementType
{
    UNKNOWN,
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT,
    UP,
    DOWN
};

bool leftpressed = false;
bool rightpressed = false;
bool pressable3 = false;
bool pressable4 = false;
bool Darker;
bool Lighter;
bool UpPressed = false;
bool DownPressed = false;
bool cursor = true;
bool fullscreen = false;
bool pressable = true;
bool pressable2 = true;
float turnspeed = 0.0f;
float tiltspeed = 0.0f;

class Camera
{
private:
    // Default camera values
    const float zNEAR = 0.1f;
    const float zFAR = 5000000.f;
    const float YAW = 90.0f;
    const float PITCH = -13.0f;
    const float FOV = 45.0f;
    glm::vec3 startPosition;

public:
    Camera(const int width, const int height, const glm::vec3& position)
    {
        startPosition = position;
        Set(width, height, position);
        offset = 0.0f;
    }

    void MouseControl(float xPos, float yPos)
    {
        if (bFirstMouseMove) {
            lastX = xPos;
            lastY = yPos;
            bFirstMouseMove = false;
        }

        float xChange = xPos - lastX;
        float yChange = lastY - yPos;
        lastX = xPos;
        lastY = yPos;

        if (fabs(xChange) <= 1e-6 && fabs(yChange) <= 1e-6) {
            return;
        }
        xChange *= mouseSensitivity;
        yChange *= mouseSensitivity;

        ProcessMouseMovement(xChange, yChange);
    }

    void use(Shader* shader)
    {
        glm::mat4 projection = this->GetProjectionMatrix();
        glm::mat4 view = this->GetViewMatrix();
        shader->SetMat4("projection", projection);
        shader->SetMat4("view", view);
        shader->SetVec3("viewPos", position);
    }

    void Set(const int width, const int height, const glm::vec3& position)
    {
        this->isPerspective = true;
        this->yaw = YAW;
        this->pitch = PITCH;

        this->FoVy = FOV;
        this->width = width;
        this->height = height;
        this->zNear = zNEAR;
        this->zFar = zFAR;

        this->worldUp = glm::vec3(0, 1, 0);
        this->position = position;

        lastX = width / 2.0f;
        lastY = height / 2.0f;
        bFirstMouseMove = true;

        UpdateCameraVectors();
    }

    void Reset(const int width, const int height)
    {
        Set(width, height, startPosition);
    }

    void Reshape(int windowWidth, int windowHeight)
    {
        width = windowWidth;
        height = windowHeight;

        // define the viewport transformation
        glViewport(0, 0, windowWidth, windowHeight);
    }

    const glm::mat4 GetViewMatrix() const
    {
        // Returns the View Matrix
        return glm::lookAt(position, position + forward, up);
    }

    const glm::vec3 GetPosition() const
    {
        return position;
    }

    void SetPosition(glm::vec3 position)
    {
        this->position = position;
    }

    const glm::mat4 GetProjectionMatrix() const
    {
        glm::mat4 Proj = glm::mat4(1);
        if (isPerspective) {
            float aspectRatio = ((float)(width)) / height;
            Proj = glm::perspective(glm::radians(FoVy), aspectRatio, zNear, zFar);
        }
        else {
            float scaleFactor = 2000.f;
            Proj = glm::ortho<float>(
                -width / scaleFactor, width / scaleFactor,
                -height / scaleFactor, height / scaleFactor, -zFar, zFar);
        }
        return Proj;
    }

    void ProcessKeyboard(ECameraMovementType direction, float deltaTime)
    {
        float velocity = (float)(cameraSpeedFactor * deltaTime);
        switch (direction) {
        case ECameraMovementType::FORWARD:
            position += forward * velocity;
            break;
        case ECameraMovementType::BACKWARD:
            position -= forward * velocity;
            break;
        case ECameraMovementType::LEFT:
            position -= right * velocity;
            break;
        case ECameraMovementType::RIGHT:
            position += right * velocity;
            break;
        case ECameraMovementType::UP:
            position += up * velocity;
            break;
        case ECameraMovementType::DOWN:
            position -= up * velocity;
            break;
        }
    }

    void ProcessMouseScroll(float yOffset)
    {
        if (FoVy >= 1.0f && FoVy <= 90.0f) {
            FoVy -= yOffset;
        }
        if (FoVy <= 1.0f)
            FoVy = 1.0f;
        if (FoVy >= 90.0f)
            FoVy = 90.0f;
    }

    void UpdateCameraVectors()
    {
        // Calculate the new forward vector
        this->forward.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->forward.y = sin(glm::radians(pitch));
        this->forward.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
        this->forward = glm::normalize(this->forward);
        // Also re-calculate the Right and Up vector
        right = glm::normalize(glm::cross(forward, worldUp));  // Normalize the vectors, because their length gets closer to 0 the more you look up or down which results in slower movement.
        up = glm::normalize(glm::cross(right, forward));
    }

    float yaw;
    float pitch;
    float offset = 0.0f;
    float frontTilt = 13.0f;
    float speed = 0.0f;

private:
    void ProcessMouseMovement(float xOffset, float yOffset, bool constrainPitch = true)
    {
        offset += xOffset / 2.f;
        frontTilt += yOffset / 4.f;

        // Avem grijã sã nu ne dãm peste cap
        if (constrainPitch) {
            if (pitch > 89.0f)
                pitch = 89.0f;
            if (pitch < -89.0f)
                pitch = -89.0f;
        }

        // Se modificã vectorii camerei pe baza unghiurilor Euler
        UpdateCameraVectors();
    }

protected:
    const float cameraSpeedFactor = -1.0f;
    const float mouseSensitivity = 0.2f;

    // Perspective properties
    float zNear;
    float zFar;
    float FoVy;
    int width;
    int height;
    bool isPerspective;

    glm::vec3 position;
    glm::vec3 forward;
    glm::vec3 right;
    glm::vec3 up;
    glm::vec3 worldUp;

    bool bFirstMouseMove = true;
    float lastX = 0.f, lastY = 0.f;
};

// process all input: query GLFW whether relevant keys are pressed/released this frame and react accordingly
void processInput(GLFWwindow* window, Camera* pCamera, double deltaTime, Mesh* Player)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
    {
        UpPressed = true;
    }
    else
    {
        UpPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
    {
        DownPressed = true;
    }
    else
    {
        DownPressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
    {
        leftpressed = true;
        if (turnspeed < 0.2f)
            turnspeed += 0.0005;
        Player->setRotation(Player->getRotation() + glm::vec3(0.0f, turnspeed * 5.f, 0.0f));
        if (Player->getPosition().y > 0.f)
        {
            if (tiltspeed < 0.2f)
                tiltspeed += 0.0005;
        }
    }
    else
    {
        if (turnspeed > 0.0f)
            turnspeed -= 0.0005;
        if (Player->getPosition().y > 0.f)
        {
            Player->setRotation(Player->getRotation() + glm::vec3(0.0f, turnspeed * 5.f, 0.0f));
            if (tiltspeed > 0.0f)
                tiltspeed -= 0.0005;
        }
        else
        {
            leftpressed = false;
            tiltspeed = 0.0f;
            if (turnspeed > 0.0f)
                turnspeed = 0.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
    {
        rightpressed = true;
        if (turnspeed > -0.2f)
            turnspeed -= 0.0005;
        Player->setRotation(Player->getRotation() + glm::vec3(0.0f, turnspeed * 5.f, 0.0f));
        if (Player->getPosition().y > 0.f)
        {
            if (tiltspeed > -0.2f)
                tiltspeed -= 0.0005;
        }
    }
    else
    {
        if (turnspeed < 0.0f)
            turnspeed += 0.0005;
        if (Player->getPosition().y > 0.f)
        {
            Player->setRotation(Player->getRotation() + glm::vec3(0.0f, turnspeed * 5.f, 0.0f));
            if (tiltspeed < 0.0f)
                tiltspeed += 0.0005;
        }
        else
        {
            rightpressed = false;
            tiltspeed = 0.0f;
            if (turnspeed < 0.0f)
                turnspeed = 0.0f;
        }
    }

    if (glfwGetKey(window, GLFW_KEY_LEFT))
    {
        pCamera->offset -= 0.3f;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT))
    {
        pCamera->offset += 0.3f;
    }

    if (glfwGetKey(window, GLFW_KEY_Y))
    {
        // std::cout << "Camera: " << pCamera->GetPosition().x << " " << pCamera->GetPosition().y << " " << pCamera->GetPosition().z << '\n';
         //std::cout << "Avion: " << Player->getPosition().x << " " << Player->getPosition().y << " " << Player->getPosition().z << '\n';
        std::cout << "YAW: " << pCamera->yaw << '\n';
        // std::cout << "offset: " << pCamera->offset << '\n';
        std::cout << "Player Y: " << Player->getRotation().y << '\n';
    }

    if (glfwGetKey(window, GLFW_KEY_UP))
    {
        if (pCamera->pitch < -5.0f)
        {
            pCamera->pitch += 0.07f;
            pCamera->UpdateCameraVectors();
        }
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN))
    {
        if (pCamera->pitch > -26.0f)
        {
            pCamera->pitch -= 0.07f;
            pCamera->UpdateCameraVectors();
        }
    }

    if (glfwGetKey(window, GLFW_KEY_V))
    {
        if (pressable == true)
        {
            if (cursor)
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
                cursor = false;
            }
            else
            {
                glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
                cursor = true;
            }
            pressable = false;
        }
    }
    else
    {
        pressable = true;
    }

    if (glfwGetKey(window, GLFW_KEY_N))
    {
        if (pressable3 == true)
        {
            Darker = true;
        }
        pressable3 = false;
    }
    else
    {
        pressable3 = true;
    }

    if (glfwGetKey(window, GLFW_KEY_M))
    {
        if (pressable4 == true)
        {
            Lighter = true;
        }
        pressable4 = false;
    }
    else
    {
        pressable4 = true;
    }
}

// glfw: whenever the window size changed (by OS or user resize) this callback function executes
// ---------------------------------------------------------------------------------------------