#include <GL/glew.h>
#include <glfw3.h>

#define STB_IMAGE_IMPLEMENTATION
#include <iostream>
#include "TextureLoader.h"
#include "Camera.h"
#include <fstream>
#include <string>
#include <sstream>
#include "Shader.h"
#include "OBJLoader.h"
#include "Mesh.h"
#include "Camera.h"
#define FREEGLUT_STATIC
#define _LIB
#define FREEGLUT_LIB_PRAGMAS 0
#include <GL/freeglut.h>
#include <filesystem>

#pragma comment (lib, "glfw3dll.lib")
#pragma comment (lib, "glew32.lib")
#pragma comment (lib, "OpenGL32.lib")

float skylight;
void changeHour(Shader& shader1, Shader& shader2)
{
    static glm::vec3 value(0.6f);
    skylight = value.x;
    if (Darker == true)
    {
        if (value.x > 0.2f)
        {
            value -= glm::vec3(0.05f);
            shader1.Use();
            shader1.SetVec3("lightColor", value);
            shader2.Use();
            shader2.SetVec3("lightColor", value);
        }
        Darker = false;
    }

    if (Lighter == true)
    {
        if (value.x < 0.9f)
        {
            value += glm::vec3(0.05f);
            shader1.Use();
            shader1.SetVec3("lightColor", value);
            shader2.Use();
            shader2.SetVec3("lightColor", value);
        }
        Lighter = false;
    }
}

std::vector<Mesh> Aeroport;
unsigned int GrassTex;
unsigned int RoadTex;
unsigned int RoofTex;
unsigned int GrindaTex;
unsigned int TileTex;
unsigned int LeafTex;
unsigned int TurnTex;

void AeroportInit()
{
    Mesh AcoperisHangar("models/AcoperisHangar.obj");
    Aeroport.push_back(AcoperisHangar);

    Mesh Iarba("models/Iarba.obj");
    Aeroport.push_back(Iarba);

    Mesh InteriorHangar("models/InteriorHangar.obj");
    Aeroport.push_back(InteriorHangar);

    Mesh MetalHangare("models/MetalHangare.obj");
    Aeroport.push_back(MetalHangare);

    Mesh Road("models/Road.obj");
    Aeroport.push_back(Road);

    Mesh TurnBaza("models/TurnBaza1.obj");
    TurnBaza.setColor(0, glm::vec3(0.85f, 0.85f, 0.85f));
    Aeroport.push_back(TurnBaza);

    Mesh TurnBazaTexture("models/TurnBazaTexture.obj");
    Aeroport.push_back(TurnBazaTexture);

    Mesh TurnVarfAlb("models/TurnVarfAlb.obj");
    TurnVarfAlb.setColor(0, glm::vec3(0.85f, 0.85f, 0.85f));
    Aeroport.push_back(TurnVarfAlb);

    Mesh TurnVarfNegru("models/TurnVarfNegru.obj");
    TurnVarfNegru.setColor(0, glm::vec3(0.0f, 0.0f, 0.0f));
    Aeroport.push_back(TurnVarfNegru);

    Mesh Fundatie("models/Fundatie.obj");
    Aeroport.push_back(Fundatie);

    for (int i = 0; i < Aeroport.size(); i++)
    {
        if (i != 1 && i != 4 && i != 9)
            Aeroport[i].setPosition(glm::vec3(10.f, -7.f, 10.f));
        else
            Aeroport[i].setPosition(glm::vec3(10.f, 0.f, 10.f));
        Aeroport[i].setScale(glm::vec3(10.f));
        Aeroport[i].initVAO();
    }

    GrassTex = CreateTexture("Resources/Grass.jpg");
    RoadTex = CreateTexture("Resources/Road.jpg");
    RoofTex = CreateTexture("Resources/Shelter_simple_greenpanel.jpg");
    TurnTex = CreateTexture("Resources/tower2.jpg");
    TileTex = CreateTexture("Resources/Shelter_simple_whitepanel.jpg");
    GrindaTex = CreateTexture("Resources/Shelter_simple_frame.bmp");
}

