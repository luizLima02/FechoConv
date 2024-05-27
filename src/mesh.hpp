#pragma once

#include<glad/glad.h>
#include<new>
#include<stdlib.h>
//render
#include"shader.hpp"

struct Vertex{
    float px; float py; //posicoes


    bool operator==(const Vertex& other) const
    {
        return px == other.px && py == other.py;
    }

    bool operator!=(const Vertex& other) const
    {
        return px != other.px || py != other.py;
    }

    bool operator<=(const Vertex& other) const
    {
        return px <= other.px;
    }
};

/*Os Vertices tem que ser definidos em ordem anti-horaria*/
class Mesh{
    private:
        Vertex* vertices;
        unsigned qntVertices;
        //buffers
        GLuint VAO;
        GLuint VBO;

        void initBuffers(){
            glCreateVertexArrays(1, &this->VAO);
            glBindVertexArray(this->VAO);
            glGenBuffers(1, &this->VBO);
            glBindBuffer(GL_ARRAY_BUFFER, this->VBO);
            glBufferData(GL_ARRAY_BUFFER, this->qntVertices * sizeof(Vertex), this->vertices, GL_STATIC_DRAW);
            //envio os dados para a placa
            //posicoes
            glEnableVertexAttribArray(0);
            glVertexAttribPointer(0, 
                                  2,
                                  GL_FLOAT,
                                  false, 
                                  2*sizeof(float), 
                                  (void*)(0*sizeof(float)));
            glBindVertexArray(0);
        }

    public:
        //contrutores passando vertices

        Mesh(Vertex* vertices, const unsigned & qntVertices){
            this->qntVertices = qntVertices;
            //alocando memoria
            this->vertices = new Vertex[this->qntVertices];
            for (int i = 0; i < this->qntVertices; i++)
            {
                this->vertices[i] = vertices[i];
            }
            this->initBuffers();
        }

/*
        Mesh(const Mesh& obj){
            this->qntVertices = obj.qntVertices;
            glm::mat4 transform = glm::mat4(1, 0, 0,  0, 0, 1, 0,  0,0, 0, 1, 0, 0, 0, 0,  1);
            this->transform = transform;
            this->vertices = new Vertex[this->qntVertices];
            for (int i = 0; i < this->qntVertices; i++)
            {
                this->vertices[i] = obj.vertices[i];
            }
            this->initBuffers();
        }
*/

        void printVertices(){
            for(int i = 0; i < qntVertices; i++){
                std::cout << this->vertices[i].px  << " " << this->vertices[i].py << "\n";
            }
        }

        ~Mesh(){
            delete[] vertices;
            glDeleteVertexArrays(1, &this->VAO);
            glDeleteBuffers(1, &this->VBO);
        } 

        void render(Shader *s, int r){
            s->use();
            s->setInt("original", r);
            glBindVertexArray(this->VAO);
            //desenha
            glDrawArrays(GL_LINE_LOOP , 0, this->qntVertices);
            glBindVertexArray(0);
        }

        void render(Shader *s, int r, int p){
            s->use();
            s->setInt("original", r);
            glBindVertexArray(this->VAO);
            //desenha
            glPointSize(p);
            glDrawArrays(GL_POINTS , 0, this->qntVertices);
            glBindVertexArray(0);
        }
        
};


