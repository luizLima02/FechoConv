#include"fecho.hpp"
#include<stdlib.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>


#define WIDTH 800
#define HEIGHT 800
#define MENORP 2
#define MAIORP 15

static bool Pontos = false;
static bool OriginalShow = true;
static int PontoSize = 5;

void processInput(GLFWwindow* window){
    //fecha a janela
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window, 1);
    }
    //mostra o fecho convexo como pontos
    if(glfwGetKey(window, GLFW_KEY_P)){
        Pontos = true;
    }
    //mostra o fecho convexo como linhas
    if(glfwGetKey(window, GLFW_KEY_L)){
        Pontos = false;
    }
    //mostra a figura original
    if(glfwGetKey(window, GLFW_KEY_O)){
        OriginalShow = true;
    }
    //esconde a figura original
    if(glfwGetKey(window, GLFW_KEY_H)){
        OriginalShow = false;
    }
    //diminui o tamanho dos pontos
    if(glfwGetKey(window, GLFW_KEY_D)){
        if(PontoSize > MENORP){
            PontoSize--;
        }
    //aumenta o tamanho dos pontos
    }else if(glfwGetKey(window, GLFW_KEY_A)){
        if(PontoSize < MAIORP){
            PontoSize++;
        }
    }
}

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

    /*Chamar funcoes aqui*/

    Vertex vertices[] = {
        0.4	,   0.15	,
        0.47,   0.0	    ,
        0.4	,   -0.2	,
        0.2	,   -0.18	,
        0.21	,-0.33	,
        0.22	,-0.42	,
        0.23	,-0.53	,
        0.16	,-0.65	,
        0.07	,-0.73	,
        -0.05	,-0.77	,
        -0.18	,-0.78	,
        -0.27	,-0.75	,
        -0.36	,-0.67	,
        -0.39	,-0.54	,
        -0.34	,-0.41	,
        -0.11	,-0.31	,
        -0.19	,-0.21	,
        -0.21	,-0.08	,
        -0.2	 ,0.05	,
        -0.17	 ,0.17	,
        -0.09	 ,0.29	,
        -0.12	 ,0.37	,
        -0.25	 ,0.39	,
        -0.4	 ,0.3	,
        -0.47	 ,0.39	,
        -0.3	 ,0.5	,
        -0.27	 ,0.72	,
        -0.2	 ,0.7	,
        -0.15	 ,0.79	,
        -0.03	 ,0.79	,
        0.07	 ,0.77	,
        0.15	 ,0.72	,
        0.2	,   0.65	,
        0.25	 ,0.56	,
        0.26	 ,0.46	,
        0.26	 ,0.26	,
        0.25	 ,0.14
    };

    auto fecho = Graham(vertices, 37);
    //auto fecho = QuickHull(vertices, 37);

    Mesh original = Mesh(vertices, 37);

    Mesh m = Mesh(fecho.data(), fecho.size());

    Shader* s = new Shader("../../Shaders/baseShader.vert", "../../Shaders/baseShader.frag");




    /*Fim chamar funcoes*/
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
            processInput(window);
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT);
            //Renderizar pontos abaixo
            if(OriginalShow){
                original.render(s, 1);
            }
            if(Pontos == true){
                m.render(s, 0, PontoSize);
            }else{
                m.render(s, 0);
            }
            //swap
            glfwSwapBuffers(window);
        }
    }

    glfwTerminate();

    return 0;
}