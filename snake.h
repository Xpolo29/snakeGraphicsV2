#include <time.h>
#include <iostream>
#include "HMAP.h"

//var declaration


int adjMap[N][4];
int Map[N];
int body[N];
int* HMap3 = getHMap(false);



struct vect2 {
    int x, y;
};
vect2 idToCoord(int p) {
    vect2 v;
    v.x = 10 * (p % MAP_LENGTH);
    v.y = 10 * (p / MAP_HEIGHT);
    return v;
}


//return new point
int getNP(int p, int d) {
    int np = -1;
    if (d == 0) { np = p + 1; }
    else if (d == 1) { np = p + MAP_LENGTH; }
    else if (d == 2) { np = p - 1; }
    else if (d == 3) { np = p - MAP_LENGTH; }
    return np;
}

bool inBorder(int np, int p = -1, int d = -1) {
    if ((np < 0) || (np >= N)) { return false; }
    if (d == 0) { if (p % MAP_LENGTH == MAP_LENGTH - 1) { return false; } }
    if (d == 1) { if (p >= N - MAP_LENGTH) { return false; } }
    if (d == 2) { if (p % MAP_LENGTH == 0) { return false; } }
    if (d == 3) { if (p < MAP_LENGTH) { return false; } }
    return true;
}

int getDir(int p, int np) {
    int delta = np - p;
    if (delta == 1) { return 0; }
    if (delta == -1) { return 2; }
    if (delta == MAP_LENGTH) { return 1; }
    if (delta == -MAP_LENGTH) { return 3; }
    return -1;
}

bool linked(int A, int B) {
    for (int i = 0; i < 4; i++) {
        if (adjMap[A][i] == B) { return true; }
    }
    return false;
}

bool contains(int L[N], int e, int delta = 0) {
    int x = sizeof(L) / sizeof(*L);
    return (std::find(L += delta, L + x, e) != L + x);
    //return false;
}


int getHIndex(int p) {
    int x = sizeof(HMap) / sizeof(*HMap);
    return std::distance(HMap, std::find(HMap, HMap + x, p));
}

int dist(int A, int B) {
    int v1[2] = { A % 100,A / 100 };
    int v2[2] = { B % 100,B / 100 };
    int d = (v1[0] - v2[0]) ^ 2 + (v1[1] - v2[1]) ^ 2;
    return d;
}
