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
#include "PlayerCollision.h"

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

    constexpr float ArenaSize = 100.0f;
    constexpr float WallHeight = 10.0f;
    constexpr float WallThickness = 1.0f;

    constexpr glm::vec3 GroundColor = glm::vec3(0.55f, 0.27f, 0.07f); // barna
    constexpr glm::vec3 WallColor1 = glm::vec3(1.0f, 0.6f, 0.0f);  // sárga
    constexpr glm::vec3 WallColor2 = glm::vec3(0.8f, 0.1f, 0.1f);  // vörös

    constexpr float CameraHeight = 2.0f;
    constexpr float CameraRadius = 0.3f;

    constexpr float MinDegree = -70.0f;
    constexpr float MaxDegree = 20.0f;
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

    if (glm::length(movementDirection) == 0.0f)
    {
        return;
    }

    movementDirection = glm::normalize(movementDirection);

    glm::vec3 movement = movementDirection * playerMovementSpeed * Time::GetDeltaTime();

    glm::vec3 newPosition = player.transform.position;

    auto ComputePlayerCapsule = [&](const glm::vec3& position, glm::vec3& outBase, glm::vec3& outTip)
    {
        const float radius = player.collision.capsule.radius;
        const float height = player.collision.capsule.height;

        outBase = position + player.collision.localOffset + glm::vec3(0.0f, radius, 0.0f);

        outTip = outBase + glm::vec3(0.0f, height - 2.0f * radius, 0.0f);
    };

    // ---- X AXIS ----
    if (movement.x != 0.0f)
    {
        glm::vec3 testPosition = newPosition;
        testPosition.x += movement.x;

        glm::vec3 capsuleBase;
        glm::vec3 capsuleTip;

        ComputePlayerCapsule(testPosition, capsuleBase, capsuleTip);

        bool collided = false;

        for (const Entity* e : worldEntities)
        {
            if (e == &player)
                continue;

            if (e->collision.type != CollisionShape::Type::AABB)
                continue;

            AABB box = ComputeWorldAABB(*e);

            if (IntersectCapsuleVsAABB(
                capsuleBase,
                capsuleTip,
                player.collision.capsule.radius,
                box))
            {
                collided = true;
                break;
            }
        }

        if (!collided)
            newPosition.x = testPosition.x;
    }

    // ---- Z AXIS ----
    if (movement.z != 0.0f)
    {
        glm::vec3 testPosition = newPosition;
        testPosition.z += movement.z;

        glm::vec3 capsuleBase;
        glm::vec3 capsuleTip;

        ComputePlayerCapsule(testPosition, capsuleBase, capsuleTip);

        bool collided = false;

        for (const Entity* e : worldEntities)
        {
            if (e == &player)
                continue;

            if (e->collision.type != CollisionShape::Type::AABB)
                continue;

            AABB box = ComputeWorldAABB(*e);

            if (IntersectCapsuleVsAABB(capsuleBase, capsuleTip, player.collision.capsule.radius, box))
            {
                collided = true;
                break;
            }
        }

        if (!collided)
            newPosition.z = testPosition.z;
    }

    player.transform.position = newPosition;
}

