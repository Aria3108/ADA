#include <iostream>
#include <vector>
#include <algorithm>
#include <limits>

using namespace std;

struct Punto {
    double x, y;
};

struct Arista {
    int id;
    Punto inicio, fin;
};

struct NodoKD {
    Arista arista;
    NodoKD* izquierda;
    NodoKD* derecha;

    NodoKD(Arista _arista) : arista(_arista), izquierda(nullptr), derecha(nullptr) {}
};

struct MBR {
    Punto min;
    Punto max;
};

class KdTree {
private:
    NodoKD* raiz;

    MBR calcularMBR(const vector<Arista>& aristas) {
        MBR mbr;
        mbr.min.x = mbr.min.y = numeric_limits<double>::max();
        mbr.max.x = mbr.max.y = numeric_limits<double>::lowest();

        for (const auto& arista : aristas) {
            mbr.min.x = min({mbr.min.x, arista.inicio.x, arista.fin.x});
            mbr.min.y = min({mbr.min.y, arista.inicio.y, arista.fin.y});
            mbr.max.x = max({mbr.max.x, arista.inicio.x, arista.fin.x});
            mbr.max.y = max({mbr.max.y, arista.inicio.y, arista.fin.y});
        }

        return mbr;
    }

    NodoKD* construirKDTree(vector<Arista>& aristas, unsigned profundidad) {
        if (aristas.empty()) {
            return nullptr;
        }

        if (aristas.size() == 1) {
            return new NodoKD(aristas[0]);
        }

        unsigned cd = profundidad % 2;
        sort(aristas.begin(), aristas.end(), [cd](const Arista& a, const Arista& b) {
            return cd == 0 ? (a.inicio.x + a.fin.x) / 2 < (b.inicio.x + b.fin.x) / 2 : (a.inicio.y + a.fin.y) / 2 < (b.inicio.y + b.fin.y) / 2;
        });

        size_t mediana = aristas.size() / 2;
        NodoKD* nodo = new NodoKD(aristas[mediana]);

        vector<Arista> izquierda(aristas.begin(), aristas.begin() + mediana);
        vector<Arista> derecha(aristas.begin() + mediana + 1, aristas.end());

        nodo->izquierda = construirKDTree(izquierda, profundidad + 1);
        nodo->derecha = construirKDTree(derecha, profundidad + 1);

        return nodo;
    }

    void imprimirRecursivo(NodoKD* nodo, int espacio, unsigned profundidad) {
        if (nodo == nullptr) {
            return;
        }

        espacio += 10;

        imprimirRecursivo(nodo->derecha, espacio, profundidad + 1);

        cout << endl;
        for (int i = 10; i < espacio; i++) {
            cout << " ";
        }
        cout << "ID " << nodo->arista.id << ": ("
                  << nodo->arista.inicio.x << ", " << nodo->arista.inicio.y << ") -> ("
                  << nodo->arista.fin.x << ", " << nodo->arista.fin.y << ") [Nivel " << profundidad << "]" << endl;

        imprimirRecursivo(nodo->izquierda, espacio, profundidad + 1);
    }

    NodoKD* aristaMasCercanaRecursivo(NodoKD* nodo, Punto punto, unsigned profundidad) {
        if (nodo == nullptr) {
            return nullptr;
        }

        unsigned cd = profundidad % 2;
        NodoKD* siguiente = (cd == 0 && punto.x < (nodo->arista.inicio.x + nodo->arista.fin.x) / 2) || (cd == 1 && punto.y < (nodo->arista.inicio.y + nodo->arista.fin.y) / 2) ? nodo->izquierda : nodo->derecha;

        if (siguiente == nullptr) {
            return nodo;
        }

        NodoKD* mejor = aristaMasCercanaRecursivo(siguiente, punto, profundidad + 1);

        return mejor == nullptr ? nodo : mejor;
    }

public:
    KdTree() : raiz(nullptr) {}

    void construir(const vector<Arista>& aristas) {
        vector<Arista> aristasCopy = aristas;
        raiz = construirKDTree(aristasCopy, 0);
    }

    void imprimir() {
        imprimirRecursivo(raiz, 0, 0);
    }

    Arista aristaMasCercana(Punto punto) {
        NodoKD* nodo = aristaMasCercanaRecursivo(raiz, punto, 0);
        return nodo != nullptr ? nodo->arista : Arista{-1, {0, 0}, {0, 0}};
    }
};

int main() {
    vector<Arista> aristas = {
        {1, {5, 1}, {2, 3}}, {2, {2, 3}, {7, 2}}, {3, {2, 3}, {1, 6}},
        {4, {5, 1}, {7, 2}}, {5, {4, 7}, {1, 6}}, {6, {9, 4}, {8, 9}},
        {7, {4, 7}, {4, 9}}, {8, {9, 4}, {4, 7}}, {9, {4, 7}, {7, 2}},
        {10, {9, 4}, {7, 2}}, {11,{4, 7}, {1, 8}}, {12, {1, 8}, {4, 9}},
        {13, {7, 7}, {4, 9}}, {14, {7, 7}, {8, 9}}
    };

    KdTree arbol;
    arbol.construir(aristas);

    cout << "Arbol KD:\n";
    arbol.imprimir();

    Punto consulta = {3,5};
    Arista masCercana = arbol.aristaMasCercana(consulta);
    cout << "\nArista mas cercana al punto (" << consulta.x << ", " << consulta.y << "):\n";
    cout << "ID " << masCercana.id << ": ("
              << masCercana.inicio.x << ", " << masCercana.inicio.y << ") -> ("
              << masCercana.fin.x << ", " << masCercana.fin.y << ")\n";

    return 0;
}
