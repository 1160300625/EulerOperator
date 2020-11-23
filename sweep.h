//
// Created by Yiming Li on 2020/11/19.
//

#ifndef EULEROPERATOR_SWEEP_H
#define EULEROPERATOR_SWEEP_H

#include "euler_operator.h"
#include "half_edge_data_structure.h"

namespace sweep {
using namespace half_edge_data_structure;
using namespace euler_operator;
class Sweep {
   public:
    EulerOperator *eulerOperator;

    Sweep(EulerOperator *eulerOperator)
        : eulerOperator(eulerOperator) {}

    // 平移扫成，方向为 `direction`，距离为 `distance`
    Solid *sweep(Solid *solid, const double direction[3], double distance);
};
}  // namespace sweep

#endif  //EULEROPERATOR_SWEEP_H