static void RunGameLoop(GLFWwindow* window)
{
    float playerMovementSpeed = 10.0f;

    Input::Initialize(window);

    Shader shader(VertexShaderSource, FragmentShaderSource);
    Mesh cube;

    Entity ground;
    ground.transform.position = glm::vec3(0.0f, 0.0f, 0.0f);
    ground.color = GroundColor;
    ground.useVertexColor = false;
    ground.collision.type = CollisionShape::Type::AABB;
    ground.collision.localOffset = glm::vec3(0.0f);
    ground.collision.halfExtents =
    {
        ArenaSize * 0.5f,
        0.05f,
        ArenaSize * 0.5f
    };
    ground.transform.scale = ground.collision.halfExtents * 2.0f;


    Entity wallNorth;
    wallNorth.transform.position = glm::vec3(0.0f, WallHeight * 0.5f, ArenaSize * 0.5f - WallThickness * 0.5f);
    wallNorth.color = WallColor1;
    wallNorth.useVertexColor = false;
    wallNorth.collision.type = CollisionShape::Type::AABB;
    wallNorth.collision.localOffset = glm::vec3(0.0f);
    wallNorth.collision.halfExtents =
    {
        ArenaSize * 0.5f,
        WallHeight * 0.5f,
        WallThickness * 0.5f
    };
    wallNorth.transform.scale = wallNorth.collision.halfExtents * 2.0f;

    Entity wallSouth;
    wallSouth.transform.position = glm::vec3(0.0f, WallHeight * 0.5f, -ArenaSize * 0.5f + WallThickness * 0.5f);
    wallSouth.color = WallColor1;
    wallSouth.useVertexColor = false;
    wallSouth.collision.type = CollisionShape::Type::AABB;
    wallSouth.collision.localOffset = glm::vec3(0.0f);
    wallSouth.collision.halfExtents =
    {
        ArenaSize * 0.5f,
        WallHeight * 0.5f,
        WallThickness * 0.5f
    };
    wallSouth.transform.scale = wallSouth.collision.halfExtents * 2.0f;

    Entity wallEast;
    wallEast.transform.position = glm::vec3(ArenaSize * 0.5f - WallThickness * 0.5f, WallHeight * 0.5f, 0.0f);
    wallEast.color = WallColor2;
    wallEast.useVertexColor = false;
    wallEast.collision.type = CollisionShape::Type::AABB;
    wallEast.collision.localOffset = glm::vec3(0.0f);
    wallEast.collision.halfExtents =
    {
        WallThickness * 0.5f, // X – vékony
        WallHeight * 0.5f,    // Y – magas
        ArenaSize * 0.5f     // Z – hosszú
    };
    wallEast.transform.scale = wallEast.collision.halfExtents * 2.0f;

    Entity wallWest;
    wallWest.transform.position = glm::vec3(-ArenaSize * 0.5f + WallThickness * 0.5f, WallHeight * 0.5f, 0.0f);
    wallWest.color = WallColor2;
    wallWest.useVertexColor = false;
    wallWest.collision.type = CollisionShape::Type::AABB;
    wallWest.collision.localOffset = glm::vec3(0.0f);
    wallWest.collision.halfExtents =
    {
        WallThickness * 0.5f,
        WallHeight * 0.5f,
        ArenaSize * 0.5f
    };
    wallWest.transform.scale = wallWest.collision.halfExtents * 2.0f;

    Entity centerCube;
    centerCube.transform.position = glm::vec3(0.0f, 0.5f, 0.0f);
    centerCube.transform.scale = glm::vec3(1.0f);
    centerCube.color = glm::vec3(1.0f);
    centerCube.useVertexColor = true;
    centerCube.collision.type = CollisionShape::Type::AABB;
    centerCube.collision.localOffset = glm::vec3(0.0f);
    centerCube.collision.halfExtents =
    {
        0.5f,
        0.5f,
        0.5f
    };

    Entity player;
    player.transform.position = glm::vec3(0.0f, 0.0f, 5.0f);
    player.transform.scale = glm::vec3(1.0f);
    player.color = glm::vec3(0.9f, 0.6f, 0.3f);
    player.useVertexColor = true;

    player.collision.type = CollisionShape::Type::Capsule;
    player.collision.localOffset = glm::vec3(0.0f);
    player.collision.capsule.radius = 0.5f;
    player.collision.capsule.height = 1.0f;

    const float groundHalfHeight = ground.collision.halfExtents.y;
    player.transform.position.y = groundHalfHeight + player.collision.capsule.radius;

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

    Camera camera(window);

    glm::vec3 pivot = player.transform.position + glm::vec3(0.0f, player.collision.capsule.height * 0.5f + CameraHeight, 0.0f);

    const float cameraDistance = player.collision.capsule.height * 0.5f + CameraHeight;

    const float aspectRatio = static_cast<float>(WindowWidth) / static_cast<float>(WindowHeight);

    auto DrawEntity = [&](const Entity& entity)
    {
        glm::mat4 model = entity.transform.GetModelMatrix();

        shader.SetMat4("model", model);
        shader.SetVec3("objectColor", entity.color);
        shader.SetBool("useVertexColor", entity.useVertexColor);

        cube.Draw();
    };

#ifdef ENGINE_DEBUG

    auto DrawCollisionAABB = [&](const Entity& entity)
    {
        if (entity.collision.type != CollisionShape::Type::AABB)
            return;

        Transform t;
        t.position = entity.transform.position + entity.collision.localOffset;
        t.scale = entity.collision.halfExtents * 2.0f;

        glm::mat4 model = t.GetModelMatrix();

        shader.SetMat4("model", model);
        shader.SetVec3("objectColor", glm::vec3(0.48f, 0.99f, 0.0f));
        shader.SetBool("useVertexColor", false);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        cube.Draw();
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    };

    auto DrawPlayerCapsule = [&](const Entity& player)
        {
            if (player.collision.type != CollisionShape::Type::Capsule)
                return;

            const float radius = player.collision.capsule.radius;
            const float height = player.collision.capsule.height;

            const glm::vec3 position = player.transform.position + player.collision.localOffset;

            const float cylinderHeight = height - 2.0f * radius;

            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

            // ---- Cylinder
            Transform t;
            t.position = position + glm::vec3(0.0f, radius + cylinderHeight * 0.5f, 0.0f);
            t.scale = glm::vec3(radius * 2.0f, cylinderHeight, radius * 2.0f);

            shader.SetMat4("model", t.GetModelMatrix());
            shader.SetVec3("objectColor", glm::vec3(0.48f, 0.99f, 0.0f));
            shader.SetBool("useVertexColor", false);
            cube.Draw();

            // ---- Bottom sphere
            t.position = position + glm::vec3(0.0f, radius, 0.0f);
            t.scale = glm::vec3(radius * 2.0f);
            shader.SetMat4("model", t.GetModelMatrix());
            cube.Draw();

            // ---- Top sphere
            t.position = position + glm::vec3(0.0f, radius + cylinderHeight, 0.0f);
            shader.SetMat4("model", t.GetModelMatrix());
            cube.Draw();

            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        };

    bool showWorld = true;
    bool showCollision = false;
    bool showPlayerCapsule = false;

    bool wasCtrlTDown = false;
    bool wasCtrlCDown = false;
    bool wasCtrlPDown = false;

#endif

    glEnable(GL_DEPTH_TEST);

    while (glfwWindowShouldClose(window) == GLFW_FALSE)
    {
        Time::Update();

#ifdef ENGINE_DEBUG

        bool ctrlDown = Input::IsKeyPressed(GLFW_KEY_LEFT_CONTROL) || Input::IsKeyPressed(GLFW_KEY_RIGHT_CONTROL);

        bool tDown = Input::IsKeyPressed(GLFW_KEY_T);

        bool ctrlTDown = ctrlDown && tDown;

        if (ctrlTDown && !wasCtrlTDown)
        {
            showWorld = !showWorld;
        }

        wasCtrlTDown = ctrlTDown;

        bool cDown = Input::IsKeyPressed(GLFW_KEY_C);
        bool ctrlCDown = ctrlDown && cDown;

        if (ctrlCDown && !wasCtrlCDown)
        {
            showCollision = !showCollision;
        }

        wasCtrlCDown = ctrlCDown;

        bool pDown = Input::IsKeyPressed(GLFW_KEY_P);
        bool ctrlPDown = ctrlDown && pDown;

        if (ctrlPDown && !wasCtrlPDown)
        {
            showPlayerCapsule = !showPlayerCapsule;
        }

        wasCtrlPDown = ctrlPDown;

#endif

        if (Input::IsKeyPressed(GLFW_KEY_ESCAPE))
        {
            glfwSetWindowShouldClose(window, GLFW_TRUE);
        }

        camera.Update();

        PlayerMovement(player, worldEntities, camera, playerMovementSpeed);

        pivot =
            player.transform.position +
            player.collision.localOffset +
            glm::vec3(0.0f, player.collision.capsule.height * 0.5f, 0.0f);

        camera.UpdateThirdPerson(pivot, cameraDistance, CameraHeight, MinDegree, MaxDegree);

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

#ifdef ENGINE_DEBUG

        if (showWorld)
        {
            for (const Entity* entity : worldEntities)
            {
                DrawEntity(*entity);
            }
        }
        else
        {
            DrawEntity(player);
        }

        if (showCollision)
        {
            for (const Entity* entity : worldEntities)
            {
                DrawCollisionAABB(*entity);
            }
        }

        if (showPlayerCapsule)
        {
            DrawPlayerCapsule(player);
        }

#else

        for (const Entity* entity : worldEntities)
        {
            DrawEntity(*entity);
        }

#endif

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
