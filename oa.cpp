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

bool Contenida(Arista ar, MBR mbr) {
    if (puntoDentroDeMBR(ar.ini, mbr) && puntoDentroDeMBR(ar.fin, mbr)) {
        return true;
    }

    Punto p1 = mbr.min, p2 = mbr.max;
    Punto pa1 = ar.ini, pa2 = ar.fin;
    double A = pa2.y - pa1.y;
    double B = -(pa2.x - pa1.x);
    double C = pa2.x * pa1.y - pa1.x * pa2.y;
    std::vector<Punto> intersections;

    if (B != 0) {
        double y = (-C - A * p1.x) / B;
        if (p1.y <= y && y <= p2.y) {
            intersections.push_back({ p1.x, y });
        }
    }

    if (B != 0) {
        double y = (-C - A * p2.x) / B;
        if (p1.y <= y && y <= p2.y) {
            intersections.push_back({ p2.x, y });
        }
    }

    if (A != 0) {
        double x = (-C - B * p1.y) / A;
        if (p1.x <= x && x <= p2.x) {
            intersections.push_back({ x, p1.y });
        }
    }

    if (A != 0) {
        double x = (-C - B * p2.y) / A;
        if (p1.x <= x && x <= p2.x) {
            intersections.push_back({ x, p2.y });
        }
    }

    if (intersections.size() == 2) {
        Punto i1 = intersections[0];
        Punto i2 = intersections[1];
        if (i1 == i2) {
            return false;
        } else if (puntoDentroDeMBR(i1, mbr) && puntoDentroDeMBR(i2, mbr)) {
            if ((i1.x >= std::min(pa1.x, pa2.x) && i1.x <= std::max(pa1.x, pa2.x) &&
                i1.y >= std::min(pa1.y, pa2.y) && i1.y <= std::max(pa1.y, pa2.y)) &&
                (i2.x >= std::min(pa1.x, pa2.x) && i2.x <= std::max(pa1.x, pa2.x) &&
                    i2.y >= std::min(pa1.y, pa2.y) && i2.y <= std::max(pa1.y, pa2.y))) {
                return true;
            }
        }
    }

    return false;
}

bool menorx(Punto a, Punto b) { return (a.x < b.x); }
bool menory(Punto a, Punto b) { return (a.y < b.y); }

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

struct Nodo {
    Nodo* izq, * der;
    MBR mbr;
    bool eshoja;
    Arista* inter;
    Nodo(bool eshoja_ = false, Arista* inter_ = nullptr) : izq(nullptr), der(nullptr), eshoja(eshoja_), inter(inter_) {}
};

struct kd {
    Nodo* raiz;

    void crearKD(vector<Arista>& aristas) {
        MBR mbr = getMBRfromAristas(aristas);
        raiz = construirKD(aristas, mbr, 0);
    }

    Nodo* construirKD(vector<Arista>& aristas, MBR mbr, int depth) {
        if (aristas.empty()) {
            return nullptr;
        }

        if (aristas.size() == 1) {
            Nodo* nodo = new Nodo(true, new Arista(aristas[0]));
            nodo->mbr = mbr;
            return nodo;
        }

        Nodo* nodo = new Nodo();
        nodo->mbr = mbr;

        vector<Arista> izqAristas, derAristas;
        MBR mbri, mbrd;

        if (depth % 2 == 0) {
            mbri = {mbr.min, {(mbr.max.x + mbr.min.x) / 2, mbr.max.y}};
            mbrd = {{(mbr.max.x + mbr.min.x) / 2, mbr.min.y}, mbr.max};
        } else { 
            mbri = {{mbr.min.x, (mbr.min.y + mbr.max.y) / 2}, mbr.max};  
            mbrd = {mbr.min, {mbr.max.x, (mbr.min.y + mbr.max.y) / 2}};
        }

        for (auto& arista : aristas) {
            if (Contenida(arista, mbri)) {
                izqAristas.push_back(arista);
            }
            if (Contenida(arista, mbrd)) {
                derAristas.push_back(arista);
            }
        }

        nodo->izq = construirKD(izqAristas, mbri, depth + 1);
        nodo->der = construirKD(derAristas, mbrd, depth + 1);

        return nodo;
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
                    cout << " -> Arista: [ " << nodo->inter->id << " (" << nodo->inter->ini.x << ", " << nodo->inter->ini.y << "), ("
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
    Punto A(1, 1), B(3, 5), C(5, 3), D(9, 6), E(4, 9);
    vector<Arista> aristas = { {1, A,B}, {2, B,C}, {3, C,D}, {4, D,E} };

    kd kdtree;
    kdtree.crearKD(aristas);
    kdtree.printKD();

    return 0;
}
