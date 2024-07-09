#include "fecho.hpp"
#include "objeto.hpp"
#include "triang.hpp"
#include <stdlib.h>
#include <iostream>
#include<new>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define WIDTH 800
#define HEIGHT 800
#define MENORP 2
#define MAIORP 15

void handleUserInput(int& command, int& numberOfPoints);
void renderScene(GLFWwindow* window, int command, const std::vector<Vertex>& points, Model& convexHull, Model& theme, Shader* shader);
void processInput(GLFWwindow* window);
void renderPoints(Shader* shader, const std::vector<Vertex>& points);
void renderConvexHull(Shader* shader, Model& convexHull);
void renderTheme(Shader* shader, Model& theme);
std::vector<Vertex> generateRandomPoints(int numberOfPoints);
std::vector<Vertex> createHexagonVertices();
std::vector<Vertex> computeConvexHull(Vertex* points, int size, bool isQuickHull);

static bool ShowPontos = true;
static bool OriginalShow = true;
static int PontoSize = 5;

void processInput(GLFWwindow* window){
    // Fecha a janela
    if(glfwGetKey(window, GLFW_KEY_ESCAPE)){
        glfwSetWindowShouldClose(window, 1);
    }
    // Mostra o fecho convexo como pontos
    if(glfwGetKey(window, GLFW_KEY_P)){
        ShowPontos = true;
    }
    // Mostra o fecho convexo como linhas
    if(glfwGetKey(window, GLFW_KEY_L)){
        ShowPontos = false;
    }
    // Mostra a figura original
    if(glfwGetKey(window, GLFW_KEY_O)){
        OriginalShow = true;
    }
    // Esconde a figura original
    if(glfwGetKey(window, GLFW_KEY_H)){
        OriginalShow = false;
    }
    // Diminui o tamanho dos pontos
    if(glfwGetKey(window, GLFW_KEY_D)){
        if(PontoSize > MENORP){
            PontoSize--;
        }
    // Aumenta o tamanho dos pontos
    }else if(glfwGetKey(window, GLFW_KEY_A)){
        if(PontoSize < MAIORP){
            PontoSize++;
        }
    }
}

void handleUserInput(int& command, int& numberOfPoints, bool& isQuickHull) {
    std::cout << "1) Testar com hexagono\n";
    std::cout << "2) Testar com pontos aleatorios\n";
    std::cout << "3) Testar com tema\n";
    std::cout << "Digite um comando: ";
    std::cin >> command;

    if (command == 2) {
        std::cout << "Numero de pontos: ";
        std::cin >> numberOfPoints;
    }
    std::cout << "Algoritmo\n";
    std::cout << "1) QuickHull\n";
    std::cout << "2) Graham\n";
    std::cout << "Digite um comando: ";
    int algoritmo = 0;
    std::cin >> algoritmo;
    isQuickHull = (algoritmo == 1);
}

std::vector<Vertex> generateRandomPoints(int numberOfPoints) {
    std::vector<Vertex> points(numberOfPoints);
    for (int i = 0; i < numberOfPoints; ++i) {
        points[i].px = (float) (2.0 * ((double)rand() / RAND_MAX) - 1.0);
        points[i].py = (float) (2.0 * ((double)rand() / RAND_MAX) - 1.0);
    }
    return points;
}

std::vector<Vertex> createHexagonVertices() {
    std::vector<Vertex> hexagonVertices(6);
    float radius = 0.5f;
    for (int i = 0; i < 6; ++i) {
        float angle = M_PI / 3 * i;
        hexagonVertices[i] = { (float) (radius * cos(angle)), (float) (radius * sin(angle))};
    }
    return hexagonVertices;
}

std::vector<Vertex> computeConvexHull(Vertex* points, int size, bool isQuickHull) {
    if (isQuickHull) {
        return QuickHull(points, size);
    } else {
        return Graham(points, size);
    }
}

Model* Triangulate(Model* modelo)
{
    int size = 0;
    vector<Mesh*> meshs;
    for(auto m: modelo->meshes){
        //pegar os vertices de cada mesh
        int n = m->getSize();
        size = m->getSize() + 3;
        Vertex* vertices = m->getVertex();
        Vertex* v_temp = new(nothrow)Vertex[size];
        for(int i = 0; i < n; i++){
            v_temp[i] = vertices[i];
        }
        //criar vetor v de triangulos
        Triang* v = new(nothrow)Triang[3*n];
        int ntri = 0;
        //ordena os pontos
        qsort(v_temp, n, sizeof(Vertex), VertexCompare);
        delunayTriang(v_temp, n, v, ntri);
        //para ntri:
        for(int i = 0; i < ntri; i++){
            Vertex p1 = v_temp[v[i].p1];
            Vertex p2 = v_temp[v[i].p2];
            Vertex p3 = v_temp[v[i].p3];
            //  crio array de vertice
            Vertex* triangulo = new Vertex[3];
            //  adiciono os pontos p1 p2 p3 para o array
            triangulo[0] = p1;
            triangulo[1] = p2;
            triangulo[2] = p3;
            //adiciono o *mesh(array de vertice, 3) para o vetor de arrays
            meshs.push_back(new Mesh(triangulo, 3));
            delete[] triangulo;
        }
        cout << "numero de triangulos: " << ntri << "\n";
        delete[] v;
        delete[] v_temp;
    }
    //delunayTriang(nullptr, 3);
    
    //crio o novo modelo triangularizado
    //retorno o modelo triangularizado
    Model *modeloTriangulado = new Model(meshs);
    return modeloTriangulado;
}

