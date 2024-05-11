#include"fecho.hpp"
#include<stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define WIDTH 800
#define HEIGHT 800

int main(){


    if(!glfwInit())
        return 1;

    //Window Hints
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);


    /* Create a windowed mode window and its OpenGL context */
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fecho Convexo", NULL, NULL);
    
    if (!window)
    {
        glfwTerminate();
        return 1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);
    //glfwSetWindowPos(window, 1, 31);
   
    if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
        std::cerr << "Failed to initialize GLAD\n";
        return 1;
    }
    glClearColor(0,0,0,0);
    glViewport(0, 0, WIDTH, HEIGHT);

    //Loop da Janela
    int frames = 0;
    double lastTime = glfwGetTime();
    const int FPS = 60;
    const double frameDuration = 1.0 / FPS;
    double last_frame = glfwGetTime();
    while (!glfwWindowShouldClose(window))
    {
        double now = glfwGetTime();
        double deltaTime = now - last_frame;
        if(deltaTime >= frameDuration){
            last_frame = now;

            frames++;
            if (now - lastTime >= 1.0) {
                frames = 0;
                lastTime += 1.0;
            }
            glfwPollEvents();
            
            //Renderizar pontos abaixo

            //swap
            glfwSwapBuffers(window);
        }
    }

    glfwTerminate();

    return 0;
}