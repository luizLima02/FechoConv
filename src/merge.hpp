#pragma once
#include <vector>
#include <functional>

// Junta 2 metades ordenadas
template<typename T>
void merge(std::vector<T>& arr, int l, int m, int r, const std::function<bool(const T&, const T&)>& compare) {
    int n1 = m - l + 1;
    int n2 = r - m;

    // Arrays temporários
    std::vector<T> L(n1), R(n2);

    // Copia para os arrays temporários
    for (int i = 0; i < n1; i++)
        L[i] = arr[l + i];
    for (int j = 0; j < n2; j++)
        R[j] = arr[m + 1 + j];

    // Junta os arrays temporários em arr[l..r]
    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2) {
        if (compare(L[i], R[j])) {
            arr[k] = L[i];
            i++;
        } else {
            arr[k] = R[j];
            j++;
        }
        k++;
    }

    // Copia os elementos restantes de L[]
    while (i < n1) {
        arr[k] = L[i];
        i++;
        k++;
    }

    // Copia os elementos restantes de R[]
    while (j < n2) {
        arr[k] = R[j];
        j++;
        k++;
    }
}

// Merge sort recursivo
template<typename T>
void mergeSortAux(std::vector<T>& arr, int l, int r, const std::function<bool(const T&, const T&)>& compare) {
    if (l < r) {
        // Encontra o meio
        int m = l + (r - l) / 2;

        // Ordena a primeira e a segunda metade
        mergeSortAux(arr, l, m, compare);
        mergeSortAux(arr, m + 1, r, compare);

        // Junta as duas metades
        merge(arr, l, m, r, compare);
    }
}

// Função de ordenação principal
template<typename T>
void mergeSort(std::vector<T>& arr, const std::function<bool(const T&, const T&)>& compare) {
    mergeSortAux(arr, 0, arr.size() - 1, compare);
}