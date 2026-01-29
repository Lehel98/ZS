#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Camera.h"
#include "Input.h"
#include "Time.h"
#include "Transform.h"

#include "Entity.h"
#include "AABB.h"
#include "CollisionSystem.h"
#include "CameraCollision.h"

#include "Shader.h"
#include "Mesh.h"
#include <glm/gtc/matrix_transform.hpp>

static const char* VertexShaderSource = R"(
#version 460 core

layout (location = 0) in vec3 aPosition;
layout (location = 1) in vec3 aColor;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

out vec3 vColor;

void main()
{
    vColor = aColor;
    gl_Position = projection * view * model * vec4(aPosition, 1.0);
}
)";

static const char* FragmentShaderSource = R"(
#version 460 core

in vec3 vColor;

uniform vec3 objectColor;
uniform bool useVertexColor;

out vec4 FragColor;

void main()
{
    vec3 finalColor = useVertexColor ? vColor : objectColor;
    FragColor = vec4(finalColor, 1.0);
}
)";

namespace
{
    constexpr int WindowWidth = 1920;
    constexpr int WindowHeight = 1080;
    constexpr const char* WindowTitle = "Zombie Survival";

    constexpr float FieldOfViewDegrees = 70.0f;
    constexpr float NearClippingPlane = 0.1f;
    constexpr float FarClippingPlane = 100.0f;

    constexpr float ArenaSize = 40.0f;
    constexpr float WallHeight = 10.0f;

    constexpr glm::vec3 GroundColor = glm::vec3(0.55f, 0.27f, 0.07f); // barna
    constexpr glm::vec3 WallColor1 = glm::vec3(1.0f, 0.6f, 0.0f);  // sárga
    constexpr glm::vec3 WallColor2 = glm::vec3(0.8f, 0.1f, 0.1f);  // vörös

    constexpr float CameraDistance = 6.0f;
    constexpr float CameraHeight = 3.0f;
}

