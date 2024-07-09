#pragma once
#ifndef TRIANGULATE
#define TRIANGULATE

#include<iostream>
#include <stdlib.h>
#include<new>
#include<algorithm>
#include<utility>
#include<cmath>
#include<vector>

#include"shader.hpp"
#include"mesh.hpp"


const double EPSILON = 0.000001;
using std::cout; using std::nothrow; using std::vector;
/*guarda as posicoes dos pontos que formam o triangulo*/
struct Triang{
    int p1;
    int p2;
    int p3;
};

/*guarda as posicoes dos pontos que formam a Edge*/
struct Edge{
    int p1;
    int p2;
};

int VertexCompare(const void *v1, const void *v2)
{
    Vertex *p1, *p2;

    p1 = (Vertex*)v1;
    p2 = (Vertex*)v2;
    if(p1->px < p2->px){
        return -1;
    }else if(p1->px > p2->px){
        return 1;
    }else{
        return 0;
    }

}

int inCircle(Vertex p, Vertex A, Vertex B, Vertex C, Vertex &Central, double &r){
    double m1, m2, mx1, mx2, my1, my2;
    double dx, dy, rsqr, drsqr;
    double xc, yc;
    /* Check for coincident points */
    if(abs(A.py - B.py) < EPSILON && abs(B.py - C.py) < EPSILON)
        return(false);
    if(abs(B.py-A.py) < EPSILON){ 
        m2 = - (C.px - B.px) / (C.py - B.py);
        mx2 = (B.px + C.px) / 2.0;
        my2 = (B.py + C.py) / 2.0;
        Central.px = (B.px + A.px) / 2.0;
        Central.py = m2 * (Central.px - mx2) + my2;
    }else if(abs(C.py - B.py) < EPSILON){ 
            m1 = - (B.px - A.px) / (B.py - A.py);
            mx1 = (A.px + B.px) / 2.0;
            my1 = (A.py + B.py) / 2.0;
            Central.px = (C.px + B.px) / 2.0;
            Central.py = m1 * (Central.px - mx1) + my1;
        }else{
            m1 = - (B.px - A.px) / (B.py - A.py); 
            m2 = - (C.px - B.px) / (C.py - B.py); 
            mx1 = (A.px + B.px) / 2.0; 
            mx2 = (B.px + C.px) / 2.0;
            my1 = (A.py + B.py) / 2.0;
            my2 = (B.py + C.py) / 2.0;
            Central.px = (m1 * mx1 - m2 * mx2 + my2 - my1) / (m1 - m2); 
            Central.py = m1 * (Central.px - mx1) + my1; 
        }
    dx = B.px - Central.px;
    dy = B.py - Central.py;
    rsqr = dx * dx + dy * dy;
    r = sqrt(rsqr); 
    dx = p.px - Central.px;
    dy = p.py - Central.py;
    drsqr = dx * dx + dy * dy;
    return((drsqr <= rsqr) ? true : false);
}

