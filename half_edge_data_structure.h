//
// Created by Yiming Li on 2020/11/19.
//

#ifndef EULEROPERATOR_HALF_EDGE_DATA_STRUCTURE_H
#define EULEROPERATOR_HALF_EDGE_DATA_STRUCTURE_H

// 半边数据结构的定义，参考《计算机图形学教程》图 10.16

namespace half_edge_data_structure {
class Solid;
class Face;
class Loop;
class HalfEdge;
class Edge;
class Vertex;

class Solid {
   public:
    Solid *prevs, *nexts;
    Face *sfaces;
    Edge *sedges;

    Solid() : prevs(nullptr), nexts(nullptr), sfaces(nullptr), sedges(nullptr) {}
};

class Face {
   public:
    Solid *fsolid;
    Face *prevf, *nextf;
    // 外环和内环
    Loop *floops_out, *floops_in;

    Face() : fsolid(nullptr), prevf(nullptr), nextf(nullptr), floops_out(nullptr), floops_in(nullptr) {}
};

class Loop {
   public:
    Face *lface;
    Loop *prevl, *nextl;
    HalfEdge *ledg;

    Loop() : lface(nullptr), prevl(nullptr), nextl(nullptr), ledg(nullptr) {}
};

class HalfEdge {
   public:
    Loop *wloop;
    HalfEdge *prv, *nxt;
    Edge *edg;
    // 书上只说是 vtx，应该分起始点和终止点
    Vertex *svtx, *evtx;

    HalfEdge() : wloop(nullptr), prv(nullptr), nxt(nullptr), edg(nullptr), svtx(nullptr), evtx(nullptr) {}
};

class Edge {
   public:
    HalfEdge *he1, *he2;
    Edge *preve, *nexte;

    Edge() : he1(nullptr), he2(nullptr), preve(nullptr), nexte(nullptr) {}
};

class Vertex {
   public:
    Vertex *prevv, *nextv;
    // 点的几何信息
    double x, y, z;

    Vertex(double x = 0, double y = 0, double z = 0) : prevv(nullptr), nextv(nullptr), x(x), y(y), z(z) {}
    Vertex operator=(Vertex *v) {
        this->x = v->x, this->y = v->y, this->z = v->z;
        return *this;
    }
};

}  // namespace half_edge_data_structure

#endif  //EULEROPERATOR_HALF_EDGE_DATA_STRUCTURE_H