static bool InitializeGlfw()
{
    if (glfwInit() == GLFW_FALSE)
    {
        std::cerr << "Failed to initialize GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwWindowHint(GLFW_DEPTH_BITS, 24);

    return true;
}

static GLFWwindow* CreateWindow()
{
    GLFWwindow* window =
        glfwCreateWindow(
            WindowWidth,
            WindowHeight,
            WindowTitle,
            nullptr,
            nullptr);

    if (window == nullptr)
    {
        std::cerr << "Failed to create GLFW window\n";
        return nullptr;
    }

    glfwMakeContextCurrent(window);
    return window;
}

static bool InitializeGlad()
{
    if (gladLoadGLLoader(
        reinterpret_cast<GLADloadproc>(glfwGetProcAddress)) == 0)
    {
        std::cerr << "Failed to initialize GLAD\n";
        return false;
    }

    return true;
}

static void PlayerMovement(Entity& player, const std::vector<Entity*>& worldEntities, const Camera& camera, float playerMovementSpeed)
{
    glm::vec3 forward =
    {
        camera.GetForwardDirection().x,
        0.0f,
        camera.GetForwardDirection().z
    };

    glm::vec3 right =
    {
        camera.GetRightDirection().x,
        0.0f,
        camera.GetRightDirection().z
    };

    if (glm::length(forward) > 0.0f)
        forward = glm::normalize(forward);

    if (glm::length(right) > 0.0f)
        right = glm::normalize(right);

    glm::vec3 movementDirection(0.0f);

    if (Input::IsKeyPressed(GLFW_KEY_W))
    {
        movementDirection += forward;
    }

    if (Input::IsKeyPressed(GLFW_KEY_S))
    {
        movementDirection -= forward;
    }

    if (Input::IsKeyPressed(GLFW_KEY_D))
    {
        movementDirection += right;
    }

    if (Input::IsKeyPressed(GLFW_KEY_A))
    {
        movementDirection -= right;
    }

    if (glm::length(movementDirection) > 0.0f)
    {
        movementDirection = glm::normalize(movementDirection);
    }

    glm::vec3 movement = movementDirection * playerMovementSpeed * Time::GetDeltaTime();

    MoveEntityWithCollision(player, movement, worldEntities);
}

static void RunGameLoop(GLFWwindow* window)
{
    float playerMovementSpeed = 5.0f;

    Input::Initialize(window);

    Shader shader(VertexShaderSource, FragmentShaderSource);
    Mesh cube;

    Entity ground;
    ground.transform.position = glm::vec3(0.0f, -0.5f, 0.0f);
    ground.transform.scale = glm::vec3(ArenaSize, 1.0f, ArenaSize);
    ground.color = GroundColor;
    ground.useVertexColor = false;
    ground.hasCollision = true;

    Entity wallNorth;
    wallNorth.transform.position = glm::vec3(0.0f, WallHeight / 2.0f, ArenaSize / 2.0f);
    wallNorth.transform.scale = glm::vec3(ArenaSize, WallHeight, 1.0f);
    wallNorth.color = WallColor1;
    wallNorth.useVertexColor = false;
    wallNorth.hasCollision = true;

    Entity wallSouth;
    wallSouth.transform.position = glm::vec3(0.0f, WallHeight / 2.0f, -ArenaSize / 2.0f);
    wallSouth.transform.scale = glm::vec3(ArenaSize, WallHeight, 1.0f);
    wallSouth.color = WallColor1;
    wallSouth.useVertexColor = false;
    wallSouth.hasCollision = true;

    Entity wallEast;
    wallEast.transform.position = glm::vec3(ArenaSize / 2.0f, WallHeight / 2.0f, 0.0f);
    wallEast.transform.scale = glm::vec3(1.0f, WallHeight, ArenaSize);
    wallEast.color = WallColor2;
    wallEast.useVertexColor = false;
    wallEast.hasCollision = true;

    Entity wallWest;
    wallWest.transform.position = glm::vec3(-ArenaSize / 2.0f, WallHeight / 2.0f, 0.0f);
    wallWest.transform.scale = glm::vec3(1.0f, WallHeight, ArenaSize);
    wallWest.color = WallColor2;
    wallWest.useVertexColor = false;
    wallWest.hasCollision = true;

    Entity centerCube;
    centerCube.transform.position = glm::vec3(0.0f, 0.5f, 0.0f);
    centerCube.transform.scale = glm::vec3(1.0f);
    centerCube.color = glm::vec3(1.0f);
    centerCube.useVertexColor = true;
    centerCube.hasCollision = true;

    Entity player;
    player.transform.position = glm::vec3(0.0f, 0.6f, 5.0f);
    player.transform.scale = glm::vec3(1.0f);
    player.color = glm::vec3(0.9f, 0.6f, 0.3f);
    player.useVertexColor = true;
    player.hasCollision = true;

    std::vector<Entity*> worldEntities =
    {
        &ground,
        &wallNorth,
        &wallSouth,
        &wallEast,
        &wallWest,
        &centerCube,
        &player
    };

    const float aspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);

    auto DrawEntity = [&](const Entity& entity)
    {
        glm::mat4 model = entity.transform.GetModelMatrix();

        shader.SetMat4("model", model);
        shader.SetVec3("objectColor", entity.color);
        shader.SetBool("useVertexColor", entity.useVertexColor);

        cube.Draw();
    };

    Camera camera(window);
    glm::vec3 initialCameraOffset = -camera.GetForwardDirection() * CameraDistance + glm::vec3(0.0f, CameraHeight, 0.0f);
    camera.SetPosition(player.transform.position + initialCameraOffset);

    glEnable(GL_DEPTH_TEST);

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        Time::Update();

        if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        camera.Update();

        PlayerMovement(player, worldEntities, camera, playerMovementSpeed);

        glm::vec3 cameraOffset = -camera.GetForwardDirection() * CameraDistance + glm::vec3(0.0f, CameraHeight, 0.0f);

        glm::vec3 desiredCameraPosition = player.transform.position + cameraOffset;

        glm::vec3 finalCameraPosition = ComputeCameraPositionWithCollision(player, desiredCameraPosition, worldEntities);
        camera.SetPosition(finalCameraPosition);

        glClearColor(0.05f, 0.05f, 0.08f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        shader.Use();

        glm::mat4 view = camera.GetViewMatrix();
        
        glm::mat4 projection =
            glm::perspective(
                glm::radians(FieldOfViewDegrees),
                aspectRatio,
                NearClippingPlane,
                FarClippingPlane
            );

        shader.SetMat4("view", view);
        shader.SetMat4("projection", projection);

        centerCube.transform.rotationDegrees.y += 30.0f * Time::GetDeltaTime();
        for (const Entity* entity : worldEntities)
        {
            DrawEntity(*entity);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }
}

int main()
{
    if (!InitializeGlfw())
    {
        return -1;
    }

    GLFWwindow* window = CreateWindow();
    if (window == nullptr)
    {
        glfwTerminate();
        return -1;
    }

    if (!InitializeGlad())
    {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, WindowWidth, WindowHeight);

    RunGameLoop(window);

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}
