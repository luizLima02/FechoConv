#pragma once
#ifndef FECHO_H
#define FECHO_H

//editar esse arquivo e chamar as funcoes no main
#include<iostream>
#include<new>
#include <algorithm>
#include <utility>
#include <cmath>
#include<vector>
#include "shader.hpp"
#include "mesh.hpp"
#include "merge.hpp"


using std::cout; using std::nothrow; using std::vector;


static int Contador = 0;

double distPontos(Vertex proj, Vertex k){
    return ((proj.px-k.px)*(proj.px-k.px)) + ((proj.py-k.py)*(proj.py-k.py));
}

int getXmax(Vertex* vertices, int n){
    int xmaximo = 0;
    for (int i = 1; i < n; i++)
    {
        if(vertices[xmaximo].px < vertices[i].px || 
          (vertices[xmaximo].px == vertices[i].px && vertices[xmaximo].py < vertices[i].py))
        {
            xmaximo = i;
        }
    }
    return xmaximo;
}

int getXmin(Vertex* vertices, int n){
    int xminimo = 0;
    for (int i = 1; i < n; i++)
    {
        if(vertices[xminimo].px > vertices[i].px || 
          (vertices[xminimo].px == vertices[i].px && vertices[xminimo].py > vertices[i].py))
        {
            xminimo = i;
        }
    }
    return xminimo;
}

/* 0 -> left, 1 -> right*/
int sidePoint(Vertex pi, Vertex pf, Vertex ver){
    double val = (pf.py - pi.py) * (ver.px - pf.px) - (pf.px - pi.px) * (ver.py - pf.py);
    
    if(val > 0)
    {
        return -1;
    }
    if(val < 0){
        return 1;
    }

    return 0;
}

double lineDist(Vertex p1, Vertex p2, Vertex p) {
    //leva para a origem;
    Vertex j = {(p2.px-p1.px), (p2.py-p1.py)};
    Vertex k = {(p.px-p1.px), (p.py-p1.py)};

    double a = (j.px * k.px) + (j.py * k.py);
    double b = (j.px * j.px) + (j.py * j.py);
    if(b == 0){ b = 1;}
    double val = a/b;

    Vertex proj = {(float)(j.px*val), (float)(j.py*val)};

    return distPontos(proj, k);
    //return abs ((p.py - p1.py) * (p2.px - p1.px) - (p2.py - p1.py) * (p.px - p1.px));
}


bool hasValue(vector<Vertex> vec, Vertex val){
    for(auto i: vec){
        if(i == val){
            return true;
        }
    }
    return false;
}

void QuickHullAux(Vertex* vertices, int n, Vertex p1, Vertex p2, int side, vector<Vertex>& fecho)
{
    Contador++;
    int ind = -1;
    double dist_max = 0;
    cout << Contador << "\n";
    //calcula a distancia da linha p1 - p2 para o ponto[i]
    for (int i = 0; i < n; i++)
    {
        double distancia = lineDist(p1, p2, vertices[i]);
        //cout << "distancia: " << distancia << "\n";
        //se o ponto estiver na direcao que esta sendo verificada e estiver mais longe doque a distancia
        //maxima o indice sera esse ponto e a distancia maxima sera ela
        if(sidePoint(p1, p2, vertices[i]) == side && distancia > dist_max){
            dist_max = distancia;
            ind = i;
        }
    }

    if(hasValue(fecho, vertices[ind])){
        return;
    }

    //cout << "Max dist: " << dist_max << "\n";
    //se não tiver ponto mais ao lado que estou verificando adicione os dois pontos para o fecho
    //e retorne
    if(ind == -1){
        return;
    }else{
        if(hasValue(fecho, vertices[ind])){
            return;
        }else{
            fecho.push_back(vertices[ind]);
        }
    }
    //passa o ponto encontrado e o primeiro ponto, o lado passado será o inverso da posicao
    //do ponto 2 em relacao aos pontos passados


    QuickHullAux(vertices, n, vertices[ind], p1, -sidePoint(vertices[ind], p1, p2), fecho);

    QuickHullAux(vertices, n, vertices[ind], p2, -sidePoint(vertices[ind], p2, p1), fecho);

    
}
//ordena pontos

float interpLinear(float p1, float p2, float perc){
    float xi = (1-perc)*p1 + perc*p2;
    return xi;
}

Vertex calculateCentralPoint(std::vector<Vertex>& vertices){
    float maxX  = vertices[0].px;//maximo x
    float maxY  = vertices[0].py;//maximo y
    float minX  =  vertices[0].px;//minimo x
    float minY  =  vertices[0].py;//minimo y
    for(int i = 0; i < vertices.size(); i++){
        float pxA = vertices[i].px;
        float pyA = vertices[i].py;
        if(pyA > maxY){
            maxY = pyA;
        }
        if(pyA < minY){
            minY = pyA;
        }
        if(pxA > maxX){
            maxY = pyA;
        }
        if(pxA < minX){
            minY = pyA;
        }
    }
    float xc = interpLinear(minX, maxX, 0.5f);
    float yc = interpLinear(minY, maxY, 0.5f);
    Vertex center = Vertex{xc, yc};
    return center;
}

