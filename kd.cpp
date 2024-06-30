#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>  
using namespace std;


struct Punto {
    double x, y;
};

struct Arista {
    int ID;
    Punto inicio, fin;
};

struct NodoKD {
    Arista arista;
    NodoKD* izquierda;
    NodoKD* derecha;

    NodoKD(Arista _arista) : arista(_arista), izquierda(nullptr), derecha(nullptr) {}
};

class KdTree {
private:
    NodoKD* raiz;

    NodoKD* insertarRecursivo(NodoKD* nodo, Arista arista, unsigned profundidad) {
        if (nodo == nullptr) {
            return new NodoKD(arista);
        }

        unsigned cd = profundidad % 2;
        if ((cd == 0 && arista.inicio.x < nodo->arista.inicio.x) || (cd == 1 && arista.inicio.y < nodo->arista.inicio.y)) {
            nodo->izquierda = insertarRecursivo(nodo->izquierda, arista, profundidad + 1);
        } else {
            nodo->derecha = insertarRecursivo(nodo->derecha, arista, profundidad + 1);
        }

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
        cout << "ID " << nodo->arista.ID << ": ("
                  << nodo->arista.inicio.x << ", " << nodo->arista.inicio.y << ") -> ("
                  << nodo->arista.fin.x << ", " << nodo->arista.fin.y << ") [Nivel " << profundidad << "]" << endl;

        imprimirRecursivo(nodo->izquierda, espacio, profundidad + 1);
    }

    NodoKD* aristaMasCercanaRecursivo(NodoKD* nodo, Punto punto, NodoKD* mejor, unsigned profundidad) {
        if (nodo == nullptr) {
            return mejor;
        }

        unsigned cd = profundidad % 2;
        NodoKD* siguiente = (cd == 0 && punto.x < nodo->arista.inicio.x) || (cd == 1 && punto.y < nodo->arista.inicio.y) ? nodo->izquierda : nodo->derecha;
        NodoKD* otro = siguiente == nodo->izquierda ? nodo->derecha : nodo->izquierda;

        mejor = aristaMasCercanaRecursivo(siguiente, punto, mejor, profundidad + 1);

        return mejor;
    }

public:
    KdTree() : raiz(nullptr) {}

    void insertar(Arista arista) {
        raiz = insertarRecursivo(raiz, arista, 0);
    }

    void imprimir() {
        imprimirRecursivo(raiz, 0, 0);
    }

    Arista aristaMasCercana(Punto punto) {
        NodoKD* nodo = aristaMasCercanaRecursivo(raiz, punto, raiz, 0);
        return nodo != nullptr ? nodo->arista : Arista{-1, {0, 0}, {0, 0}};
    }
};

int main() {
    KdTree arbol;
    arbol.insertar({1, {5, 1}, {2, 3}});arbol.insertar({2, {2, 3}, {7, 2}});
    arbol.insertar({3, {2, 3}, {1, 6}});arbol.insertar({4, {5, 1}, {7, 2}});
    arbol.insertar({5, {4, 7}, {1, 6}});arbol.insertar({6, {9, 4}, {8, 9}});
    arbol.insertar({7, {4, 7}, {4, 9}});arbol.insertar({8, {9, 4}, {4, 7}});
    arbol.insertar({9, {4, 7}, {7, 2}});arbol.insertar({10, {9, 4}, {7, 2}});
    arbol.insertar({11,{4, 7}, {1, 8}});arbol.insertar({12, {1, 8}, {4, 9}});
    arbol.insertar({13, {7, 7}, {4, 9}});arbol.insertar({14, {7, 7}, {8, 9}});
    cout << "Arbol KD:\n";
    arbol.imprimir();

    Punto consulta = {6, 3};
    Arista masCercana = arbol.aristaMasCercana(consulta);
    cout << "\nArista más cercana al punto (" << consulta.x << ", " << consulta.y << "):\n";
    cout << "ID " << masCercana.ID << ": ("
              << masCercana.inicio.x << ", " << masCercana.inicio.y << ") -> ("
              << masCercana.fin.x << ", " << masCercana.fin.y << ")\n";

    return 0;
}
