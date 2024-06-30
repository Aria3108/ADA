#include <iostream>
#include <vector>
#include <queue>
#include <algorithm>
#include <utility>

using namespace std;

struct Punto {
    double x, y;
    Punto() : x(0), y(0) {}
    Punto(double x_, double y_) : x(x_), y(y_) {}

    bool operator==(const Punto& other) const {
        return x == other.x && y == other.y;
    }
};

struct MBR {
    Punto min, max;
    MBR() : min(), max() {}
    MBR(Punto min_, Punto max_) : min(min_), max(max_) {}
};

struct Arista {
    int id;
    Punto ini, fin;
    Arista() : id(0), ini(), fin() {}
    Arista(int id_, Punto ini_, Punto fin_) : id(id_), ini(ini_), fin(fin_) {}
};

bool puntoDentroDeMBR(Punto p, MBR mbr) {
    return (p.x >= mbr.min.x && p.x <= mbr.max.x &&
        p.y >= mbr.min.y && p.y <= mbr.max.y);
}

bool puntoChoque(Punto p, MBR mbr){
    return (p.x == mbr.min.x || p.x == mbr.max.x ||
        p.y == mbr.min.y || p.y == mbr.max.y);
}

bool Contenida(Arista ar, MBR mbr) {
    //Caso 1: arista completamente incluída
    if(puntoDentroDeMBR(ar.ini,mbr) && puntoDentroDeMBR(ar.fin,mbr))
        return true;
    
    //Caso 2: un punto de choque y el otro fuera 
    if((puntoChoque(ar.ini,mbr) && !puntoDentroDeMBR(ar.fin,mbr) ) ||
    (puntoChoque(ar.fin,mbr) && !puntoDentroDeMBR(ar.ini,mbr)))
        return false;
    
    //Caso 3: un punto incluído pero no choca y otro fuera 
    if((puntoDentroDeMBR(ar.ini,mbr) && !puntoChoque(ar.ini, mbr) && !puntoDentroDeMBR(ar.fin,mbr)) ||
       (puntoDentroDeMBR(ar.fin,mbr) && !puntoChoque(ar.fin, mbr) && !puntoDentroDeMBR(ar.ini,mbr)))
        return true;
       
}

bool menorx(Punto a, Punto b) { return (a.x < b.x); }
bool menory(Punto a, Punto b) { return (a.y < b.y); }

struct Nodo {
    Nodo* izq, * der;
    MBR mbr;
    bool eshoja;
    Arista* inter;
    Nodo(bool eshoja_ = false, Arista *inter_ = nullptr) : izq(nullptr), der(nullptr), eshoja(eshoja_), inter(inter_) {}
};

struct kd {
    Nodo* raiz;
    vector<Arista> aristas;

    void crearKD(vector<Arista> aristas) {
        MBR mbr = getMBRfromAristas(aristas);
        raiz = construirKD(aristas, mbr ,0);
    }

    Nodo* construirKD(vector<Arista>& aristas, MBR mbr, int depth) {
        if (aristas.size() == 1) {
            Nodo* nodo = new Nodo(true, &(aristas[0]));
            return nodo;
        }
        if (aristas.empty()) {
            return nullptr;
        }
        
        Nodo* nodo = new Nodo();
        nodo->mbr = mbr;

        vector<Arista> izqAristas, derAristas;
        MBR mbri, mbrd;

        if (depth % 2 == 0) {
            mbri = {mbr.min, {(mbr.max.x + mbr.min.x) /2, mbr.max.y}};
            mbrd = { {(mbr.max.x + mbr.min.x) / 2, mbr.min.y}, mbr.max };
        } else { 
            mbri = { mbr.min, {mbr.max.x, (mbr.min.y + mbr.max.y) / 2} };
            mbrd = { {mbr.min.x, (mbr.min.y + mbr.max.y) / 2}, mbr.max };
        }

        for (auto arista : aristas) {
            if (Contenida(arista, mbri))
                izqAristas.push_back(arista);
            if (Contenida(arista, mbrd))
                derAristas.push_back(arista);            
        }

        nodo->izq = construirKD(izqAristas, mbri, depth + 1);
        nodo->der = construirKD(derAristas, mbrd, depth + 1);

        return nodo;
    }

    MBR getMBR(vector<Punto> puntos) {
        Punto min, max;
        sort(puntos.begin(), puntos.end(), menorx);
        min.x = puntos.front().x;
        max.x = puntos.back().x;
        sort(puntos.begin(), puntos.end(), menory);
        min.y = puntos.front().y;
        max.y = puntos.back().y;
        return MBR(min, max);
    }

    MBR getMBRfromAristas(vector<Arista>& aristas) {
        vector<Punto> puntos;
        for (auto& ar : aristas) {
            puntos.push_back(ar.ini);
            puntos.push_back(ar.fin);
        }
        return getMBR(puntos);
    }

    Arista aristaMasCercana() { 
        // Placeholder implementation
        return Arista();
    }

    void printKD() {
        if (!raiz) return;
        queue<Nodo*> q;
        q.push(raiz);
        while (!q.empty()) {
            int levelSize = q.size();
            while (levelSize--) {
                Nodo* nodo = q.front(); q.pop();
                cout << "MBR: [(" << nodo->mbr.min.x << ", " << nodo->mbr.min.y << "), ("
                    << nodo->mbr.max.x << ", " << nodo->mbr.max.y << ")]";
                if (nodo->eshoja) {
                    cout << " -> Arista: [(" << nodo->inter->ini.x << ", " << nodo->inter->ini.y << "), ("
                        << nodo->inter->fin.x << ", " << nodo->inter->fin.y << ")]";
                }
                cout << endl;
                if (nodo->izq) q.push(nodo->izq);
                if (nodo->der) q.push(nodo->der);
            }
            cout << "--------------------------------" << endl;
        }
    }
};

int main() {
    // Definir algunos puntos y aristas de prueba
    Punto A(1, 1), B(3, 5), C(5, 3), D(9, 6), E(4, 9);
    vector<Arista> aristas = { {1, A,B}, {2 ,B,C}, {3, C,D}, {4, D,E} };

    kd kdtree;
    kdtree.crearKD(aristas);
    kdtree.printKD();

    return 0;
}