int delunayTriang(Vertex* vertices, int n, Triang *v, int &ntri){
    int *complete = NULL;
    Edge *edges   = NULL;
    Edge *p_EdgeTemp;
    int nedge = 0;
    int trimax, emax = 200;
    int status = 0;
    int inside;
    int i, j, k;
    Vertex p, A, B, C, Central;
    Vertex minV, maxV, midV;
    double dx, dy, dmax;
    double r;
    //aloca memoria para a lista de flags de cada triangulo, [i] > estado do triangulo i
    trimax = 4*n;
    complete = new(nothrow)int[trimax];
    //aloca memoria para a lista de edges
    edges = new(nothrow)Edge[emax];
    //acha os pontos de maximo e minimo, isso permite calcular o triangulo que engloba os pontos
    minV.px = vertices[0].px;
    minV.py = vertices[0].py;
    maxV.px = minV.px;
    maxV.py = minV.py;
    for(i = 1; i < n; i++)
    {
        if(vertices[i].px < minV.px){minV.px = vertices[i].px;}
        if(vertices[i].py < minV.py){minV.py = vertices[i].py;}
        if(vertices[i].px > maxV.px){maxV.px = vertices[i].px;}
        if(vertices[i].py > maxV.py){maxV.py = vertices[i].py;}
    }
    dx = maxV.px - minV.px;
    dy = maxV.py - minV.py;
    dmax = (dx > dy) ? dx : dy;
    midV.px = (maxV.px + minV.px) / 2.0;
    midV.py = (maxV.py + minV.py) / 2.0;
    /*
    Set up the supertriangle
    his is a triangle which encompasses all the sample points.
    The supertriangle coordinates are added to the end of the
    vertex list. The supertriangle is the first triangle in
    the triangle list.
    */
    //primeiro ponto
    vertices[n+0].px = midV.px - 20 * dmax;
    vertices[n+0].py = midV.py - dmax;
    //segundo ponto
    vertices[n+1].px = midV.px;
    vertices[n+1].py = midV.py + 20 * dmax;
    //terceiro ponto
    vertices[n+2].px = midV.px + 20 * dmax;
    vertices[n+2].py = midV.py - dmax;
    //primeiro triangulo
    v[0].p1 = n;
    v[0].p2 = n+1;
    v[0].p3 = n+2;
    complete[0] = false;
    ntri = 1;
    //adiciona cada ponto na malha existente
    for(i = 0; i < n; i++){
        p = vertices[i];
        nedge = 0;
        /*configura o buffer de edge.
        se o ponto p, estiver dentro da circuferencia, entao as tres edges
        do triangulo sao adicionadas ao buffer e esse triangulo e removido
        */
        for(j = 0; j < ntri; j++){
            if(complete[j]){continue;}
            A = vertices[v[j].p1];
            B = vertices[v[j].p2];
            C = vertices[v[j].p3];
            inside = inCircle(p, A, B, C, Central, r);
            if(Central.px + r < p.px){complete[j] = true;}
            if(inside){
                /* verifica se a lista de edges possui espaco,
                   se n possui realoca o vetor
                */ 
                if(nedge + 3 >= emax){
                    emax += 100;
                    p_EdgeTemp = new(nothrow)Edge[emax];
                    for(int w = 0; w < nedge; w++){
                        p_EdgeTemp[w] = edges[w];
                    }
                    delete[] edges;
                    edges = p_EdgeTemp;
                }
                edges[nedge+0].p1 = v[j].p1;
                edges[nedge+0].p2 = v[j].p2;
                edges[nedge+1].p1 = v[j].p2;
                edges[nedge+1].p2 = v[j].p3;
                edges[nedge+2].p1 = v[j].p3;
                edges[nedge+2].p2 = v[j].p1;
                nedge += 3;
                v[j] = v[ntri-1];
                complete[j] = complete[ntri-1];
                ntri--;
                j--;
            }
        }
        /*
            marcando multiplos edges
            nota: se todos os triangulos estiverem definidos em ordem anti-horaria entao
            todas as edges interiores estao apontando na direcao oposta
        */
       for(j = 0; j < nedge - 1; j++)
       {
        for(k = j + 1; k < nedge; k++){
            if((edges[j].p1 == edges[k].p2) && (edges[j].p2 == edges[k].p1)){
                edges[j].p1 = -1;
                edges[j].p2 = -1;
                edges[k].p1 = -1;
                edges[k].p2 = -1;
            }
        }
       }
        /*
            monta novos triangulos com os pontos atuais
            pula todos os edges marcados.
            todos os edges estao montados em ordem anti-horaria
        */
        for(j = 0; j < nedge; j++){
            if(edges[j].p1 < 0 || edges[j].p2 < 0){ continue; }
            v[ntri].p1 = edges[j].p1;
            v[ntri].p2 = edges[j].p2;
            v[ntri].p3 = i;
            complete[ntri] = false;
            ntri++;
        }
    }
    /*
    deleta os triangulos que possuem o numero de vertice meior que n
    */
    for(i = 0; i < ntri; i++){
        if(v[i].p1 >= n || v[i].p2 >= n || v[i].p3 >= n){
            v[i] = v[ntri-1];
            ntri--;
            i--;
        }
    }
    delete[] edges;
    delete[] complete;
    return 0;
}

#endif