GLFWwindow* initWindow() 
{
    if (!glfwInit()) return nullptr;

    // Window Hints
    glfwWindowHint(GLFW_RESIZABLE, false);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 1);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    // Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(WIDTH, HEIGHT, "Fecho Convexo", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return nullptr;
    }

    // Make the window's context current
    glfwMakeContextCurrent(window);
   
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cerr << "Failed to initialize GLAD\n";
        return nullptr;
    }

    return window;
}

void renderPoints(Shader* shader, const std::vector<Vertex>& points) 
{
    if (!ShowPontos) return;
    glPointSize(PontoSize);
    Mesh mesh(const_cast<Vertex*>(points.data()), points.size());
    mesh.render(shader, GL_POINTS, 1);
}

void renderConvexHull(Shader* shader, Model& convexHull) 
{
    glLineWidth(2);
    glPointSize(PontoSize);
    convexHull.render(shader, GL_LINE_LOOP, 0);
    convexHull.render(shader, GL_POINTS, 0);
}

void renderTheme(Shader* shader, Model& theme) 
{
    if (!OriginalShow) return;
    glLineWidth(8);
    theme.render(shader, GL_LINE_LOOP, 1);
}


void renderScene(GLFWwindow* window, int command, const std::vector<Vertex>& points, Model& convexHull, Model& theme, Shader* shader) 
{

    glClearColor(0.1, 0.1, 0.1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);

    // Loop da Janela
    int frames = 0;
    double lastTime = glfwGetTime();
    const int FPS = 60;
    const double frameDuration = 1.0 / FPS;
    double last_frame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double deltaTime = now - last_frame;
        if (deltaTime >= frameDuration) {
            last_frame = now;

            frames++;
            if (now - lastTime >= 1.0) {
                frames = 0;
                lastTime += 1.0;
            }

            processInput(window);
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT);

            if (command == 3) {
                renderTheme(shader, theme);
            }
            renderPoints(shader, points);
            renderConvexHull(shader, convexHull);
    
            glfwSwapBuffers(window);
        }
    }

    delete shader;
}

void render(Shader* shader, Model* modelo){
    modelo->render(shader, GL_LINE_LOOP, 1);
}

int main() {
    /**
    int command = 0;
    int numberOfPoints = 0;
    bool isQuickHull = false;

    handleUserInput(command, numberOfPoints, isQuickHull);

    GLFWwindow* window = initWindow();
    if (!window) return 1;

    std::vector<Vertex> points;
    if (command == 1) {
        points = createHexagonVertices();
    }
    else if (command == 2) {
        points = generateRandomPoints(numberOfPoints);
    }
    else {
        ShowPontos = false;
    }
    
    vector<Mesh*> meshVector;
    Model* theme;
     
    if (command == 3) {
        theme = new Model("../../OBJ/container.obj");
        for (auto& mesh : theme->meshes) {
            auto fmesh = computeConvexHull(mesh->getVertex(), mesh->getSize(), isQuickHull);
            meshVector.push_back(new Mesh(fmesh.data(), fmesh.size()));
            for (const auto& vertex : fmesh) {
                points.push_back(vertex);
            }
        }
    } else {
        auto fmesh = computeConvexHull(points.data(), points.size(), isQuickHull);
        meshVector.push_back(new Mesh(fmesh.data(), fmesh.size()));
    }
    Model convexHull = Model(meshVector);
    // Initialize shader
    Shader* shader = new Shader("../../Shaders/baseShader.vert", "../../Shaders/baseShader.frag");

    // Main render loop
    while (!glfwWindowShouldClose(window)) {
        renderScene(window, command, points, convexHull, *theme, shader);
    }

    delete shader;
    glfwTerminate();
    */
    GLFWwindow* window = initWindow();
    if (!window) return 1;

    Model *mod = new Model("../../OBJ/container.obj");
    auto triangulado = Triangulate(mod);

    // Initialize shader
    Shader* shader = new Shader("../../Shaders/baseShader.vert", "../../Shaders/baseShader.frag");

    glClearColor(0.1, 0.1, 0.1, 1);
    glViewport(0, 0, WIDTH, HEIGHT);

    // Loop da Janela
    int frames = 0;
    double lastTime = glfwGetTime();
    const int FPS = 60;
    const double frameDuration = 1.0 / FPS;
    double last_frame = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        double now = glfwGetTime();
        double deltaTime = now - last_frame;
        if (deltaTime >= frameDuration) {
            last_frame = now;

            frames++;
            if (now - lastTime >= 1.0) {
                frames = 0;
                lastTime += 1.0;
            }

            processInput(window);
            glfwPollEvents();
            glClear(GL_COLOR_BUFFER_BIT);
            //renderizar aqui
            render(shader, triangulado);
    
            glfwSwapBuffers(window);
        }
    }
    triangulado->writeTriangOBJ("../../OBJ/cavaloTriang.obj");
    delete shader;
    glfwTerminate();
    system("pause");
    return 0;
}
