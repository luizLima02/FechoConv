#pragma once
#ifndef OBJETO_H
#define OBJETO_H

#include<iostream>
#include<stdlib.h>
#include<new>
#include<vector>
#include<string>
#include<fstream>
#include"mesh.hpp"
#include"shader.hpp"

using std::cout; using std::nothrow;
using std::string;
using std::vector;


//parte de leitura de obj
struct vectorInds{
    int val1, val2, val3;  
};

struct Ret{
    std::string val;
    int barraPos;
};

Ret getNumero(std::string valor, int posI){
    Ret r;
    std::string p = "";
    int indice = posI;
    for(int i = posI; i < valor.size(); i++){
        indice = i;
        if(valor.substr(i,1) == "/"){
            break;
        }
        p += valor.substr(i,1);
    }
    r.val = p;
    r.barraPos = indice;
    return r;
}

vectorInds processarString(std::string valor){
    vectorInds vet;
    //vertice
    Ret rv = getNumero(valor,0);
    if(rv.val == ""){return vet;}
    vet.val1 = stoi(rv.val);
    //textura
    Ret rt = getNumero(valor, rv.barraPos+1);
    if(rt.val == ""){vet.val3 = 1;}else{vet.val3 = stoi(rt.val);}
    //normais
    Ret rn = getNumero(valor, rt.barraPos+1);
    if(rn.val == ""){vet.val2 = 1;}else{vet.val2 = stoi(rn.val);}
    return vet;
}

struct Vect3{
    float x, y, z;
};

struct Vect2{
    float u, v;
};

//so lê objetos triangularizados
static std::vector<Mesh*> lerObj(std::string filename){
            std::vector<Mesh*> Malha;
            std::vector<Vertex> vertex;
            bool readingGroup = false;
            //tenta abrir o arquivo
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return Malha;
            }
            //pega uma linha
            std::string line;
            while (std::getline(file, line)) 
            {
                //armazena o conteudo da linha em um buffer
                std::istringstream iss(line);
                std::string prefix;
                //pego o conteudo da linha ate o primeiro tab
                iss >> prefix;
                //cout << prefix << "\n";
                if(prefix == "g"){
                    if(readingGroup){
                        //cria mesh
                        Malha.push_back(new Mesh(vertex.data(), vertex.size()));
                        //zera todos os vetores
                        vertex.clear();
                    }else{
                        readingGroup = true;
                    }
                }
                //verifico o prefixo da linha
                if (prefix == "v") { //vertice
                    Vertex ver;
                    iss >> ver.px >> ver.py ;
                    //cout << ver.px << " - " << ver.py << "\n";
                    vertex.push_back(ver);
                }
            }
            Malha.push_back(new Mesh(vertex.data(), vertex.size()));
            //zera todos os vetores
            vertex.clear();
            return Malha;
}


class Model
{
private:

public:
    std::vector<Mesh*> meshes;

    //// Contrutores baseados em leitura de arquivo
    Model() {
        
    }
    //path to model, path to texture
    Model(string pathModel){
        std::vector<Mesh*> msh = lerObj(pathModel);
        for(int i = 0; i < msh.size(); i++){
            this->meshes.push_back(msh[i]);
        }
        //nao leu nada
        /*if(vertices.empty()){ 
            return;
        }
        this->meshes.push_back(new Mesh(vertices.data(), vertices.size()));*/
    }

    Model(std::vector<Mesh*> mesh){
        for(int i = 0; i < mesh.size(); i++){
            this->meshes.push_back(mesh[i]);
        }

    }

//////////////////////////////////////////////

    void printV(){
        int k = 0;
        for(auto&i: this->meshes){
            cout << "Group " << k << "\n";
            i->printVertices();
            k++;
        }
    }

    ~Model(){
        for(auto* &i: this->meshes){
            delete i;
        }
    }

    //renderizador
    void render(Shader* s, GLenum modo, int r){
        for(auto&i: this->meshes){
            i->render(s, modo, r);
        }
    }

    void writeOBJ(string path){

        std::fstream f;
        f.open(path, std::ios::out);
        f << "# Fecho Convexo MC " << std::endl;
        f << "# " << std::endl;
        f << "# Fecho A seguir" << std::endl;
        f << "# " << std::endl;
        f << "o output " << std::endl;
        int k = 1;
        for(auto &i: this->meshes){
            f << "  g " << k << std::endl;
            f << i->getVertices();
            k++;
        }
        f.close();
    }

    void addValNew(Vertex* arrayV, int &n, Vertex val){
        for(int i = 0; i < n; i++){
            if(arrayV[i].px == val.px && arrayV[i].py == val.py){
                return;
            }
        }
        arrayV[n] = val;
        n++;
    }

    int GetValArr(Vertex* arrayV, int n, Vertex val){
        for(int i = 0; i < n; i++){
            if(arrayV[i].px == val.px && arrayV[i].py == val.py){
                return i;
            }
        }
        return -1;
    }

    void writeTriangOBJ(string path){
        std::fstream f;
        f.open(path, std::ios::out);
        f << "# Fecho Convexo MC " << std::endl;
        f << "# " << std::endl;
        f << "# Fecho A seguir" << std::endl;
        f << "# " << std::endl;
        f << "o output " << std::endl;
        //lista de vertices unicos
        int k = 0;
        for(auto &i: this->meshes){
            k += i->getSize();
        }
        Vertex* verticesOBJ = new Vertex[k];
        int n = 0;
        for(auto &i: this->meshes){
            for(int j = 0; j < i->getSize(); j++){
                addValNew(verticesOBJ, n, i->getVertex()[j]);
            }
        }
        cout <<"qnt vertices: " << k << "\n";
        cout <<"qnt vertices unicos: " << n << "\n";
        for(int k = 0; k < n; k++){
            //escreve os vertices para o arquivo
            f << "v " << verticesOBJ[k].px << " " << verticesOBJ[k].py << " " << 0.0 << std::endl;
        }
        //escreve as faces
        for(auto &i: this->meshes){
            f << "f ";
            for(int j = 0; j < i->getSize(); j++){
                Vertex vert = i->getVertex()[j];
                int indice = GetValArr(verticesOBJ, n, vert);
                f << indice+1 << " ";
            }
            f << std::endl;
        }
        f << "\0";
        f.close();
    }

};


#endif