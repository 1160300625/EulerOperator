//
// Created by Yiming Li on 2020/11/19.
//

#include "renderer.h"

#include <GL/glut.h>

#include <iostream>
#include <vector>

using namespace render;
using namespace half_edge_data_structure;
using namespace std;

GLfloat mouse_x = 0.0, mouse_y = 0.0;
GLfloat camera_angle_x = 0.0, camera_angle_y = 0.0, camera_z_pos = -30.0;
bool mouseLeftDown = false, mouseRightDown = false;
extern Solid *solid;
vector<vector<GLdouble>> vGL_arr;

void GLU_TESS_VERTEX_callback(GLvoid *vertex) {
    GLdouble *pt = (GLdouble *)vertex;
    glVertex3d(pt[0], pt[1], pt[2]);
}
GLvoid GLU_TESS_BEGIN_callback(GLenum type) {
    glBegin(type);
}
GLvoid GLU_TESS_END_callback() {
    glEnd();
}
GLvoid GLU_TESS_ERROR_callback(GLenum errorCode) {
    perror("错误发生啦：");
    exit(0);
}

void displayFunc() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glMatrixMode(GL_MODELVIEW);
    glLoadIdentity();
    glLightModeli(GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE);
    GLfloat ambient_color[] = {0.2, 0.2, 0.2, 0.2};
    glLightModelfv(GL_LIGHT_MODEL_AMBIENT, ambient_color);
    GLfloat light0_diffuse[] = {1, 1, 1, 1};
    GLfloat light0_position[] = {2, 0, 2, 1};
    glLightfv(GL_LIGHT0, GL_DIFFUSE, light0_diffuse);
    glLightfv(GL_LIGHT0, GL_POSITION, light0_position);
    glTranslatef(0, 0, camera_z_pos);
    glRotatef(camera_angle_x, 1, 0, 0);
    glRotatef(camera_angle_y, 0, 1, 0);

    auto face = solid->sfaces;
    GLUtesselator *tesselator = gluNewTess();
    gluTessCallback(tesselator, GLU_TESS_VERTEX, (GLvoid(*)())GLU_TESS_VERTEX_callback);
    gluTessCallback(tesselator, GLU_TESS_BEGIN, (GLvoid(*)())GLU_TESS_BEGIN_callback);
    gluTessCallback(tesselator, GLU_TESS_END, (GLvoid(*)())GLU_TESS_END_callback);
    gluTessCallback(tesselator, GLU_TESS_ERROR, (GLvoid(*)())GLU_TESS_ERROR_callback);

    while (face) {
        gluTessBeginPolygon(tesselator, nullptr);
        //处理out loop, out loop 只有一个
        auto loop = face->floops_out;
        auto *half_edge = loop->ledg;
        auto a = half_edge->evtx, b = half_edge->nxt->evtx, c = half_edge->nxt->nxt->evtx;
        glNormal3f(
            (b->y - a->y) * (c->z - b->z) - (b->z - a->z) * (c->y - b->y),
            (b->z - a->z) * (c->x - b->x) - (b->x - a->x) * (c->z - b->z),
            (b->x - a->x) * (c->y - b->y) - (b->y - a->y) * (c->x - b->x));
        gluTessBeginContour(tesselator);
        HalfEdge *start_half_edge = loop->ledg,
                 *p = start_half_edge;
        half_edge = nullptr;
        while (start_half_edge != half_edge) {
            auto v = p->evtx;
            vGL_arr.push_back({v->x, v->y, v->z});
            // cout << v->x << ", " << v->y << ", " << v->z << endl;
            gluTessVertex(tesselator, &vGL_arr[vGL_arr.size() - 1][0], &vGL_arr[vGL_arr.size() - 1][0]);
            p = p->nxt;
            half_edge = p;
        }
        gluTessEndContour(tesselator);
        //处理in loop，in loop 可能有多个
        loop = face->floops_in;
        while (loop != nullptr) {
            gluTessBeginContour(tesselator);
            start_half_edge = loop->ledg, half_edge = nullptr, p = start_half_edge;
            while (start_half_edge != half_edge) {
                Vertex *v = p->evtx;
                vGL_arr.push_back({v->x, v->y, v->z});
                // cout << v->x << ", " << v->y << ", " << v->z << endl;
                gluTessVertex(tesselator, &vGL_arr[vGL_arr.size() - 1][0], &vGL_arr[vGL_arr.size() - 1][0]);
                p = p->nxt;
                half_edge = p;
            }
            loop = loop->nextl;
            gluTessEndContour(tesselator);
        }
        face = face->nextf;
        gluTessEndPolygon(tesselator);
    }

    glutSwapBuffers();
}

void handleResize(int w, int h) {
    glViewport(0, 0, w, h);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0, (double)w / (double)h, 1.0, 200.0);
}
void handleKeypress(unsigned char key, int x, int y) {
    switch (key) {
        case 27:
            exit(0);
    }
}
void handleMousePress(int button, int state, int x, int y) {
    mouse_x = x;
    mouse_y = y;

    if (button == GLUT_LEFT_BUTTON) {
        if (state == GLUT_DOWN)
            mouseLeftDown = true;
        else if (state == GLUT_UP)
            mouseLeftDown = false;
    } else if (button == GLUT_RIGHT_BUTTON) {
        if (state == GLUT_DOWN)
            mouseRightDown = true;
        else if (state == GLUT_UP)
            mouseRightDown = false;
    }
}
void handleMouseActiveMotion(int x, int y) {
    if (mouseLeftDown) {
        camera_angle_y += (x - mouse_x) * 0.2;
        camera_angle_x += (y - mouse_y) * 0.2;
        mouse_x = x;
        mouse_y = y;
    }

    if (mouseRightDown) {
        camera_z_pos += (y - mouse_y) * 0.2;
        mouse_y = y;
    }

    glutPostRedisplay();
}

void Renderer::renderModel() {
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(glutGet(GLUT_SCREEN_WIDTH), glutGet(GLUT_SCREEN_HEIGHT));
    glutCreateWindow("Euler Operation (Press Esc to exit)");
    // 主题颜色
    glColor3f(0.957, 0.643, 0.376);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_COLOR_MATERIAL);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHT1);
    glEnable(GL_NORMALIZE);
    glShadeModel(GL_SMOOTH);

    glutDisplayFunc(displayFunc);

    glutKeyboardFunc(handleKeypress);
    glutReshapeFunc(handleResize);
    glutMouseFunc(handleMousePress);
    glutMotionFunc(handleMouseActiveMotion);

    glutMainLoop();
}