Vertex centralPoint;
// Função para calcular o ângulo em relação ao ponto central
float angleFromCenter(const Vertex& point) {
    return atan2(point.py - centralPoint.py, point.px - centralPoint.px);
}

// Função de comparação para ordenar em ordem horária
bool compareClockwise(const Vertex& a, const Vertex& b) {
    // Calcular os ângulos de 'a' e 'b' em relação ao ponto central
    float angleA = angleFromCenter(a);
    float angleB = angleFromCenter(b);

    // Corrigir os ângulos negativos
    if (angleA < 0) angleA += 2 * M_PI;
    if (angleB < 0) angleB += 2 * M_PI;

    return angleA < angleB;
}

// Função para ordenar o vetor em ordem horária
void sortVerticesClockwise(std::vector<Vertex>& vertices) {
    centralPoint = calculateCentralPoint(vertices); // Definir o ponto central
    std::sort(vertices.begin(), vertices.end(), compareClockwise);
}


vector<Vertex> ordenaHorario(Vertex* pontos, int n){
    vector<Vertex> ordenados;
    Vertex inicio = pontos[0];
    ordenados.push_back(inicio);
    while (ordenados.size() < n)
    {
        double distMin = INFINITY;
        int ind = -1;
        for(int i = 0; i < n; i++){
            if(inicio != pontos[i] && hasValue(ordenados, pontos[i]) == false){
                double dist = distPontos(inicio, pontos[i]);
                if(dist < distMin){
                    distMin = dist;
                    ind = i;
                }
            }//end if
        }//end for
        ordenados.push_back(pontos[ind]);
        inicio = pontos[ind];
    }
    
    return ordenados;
}

vector<Vertex> QuickHull(Vertex* vertices, int n){
    //Vertex* fecho = new(nothrow)Vertex[n];
    cout << "Quick Hull\n";

    vector<Vertex> fecho;
    //verifica se o numero de pontos é menor que 3
    if (n <= 3) {
        for (int i=0; i<n; i++)
            fecho.push_back(vertices[i]);
        return fecho;
    }
    //achar xmax e xmin
    int pontoMax = getXmax(vertices,  n);
    int pontoMin = getXmin(vertices,  n);

    
    //pega o x minimo e maximo
    //cout << "Xmax: " << vertices[pontoMax].px << " - " << vertices[pontoMax].py << "\n";
    //cout << "Xmin: " << vertices[pontoMin].px << " - " << vertices[pontoMin].py << "\n";
    fecho.push_back(vertices[pontoMin]);
    fecho.push_back(vertices[pontoMax]);
    //divide o conjunto em 2
    //lado esquerdo
    QuickHullAux(vertices, n, vertices[pontoMin], vertices[pontoMax], -1, fecho);
    //lado direito
    QuickHullAux(vertices, n, vertices[pontoMin], vertices[pontoMax], 1, fecho);
    //sortVertices(fecho);
    //auto ord = ordenaHorario(fecho.data(), fecho.size());
    sortVerticesClockwise(fecho);
    return fecho;
}

// Encontrar ponto de referência (menor y)
Vertex findReferencePoint(vector<Vertex> vertices) {
    Vertex ref = vertices[0];
    for (auto &v : vertices) {
        if (v.py < ref.py || (v.py == ref.py && v.px < ref.px)) {
            ref = v;
        }
    }
    return ref;
}

// Compara ângulo polar em relação ao ponto de referência
bool comparePolarAngle(Vertex a, Vertex b, Vertex ref) {
    int side = sidePoint(a, b, ref);
    if (side == 0) {
        // Se os vértices forem colineares, ordene pela distância até o ponto de referência
        return distPontos(a, ref) < distPontos(b, ref);
    }
    return side > 0;
}

void sortByPolarAngle(vector<Vertex>& vertices, const std::function<bool(const Vertex&, const Vertex&)>& compare) {
    mergeSort(vertices, compare);
}

vector<Vertex> Graham(Vertex* vertices, int n) {
    cout << "Graham\n";

    // Converter array para vector
    vector<Vertex> vertexVector(vertices, vertices + n);

    // Ponto de referência (menor y, ou menor x em caso de empate)
    Vertex ref = findReferencePoint(vertexVector);

    // Ordenar vértices pelo ângulo polar de acordo com o ponto de referência
    sortByPolarAngle(vertexVector, [&](const Vertex& a, const Vertex& b) {
        return comparePolarAngle(a, b, ref);
    });

    // Construir Fecho
    vector<Vertex> fecho;

    for (int i = 0; i < n; i++) {
        // Remover vértices côncavos
        while (fecho.size() > 1 && sidePoint(fecho[fecho.size() - 1], vertexVector[i], fecho[fecho.size() - 2]) <= 0) {
            fecho.pop_back();
        }
        fecho.push_back(vertexVector[i]);
    }

    return fecho;
}

#endif