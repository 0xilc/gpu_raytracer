#include "App.h"
#include "Input.h"
#define _USE_MATH_DEFINES
#include <math.h>
#include <iostream>
#include "Parser.h"
#include "Vec3.h"
#include "BVH.h"
#include "Sphere.h"
#include "Triangle.h"
#include "Utils.h"
#include "GPUStructs.h"
#include <vector>

parser::Scene scene;

App::App()
{
    s_WindowState = WindowState(1000, 750, "OpenGL Ray Tracer");
    Init();
}

App::~App()
{
    glfwDestroyWindow(s_WindowState.window);
    glfwTerminate();
}

void App::Init()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif
    if (!glfwInit())
    {
        exit(-1);
    }

    s_WindowState.window = glfwCreateWindow(s_WindowState.width, s_WindowState.height, s_WindowState.title.c_str(), NULL, NULL);

    if (!s_WindowState.window)
    {
        glfwTerminate();
        exit(-1);
    }

    glfwMakeContextCurrent(s_WindowState.window);
    // glfwSwapInterval(1);

    // Initialize GLEW to setup the OpenGL Function pointers
    if (GLEW_OK != glewInit())
    {
        std::cout << "Failed to initialize GLEW" << std::endl;
        exit(-1);
    }

    glfwSetWindowUserPointer(s_WindowState.window, this);

    // glfw state
    glfwSwapInterval(0);
    glEnable(GL_DEPTH_TEST);

    // GLFW Events
    glfwSetWindowSizeCallback(s_WindowState.window, [](GLFWwindow* window, int width, int height)
        {
            glViewport(0, 0, width, height);
            App* app = (App*)glfwGetWindowUserPointer(window);
            s_WindowState.width = width;
            s_WindowState.height = height;
            app->m_Camera->OnResize(width, height);
        });

    // Key callbacks
    glfwSetKeyCallback(s_WindowState.window, [](GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        App* app = (App*)glfwGetWindowUserPointer(window);
        if (action == GLFW_PRESS || action == GLFW_REPEAT)
        {
            switch (key)
            {
            case GLFW_KEY_ESCAPE:
                glfwSetWindowShouldClose(window, true);
                break;
            default:
                break;
            }
        }
    });

    // Input setup
    Input::Init(s_WindowState.window);

    // Camera setup
    m_Camera = std::make_unique<Camera>(s_WindowState.width, s_WindowState.height, 90.0f);

    // UBO setup
    m_UBO = std::make_unique<UBO>();

    // SSBO setup
    m_SSBO = std::make_unique<SSBO>();
    
    // Quad vertices
    GLfloat quadVertices[] = {
        // Positions        // Texture Coords
        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,

        -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
         1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
         1.0f,  1.0f, 0.0f, 1.0f, 1.0f
    };

    // Init quad
    GLuint VBO;
    glGenVertexArrays(1, &m_QuadVAO);
    glGenBuffers(1, &VBO);
    glBindVertexArray(m_QuadVAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), quadVertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (GLvoid*)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(1);
    glBindVertexArray(0);

    // Ray tracing shader
    m_RayTracingShader = std::make_shared<Shader>("assets/shaders/rt.vert", "assets/shaders/rt.frag");
}

void App::Run()
{
    // Load scene and create bvh tree
    std::string scenePath = "./assets/scenes/monkey.xml";
    scene.loadFromXml(scenePath);
    std::vector<std::shared_ptr<Hittable>> objects;
    for (const parser::Sphere& sphere : scene.spheres)
    {
        objects.push_back(std::make_shared<Sphere>(sphere));
    }
    for (const parser::Triangle& triangle : scene.triangles)
    {
        objects.push_back(std::make_shared<Triangle>(triangle));
    }
    for (const parser::Mesh& mesh : scene.meshes)
    {
        for (const parser::Face& face : mesh.faces)
        {
            objects.push_back(std::make_shared<Triangle>(face, mesh.material_id));
        }
    }
    std::shared_ptr<Hittable> world = std::make_shared<BVHNode>(objects, 0, objects.size() - 1);
    std::vector<GPU::BVHNode> flatBVH;
    std::vector<GPU::Primitive> primitives;
    std::vector<GPU::Material> materials;
    std::vector<GPU::Light> lights;
    

    ExtractMaterials(materials, scene);
    ExtractLights(lights, scene);
    FlattenBVH(world, flatBVH, primitives);

    // Pass BVHnodes to SSBO
    size_t bvhSize = flatBVH.size() * sizeof(GPU::BVHNode);
    size_t primitivesSize = primitives.size() * sizeof(GPU::Primitive);
    size_t materialsSize = materials.size() * sizeof(GPU::Material);
    size_t lightsSize = lights.size() * sizeof(GPU::Light);
    m_SSBO->CreateSSBO("BVHNodes", SSBOBindingPoints::BVHNodes, bvhSize);
    m_SSBO->UpdateSSBO("BVHNodes", 0, bvhSize, flatBVH.data());

    m_SSBO->CreateSSBO("Primitives", SSBOBindingPoints::Primitives, primitivesSize);
    m_SSBO->UpdateSSBO("Primitives", 0, primitivesSize, primitives.data());

    m_SSBO->CreateSSBO("Materials", SSBOBindingPoints::Materials, materialsSize);
    m_SSBO->UpdateSSBO("Materials", 0, materialsSize, materials.data());

    m_SSBO->CreateSSBO("Lights", SSBOBindingPoints::Lights, lightsSize);
    m_SSBO->UpdateSSBO("Lights", 0, lightsSize, lights.data());

    m_UBO->CreateUBO("CameraData", UBOBindingPoints::CAMERA_DATA, 4 * sizeof(glm::vec4));
    double currentFrame = glfwGetTime();
    double lastFrame = currentFrame;
    double deltaTime;

    while (!glfwWindowShouldClose(s_WindowState.window))
    {
        currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        s_WindowState.fps = 1.0f / deltaTime;

        Update(deltaTime);
        Render();

        glfwSwapBuffers(s_WindowState.window);
        glfwPollEvents();
    }
}

void App::Update(float deltaTime)
{
    ProcessInput();
    m_Camera->Update(deltaTime);
    m_Camera->SetUniforms(*m_UBO);
    std::string title = "FPS: " + std::to_string(s_WindowState.fps);
    glfwSetWindowTitle(s_WindowState.window, title.c_str());
}

void App::Render()
{
    glViewport(0, 0, s_WindowState.width, s_WindowState.height);
    glClearColor(0, 0, 0, 1);
    glClearDepth(1.0f);
    glClearStencil(0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

    // Render quad
    m_RayTracingShader->Use();
    m_RayTracingShader->SetUniform1i("u_ScreenWidth", s_WindowState.width);
    m_RayTracingShader->SetUniform1i("u_ScreenHeight", s_WindowState.height);
    glBindVertexArray(m_QuadVAO);
    glDrawArrays(GL_TRIANGLES, 0, 6);
    glBindVertexArray(0);

}

void App::ProcessInput()
{
}