void AeroportRender(Shader& shaderT, Shader& shaderM)
{
    shaderT.Use();
    for (int i = 0; i < Aeroport.size(); i++)
    {
        if (i == 1)
        {
            glBindTexture(GL_TEXTURE_2D, GrassTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 4)
        {
            glBindTexture(GL_TEXTURE_2D, RoadTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 0)
        {
            glBindTexture(GL_TEXTURE_2D, RoofTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 6)
        {
            glBindTexture(GL_TEXTURE_2D, TurnTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 2)
        {
            glBindTexture(GL_TEXTURE_2D, TileTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 3)
        {
            glBindTexture(GL_TEXTURE_2D, GrindaTex);
            Aeroport[i].render(&shaderT);
        }
        if (i == 9)
        {
            glBindTexture(GL_TEXTURE_2D, RoadTex);
            Aeroport[i].render(&shaderT);
        }
    }
    shaderM.Use();
    for (int i = 0; i < Aeroport.size(); i++)
    {
        if (i != 1 && i != 4 && i != 0 && i != 6 && i != 2 && i != 3 && i != 9)
        {
            Aeroport[i].render(&shaderM);
        }
    }
}

Camera* pCamera;
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    // make sure the viewport matches the new window dimensions; note that width and 
    // height will be significantly larger than specified on retina displays.
    pCamera->Reshape(width, height);
}

void scroll_callback(GLFWwindow* window, double xoffset, double yOffset)
{
    pCamera->ProcessMouseScroll((float)yOffset);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    pCamera->MouseControl((float)xpos, (float)ypos);
}

const unsigned int width = 1920;
const unsigned int height = 1080;

int main(void)
{
    GLFWwindow* window;

    if (!glfwInit())
        return -1;
    window = glfwCreateWindow(width, height, "Flight_Simulator", NULL, NULL);
    glfwMakeContextCurrent(window);
    glewInit();
    glViewport(0, 0, width, height);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetScrollCallback(window, scroll_callback);
    glDisable(GL_BLEND);
    glEnable(GL_DEPTH_TEST);

    pCamera = new Camera(width, height, glm::vec3(0.f, 0.f, 0.f));
    Shader shader;
    Shader terrainShader;
    Shader AirportShader;
    shader.Set("Basic.shader");
    terrainShader.Set("terrain.shader");
    int floorTexture = CreateTexture(std::filesystem::path("GOOGLE_SAT_WM.jpg").string().c_str());
    terrainShader.SetInt("texture1", 0);

    Mesh Avion("Plane.obj");
    Avion.setPosition(glm::vec3(0.f));
    Avion.setColor(0, glm::vec3(0.0f, 0.15f, 0.3f));
    Avion.setColor(1, glm::vec3(0.1f, 0.1f, 0.1f));
    Avion.setColor(2, glm::vec3(0.5f, 0.5f, 0.5f));
    Avion.setRotation(glm::vec3(0.f, 180.0f, 0.f));
    Avion.setPosition(glm::vec3(0.0f, 0.0f, 0.0f));
    Avion.initVAO();

    Mesh Harta("Transilvania.obj");
    Harta.setScale(glm::vec3(0.1f, 0.1f, 0.1f));
    Harta.setPosition(glm::vec3(0.0f, -200.0f, -180.0f));
    Harta.initVAO();

    AeroportInit();

    float deltaTime = 0.f;
    float lastFrame = 0.f;
    float deltaAltitude = 0.f;
    bool desprindere = true;

    float position;
    shader.Use();
    shader.SetVec3("lightColor", glm::vec3(0.6f, 0.6f, 0.6f));
    terrainShader.Use();
    terrainShader.SetVec3("lightColor", glm::vec3(0.6f, 0.6f, 0.6f));

    while (!glfwWindowShouldClose(window))
    {
        double FrameStart = glfwGetTime();
        float currentAltitude = Avion.getPosition().y;
        deltaAltitude = (pCamera->speed - 0.5f) * 2.f;
        float currentTilt = Avion.getRotation().y;
        changeHour(shader, terrainShader);

        float clearR = 0.07f + skylight / 2.f - 0.1f;
        float clearG = 0.13f + skylight / 2.f - 0.1f;
        float clearB = 0.17 + skylight / 2.f - 0.1f;
        glClearColor(clearR, clearG, clearB, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        if (Avion.getPosition().y < 0.0f)
        {
            Avion.setPosition(glm::vec3(Avion.getPosition().x, 0.0f, Avion.getPosition().z));
        }

        if (Avion.getPosition().y > 0.0f)
        {
            if (desprindere)
            {
                turnspeed = 0.0f;
            }
            desprindere = false;
        }
        else
        {
            desprindere = true;
        }

        if (UpPressed == true)
        {
            if (pCamera->speed < 1.f)
            {
                pCamera->speed += 0.0002f;
            }
        }
        else
            if (DownPressed == true)
            {
                if (pCamera->speed > 0.f)
                {
                    pCamera->speed -= 0.0008;
                }
            }
            else
            {
                if (pCamera->speed > 0.f)
                {
                    pCamera->speed -= 0.0003f;
                }
            }

        if (pCamera->speed > 0.3f)
        {
            float ascending = (pCamera->speed - 0.5f);
            Avion.setPosition(glm::vec3(Avion.getPosition() + glm::vec3(0.0f, (pCamera->speed - 0.05f) * 5.f, 0.0f)));
        }
        else
            if (pCamera->speed < 0.1f)
            {
                if (Avion.getPosition().y > 0.0f)
                {
                    float descending = pCamera->speed - 0.5f;
                    Avion.setPosition(glm::vec3(Avion.getPosition() + glm::vec3(0.0f, descending * 50.f, 0.0f)));
                }
            }

        float Xrot = 0.0f;
        float Zrot = 0.0f;
        float Xstrife = 0.f;
        float Zstrife = 0.f;
        if (Avion.getPosition().y > 0.f)
        {
            Xstrife = tiltspeed * 200.f * glm::sin(glm::radians(Avion.getRotation().y));
            Zstrife = tiltspeed * 200.f * glm::cos(glm::radians(Avion.getRotation().y));
            if (pCamera->speed > 0.1f)
            {
                Xrot = deltaAltitude * 25.f * glm::cos(glm::radians(Avion.getRotation().y));
                Zrot = deltaAltitude * 25.f * -glm::sin(glm::radians(Avion.getRotation().y));
            }
            else
            {
                Xrot = deltaAltitude * 45.f * glm::cos(glm::radians(Avion.getRotation().y));
                Zrot = deltaAltitude * 45.f * -glm::sin(glm::radians(Avion.getRotation().y));
            }
        }

        float angle = glm::abs(Xrot) + glm::abs(Zrot);
        if (Avion.getRotation().y > 0.f)
            angle *= 2;
        else
            angle /= 4.f;
        float momentum = glm::abs(glm::cos(glm::radians(angle)));
        Avion.setPosition(glm::vec3(Avion.getPosition() + glm::vec3(pCamera->speed * momentum * 15.f * glm::sin(glm::radians(Avion.getRotation().y)), 0.0f, pCamera->speed * momentum * 15.f * glm::cos(glm::radians(Avion.getRotation().y)))));
        Avion.setRotation(glm::vec3(0.f, Avion.getRotation().y, 0.f) - glm::vec3(Xrot + Xstrife, 0.0f, Zrot + Zstrife));

        processInput(window, pCamera, deltaTime, &Avion);
        pCamera->SetPosition(glm::vec3(Avion.getPosition() + glm::vec3(-sin(glm::radians(Avion.getRotation().y + pCamera->offset)) * 50.f, 15.0f, -cos(glm::radians(Avion.getRotation().y + pCamera->offset)) * 50.f)));
        pCamera->SetPosition(pCamera->GetPosition() + glm::vec3(0.0f, glm::radians((pCamera->frontTilt - 13.f) / 1.5f) * 50.f, 0.0f));
        pCamera->pitch = -pCamera->frontTilt;
        pCamera->yaw = -((float)Avion.getRotation().y + (float)pCamera->offset - 90.f);

        terrainShader.Use();
        pCamera->UpdateCameraVectors();
        pCamera->use(&terrainShader);
        glBindTexture(GL_TEXTURE_2D, floorTexture);
        Harta.render(&terrainShader);
        shader.Use();
        pCamera->use(&shader);
        Avion.render(&shader);
        AeroportRender(terrainShader, shader);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    shader.Delete();
    glfwTerminate();
    return 0;
}