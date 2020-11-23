//
// Created by Yiming Li on 2020/11/19.
//

#include "euler_operator.h"

using namespace half_edge_data_structure;
using namespace euler_operator;

// v 在外面先创建好再传入
Solid *EulerOperator::mvsf(Vertex *v) {
    Solid *s = new Solid;
    Face *f = new Face;
    Loop *out_loop = new Loop;

    // 建立连接关系
    s->sfaces = f;
    f->fsolid = s;
    f->floops_out = out_loop;
    out_loop->lface = f;

    return s;
}
// v2 在外面先创建好再传入
HalfEdge *EulerOperator::mev(Vertex *v1, Vertex *v2, Loop *loop) {
    auto left_half_edge = new HalfEdge,
         right_half_edge = new HalfEdge;
    left_half_edge->svtx = v1, left_half_edge->evtx = v2;
    right_half_edge->svtx = v2, right_half_edge->evtx = v1;
    auto edge = new Edge;
    edge->he1 = left_half_edge, edge->he2 = right_half_edge;

    left_half_edge->edg = right_half_edge->edg = edge;
    left_half_edge->wloop = right_half_edge->wloop = loop;

    left_half_edge->nxt = right_half_edge;

    // loop 内部之前无边
    if (loop->ledg == nullptr) {
        // 两条半边本身先构成一个临时的环
        loop->ledg = right_half_edge->nxt = left_half_edge;
        left_half_edge->prv = right_half_edge;
        right_half_edge->prv = left_half_edge;
    } else {
        HalfEdge *half_edge = loop->ledg;
        while (half_edge->evtx != v1) half_edge = half_edge->nxt;
        right_half_edge->prv = left_half_edge;
        right_half_edge->nxt = half_edge->nxt;
        half_edge->nxt->prv = right_half_edge;
        left_half_edge->prv = half_edge;
        half_edge->nxt = left_half_edge;
    }

    auto solid = loop->lface->fsolid;
    if (solid->sedges == nullptr)
        solid->sedges = edge;
    else {
        Edge *e = solid->sedges;
        while (e->nexte != nullptr) e = e->nexte;
        e->nexte = edge;
        edge->preve = e;
    }
    return left_half_edge;
}
Face *EulerOperator::mef(Vertex *v1, Vertex *v2, Loop *loop) {
    auto left_half_edge = new HalfEdge, right_half_edge = new HalfEdge;
    left_half_edge->svtx = v1, left_half_edge->evtx = v2;
    right_half_edge->svtx = v2, right_half_edge->evtx = v1;
    auto edge = new Edge;
    edge->he1 = left_half_edge, edge->he2 = right_half_edge;
    left_half_edge->edg = right_half_edge->edg = edge;

    auto loop_new = new Loop;
    auto face = new Face;

    left_half_edge->wloop = loop_new, right_half_edge->wloop = loop;
    loop_new->lface = face;
    loop_new->ledg = left_half_edge;

    HalfEdge *v1_prev_half_edg, *v2_prev_half_edg, *tmp_half_edge = loop->ledg;
    while (tmp_half_edge->evtx != v1) tmp_half_edge = tmp_half_edge->nxt;
    v1_prev_half_edg = tmp_half_edge, tmp_half_edge = loop->ledg;
    while (tmp_half_edge->evtx != v2) tmp_half_edge = tmp_half_edge->nxt;
    v2_prev_half_edg = tmp_half_edge;

    right_half_edge->nxt = v1_prev_half_edg->nxt;
    left_half_edge->nxt = v2_prev_half_edg->nxt;
    right_half_edge->prv = v2_prev_half_edg;
    left_half_edge->prv = v1_prev_half_edg;
    v1_prev_half_edg->nxt = left_half_edge;
    v2_prev_half_edg->nxt = right_half_edge;

    right_half_edge->nxt->prv = right_half_edge, left_half_edge->nxt->prv = left_half_edge;
    loop->ledg = right_half_edge;

    // 将 face 添加到 solid 的过程
    auto solid = loop->lface->fsolid;
    face->fsolid = solid;
    auto f = solid->sfaces;
    if (!f)
        solid->sfaces = face;
    else {
        for (; f->nextf != nullptr; f = f->nextf)
            ;
        f->nextf = face;
        face->prevf = f;
    }
    // 将 edge 添加到 solid
    auto e = solid->sedges;
    if (!e)
        solid->sedges = edge;
    else {
        for (; e->nexte != nullptr; e = e->nexte)
            ;
        e->nexte = edge;
        edge->preve = e;
    }
    // 将 loop_new 添加到 face
    loop_new->lface = face;
    if (face->floops_out == nullptr) {
        face->floops_out = loop_new;
    } else {
        if (face->floops_in == nullptr) {
            face->floops_in = loop_new;
        } else {
            auto l = face->floops_in;
            while (l->nextl != nullptr) l = l->nextl;
            l->nextl = loop_new;
            loop_new->prevl = l;
        }
    }

    return face;
}
Loop *EulerOperator::kemr(Vertex *v1, Vertex *v2, Loop *loop) {
    auto solid = loop->lface->fsolid;
    auto face = loop->lface;
    auto loop_in = new Loop;
    HalfEdge *half_edge_from_v1_to_v2 = nullptr, *half_edge_from_v2_to_v1 = nullptr, *half_edge = loop->ledg;

    while (half_edge->svtx != v1 || half_edge->evtx != v2) {
        half_edge = half_edge->nxt;
    }
    half_edge_from_v1_to_v2 = half_edge;
    while (half_edge->svtx != v2 || half_edge->evtx != v1) {
        half_edge = half_edge->nxt;
    }
    half_edge_from_v2_to_v1 = half_edge;

    // 删除 v1 - v2 的边
    half_edge_from_v1_to_v2->prv->nxt = half_edge_from_v2_to_v1->nxt;
    half_edge_from_v2_to_v1->nxt->prv = half_edge_from_v1_to_v2->prv;

    half_edge_from_v1_to_v2->nxt->prv = half_edge_from_v2_to_v1->prv;
    half_edge_from_v2_to_v1->prv->nxt = half_edge_from_v1_to_v2->nxt;

    face->floops_out->ledg = half_edge_from_v1_to_v2->prv;
    loop_in->lface = face, loop_in->ledg = half_edge_from_v1_to_v2->nxt;

    face = solid->sfaces;
    loop_in->lface = face;
    if (face->floops_out == nullptr) {
        face->floops_out = loop_in;
    } else {
        if (face->floops_in == nullptr) {
            face->floops_in = loop_in;
        } else {
            auto l = face->floops_in;
            while (l->nextl != nullptr) l = l->nextl;
            l->nextl = loop_in;
            loop_in->prevl = l;
        }
    }

    delete half_edge_from_v1_to_v2, half_edge_from_v2_to_v1;
    return loop_in;
}
Solid *EulerOperator::kfmrh(Face *f1, Face *f2) {
    auto l = f2->floops_out;
    l->lface = f1;
    if (f1->floops_out == nullptr)
        f1->floops_out = l;
    else {
        if (f1->floops_in == nullptr)
            f1->floops_in = l;
        else {
            auto *ll = f1->floops_in;
            while (ll->nextl != nullptr) ll = ll->nextl;
            ll->nextl = l;
            l->prevl = ll;
        }
    }

    Solid *solid = f2->fsolid;
    auto face = solid->sfaces;
    if (face == f2) {
        solid->sfaces = face->nextf;
        solid->sfaces->prevf = face->prevf;
        delete face;
    } else {
        Face *f = face->nextf;
        while (f != f2) f = f->nextf;
        if (f == nullptr)
            return solid;
        else {
            f->prevf->nextf = f->nextf;
            if (f->nextf != nullptr) f->nextf->prevf = f->prevf;
            delete f;
        }
    }
    return solid;
}