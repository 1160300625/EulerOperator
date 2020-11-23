//
// Created by Yiming Li on 2020/11/19.
//

#include "sweep.h"

using namespace half_edge_data_structure;
using namespace euler_operator;
using namespace sweep;

Solid *Sweep::sweep(Solid *solid, const double direction[3], double distance) {
    auto base_face = solid->sfaces, next_face = base_face->nextf;
    auto loop = base_face->floops_out;

    auto loop_out = next_face->floops_out;
    auto half_edge = loop_out->ledg;

    auto first_v = half_edge->evtx;
    auto first_up = new Vertex;
    // 计算平移后的点的坐标
    first_up->x = first_v->x + distance * direction[0],
    first_up->y = first_v->y + distance * direction[1],
    first_up->z = first_v->z + distance * direction[2];

    Vertex *next_v = nullptr, *next_up = nullptr, *start_up = nullptr;

    eulerOperator->mev(first_v, first_up, loop);
    next_up = first_up;
    start_up = first_up;
    half_edge = half_edge->nxt;
    next_v = half_edge->evtx;

    while (next_v != first_v) {
        next_up = new Vertex;
        // 计算平移后的点的坐标
        next_up->x = next_v->x + distance * direction[0],
        next_up->y = next_v->y + distance * direction[1],
        next_up->z = next_v->z + distance * direction[2];

        eulerOperator->mev(next_v, next_up, loop);
        eulerOperator->mef(first_up, next_up, loop);
        first_up = next_up;
        half_edge = half_edge->nxt;
        next_v = half_edge->evtx;
    }
    next_up = start_up;
    eulerOperator->mef(first_up, next_up, loop);

    loop = base_face->floops_in;
    loop_out = next_face->floops_in;
    while (loop && loop_out) {
        half_edge = loop_out->ledg;
        first_v = half_edge->evtx;
        first_up = new Vertex;
        first_up->x = first_v->x + distance * direction[0],
        first_up->y = first_v->y + distance * direction[1],
        first_up->z = first_v->z + distance * direction[2];
        next_v = next_up = start_up = nullptr;

        eulerOperator->mev(first_v, first_up, loop);
        next_up = start_up = first_up;
        half_edge = half_edge->nxt, next_v = half_edge->evtx;

        while (next_v != first_v) {
            next_up = new Vertex;
            next_up->x = next_v->x + distance * direction[0],
            next_up->y = next_v->y + distance * direction[1],
            next_up->z = next_v->z + distance * direction[2];
            eulerOperator->mev(next_v, next_up, loop);
            eulerOperator->mef(first_up, next_up, loop);
            first_up = next_up;
            half_edge = half_edge->nxt;
            next_v = half_edge->evtx;
        }
        next_up = start_up;
        eulerOperator->mef(first_up, next_up, loop);
        loop = loop->nextl;
        loop_out = loop_out->nextl;
    }

    return solid;
}
