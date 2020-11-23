#include <GL/glut.h>

#include <map>
#include <vector>

#include "cstdio"
#include "euler_operator.h"
#include "iostream"
#include "renderer.h"
#include "sweep.h"

using namespace std;
using namespace half_edge_data_structure;
using namespace euler_operator;
using namespace sweep;

struct Point {
    double x, y, z;
    Point(double x, double y, double z) : x(x), y(y), z(z) {}
};

const double direction[3] = {0, 0, 1};
int v, r;
map<int, vector<Point>> ring2Points;
EulerOperator *eulerOperator = new EulerOperator;
Sweep sweeper(eulerOperator);
Solid *solid = nullptr;

// #define DEBUG

// 从输入中读取模型
void readModel() {
/**
     格式定义
     第一行：v(vertex 个数), r(内环个数)
     接下来 v 行，每一行的格式都是：r_index, x, y, z，其中 ：
                r_index 表示点所属的内环编号（0 - r - 1），如果为 -1 则表示不在内环里面
                (x, y, z) 表示点的坐标，类型为 double
     eg: 构造一个回字形中空立方体
     8 1

     -1 0 0 0
     -1 3 0 0
     -1 0 3 0
     -1 3 3 0

     0 1 1 0
     0 2 1 0
     0 1 2 0
     0 2 2 0
     */
#ifdef DEBUG
    freopen("../data-2-holes.txt", "r", stdin);
#else
    cout << "请输入数据（参考 README.md）：" << endl;
#endif
    cin >> v >> r;
    ring2Points.clear();
    for (int i = 0; i < v; i++) {
        int ri, x, y, z;
        cin >> ri >> x >> y >> z;
        ring2Points[ri].push_back(Point(x, y, z));
    }
}

void constructModelWithSweep() {
    Vertex *v1 = new Vertex;
    v1->x = ring2Points[-1][0].x, v1->y = ring2Points[-1][0].y, v1->z = ring2Points[-1][0].z;
    Vertex *startv = v1, *v2 = nullptr;

    solid = eulerOperator->mvsf(v1);
    //    cout << ring2Points[-1].size() << endl;
    for (int i = 1; i < ring2Points[-1].size(); i++) {
        v2 = new Vertex;
        v2->x = ring2Points[-1][i].x, v2->y = ring2Points[-1][i].y, v2->z = ring2Points[-1][i].z;
        //        cout << v1->x << ", " << v1->y << ", " << v1->z << endl;
        //        cout << v2->x << ", " << v2->y << ", " << v2->z << endl;

        eulerOperator->mev(v1, v2, solid->sfaces->floops_out);

        v1 = v2;
    }
    eulerOperator->mef(v2, startv, solid->sfaces->floops_out);

    // for 所有内环
    for (int i = 0; i < r; i++) {
        Vertex *in_loop_start_v = new Vertex;
        in_loop_start_v->x = ring2Points[i][0].x, in_loop_start_v->y = ring2Points[i][0].y, in_loop_start_v->z = ring2Points[i][0].z;
        v1 = in_loop_start_v;

        //        cout << i << ", " << ring2Points[i].size() << endl;
        eulerOperator->mev(startv, in_loop_start_v, solid->sfaces->nextf->floops_out);
        for (int j = 1; j < ring2Points[i].size(); j++) {
            v2 = new Vertex;
            v2->x = ring2Points[i][j].x, v2->y = ring2Points[i][j].y, v2->z = ring2Points[i][j].z;

            //            cout << v1->x << ", " << v1->y << ", " << v1->z << endl;
            //            cout << v2->x << ", " << v2->y << ", " << v2->z << endl;

            eulerOperator->mev(v1, v2, solid->sfaces->nextf->floops_out);

            v1 = v2;
        }
        cout << "mef..." << endl;
        auto face = eulerOperator->mef(v2, in_loop_start_v, solid->sfaces->nextf->floops_out);
        cout << "kemr..." << endl;
        eulerOperator->kemr(startv, in_loop_start_v, solid->sfaces->nextf->floops_out);
        cout << "kfmrh..." << endl;
        eulerOperator->kfmrh(solid->sfaces->nextf, face);
    }
    cout << "sweeping..." << endl;
    sweeper.sweep(solid, direction, 3);
}

int main(int argc, char **argv) {
    readModel();
    constructModelWithSweep();
    glutInit(&argc, argv);

    render::Renderer renderer;
    renderer.renderModel();

    delete eulerOperator;
    cout << "Hello world!" << endl;

    return 0;
}
