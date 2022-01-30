#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>

#include "Renderer.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw.h"
#include "imgui/imgui_impl_opengl3.h"


int main()
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    // Request OpenGL 3.3 without obsoleted functions.
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window, true);
    ImGui_ImplOpenGL3_Init("#version 330 core");

    if (glewInit() != GLEW_OK)
    {
        std::cout << "Failed to init GLEW" << std::endl;
        return -1;
    }

    std::cout << glGetString(GL_VERSION) << std::endl;
    
    float positions[] = {
        -50.0f,  -50.0f, 0.0f, 0.0f, // 0
         50.0f, -50.0f, 1.0f, 0.0f, // 1
         50.0f,  50.0f, 1.0f, 1.0f, // 2
        -50.0f,  50.0f, 0.0f, 1.0f, // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0,
    };

    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    VertexArray va;
    VertexBuffer vb(positions, 4 * 4 * sizeof(GLfloat));

    VertexBufferLayout layout;
    layout.Push<float>(2);
    layout.Push<float>(2);
    va.AddBuffer(vb, layout);

    IndexBuffer ib(indices, 6);

    glm::mat4 proj = glm::ortho(0.0f, 960.f, 0.0f, 540.0f, -0.1f, 100.0f);
    glm::mat4 view = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f));

    Shader shader("res/shaders/Basic.shader");
    shader.Bind();

    Texture texture("res/textures/Logo.png");
    texture.Bind(0);

    shader.SetUniform1i("u_Texture", 0);

    // Unbind everything
    va.Unbind();
    vb.Unbind();
    ib.Unbind();
    shader.Unbind();

    Renderer renderer;

    glm::vec3 translationA(200.0f, 200.0f, 0.0f);
    glm::vec3 translationB(400.0f, 200.0f, 0.0f);

    float r = 0.0f;
    float increment = 0.01f;
    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        renderer.Clear();

        // Start the Dear ImGui frame
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        shader.Bind();

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationA);
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader);
        }

        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), translationB);
            glm::mat4 mvp = proj * view * model;
            shader.SetUniformMat4f("u_MVP", mvp);
            renderer.Draw(va, ib, shader);
        }

        if (r < 0.0f || r > 1.0f)
            increment *= -1;

        r += increment;

        {
            ImGui::SliderFloat2("TranslationA", &translationA.x, 0.0f, 960.0f);
            ImGui::SliderFloat2("TranslationB", &translationB.x, 0.0f, 960.0f);
            ImGui::Text(
                "Application average %.3f ms/frame (%.1f FPS)",
                1000.0f / ImGui::GetIO().Framerate,
                ImGui::GetIO().Framerate
            );
        }

        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwTerminate();
    return 0;
}