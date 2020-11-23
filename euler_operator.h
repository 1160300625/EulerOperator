//
// Created by Yiming Li on 2020/11/19.
//

#ifndef EULEROPERATOR_EULER_OPERATOR_H
#define EULEROPERATOR_EULER_OPERATOR_H

#include "half_edge_data_structure.h"

namespace euler_operator {
class EulerOperator;

using namespace half_edge_data_structure;

class EulerOperator {
   public:
    // 五个欧拉操作
    // 输入一初始点 v，作为构造物体的开始
    Solid *mvsf(Vertex *v);
    // 输入一点 v2，并构造一条连结 v1, v2 的新边 e
    HalfEdge *mev(Vertex *v1, Vertex *v2, Loop *loop);
    // 构造一条连结 v1, v2 的新边 e，同时构造一张新面 f2 与已有面 f1 通过 e 邻接
    Face *mef(Vertex *v1, Vertex *v2, Loop *loop);
    // 删除 v1,v2 之间的桥边 e ，同时构造一个新的内环
    Loop *kemr(Vertex *v1, Vertex *v2, Loop *loop);
    // 将表面 f2 中的内环面 f1 删除，同时构造 f2 的一个新内环，生成物体的一个通孔
    Solid *kfmrh(Face *f1, Face *f2);
};
}  // namespace euler_operator

#endif  //EULEROPERATOR_EULER_OPERATOR_H
