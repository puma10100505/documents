#include "ode/ode.h"
#include "drawstuff/drawstuff.h"

#include <cstdio>

using namespace std;

#ifdef dDOUBLE
#define dsDrawSphere dsDrawSphereD
#endif

static dWorldID world;
dBodyID ball;
const dReal radius = 0.2f;
const dReal mass = 1.0f;

static void step(int pause) {
    const dReal *pos, *R;
    dWorldStep(world, 0.05f);
    dsSetColor(1.0f, 0.0f, 0.0f);
    pos = dBodyGetPosition(ball);
    R = dBodyGetRotation(ball);
    dsDrawSphere(pos, R, radius);
    printf("after step\n");
}

static void start() {
    static float xyz[3] = {0.0f, -3.0f, 1.0f};
    static float hpr[3] = {90.0f, 0.0f, 0.0f};
    dsSetViewpoint(xyz, hpr);
    printf("after start\n");
}

int main(int argc, char** argv) {
    dReal x0 = 0.0f, y0 = 0.0f, z0 = 1.0f;
    dMass m1;

    printf("begin\n");

    dsFunctions fn;
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &step;
    fn.command = NULL;
    fn.stop = NULL;
    fn.path_to_textures = "../resources/ode/textures";

    dInitODE();
    world = dWorldCreate();
    dWorldSetGravity(world, 0, 0, -0.001);

    ball = dBodyCreate(world);
    dMassSetZero(&m1);
    dMassSetSphereTotal(&m1, mass, radius);
    dBodySetMass(ball, &m1);
    dBodySetPosition(ball, x0, y0, z0);

    dsSimulationLoop(argc, argv, 352, 288, &fn);

    dWorldDestroy(world);
    dCloseODE();
    printf("after close ode\n");
    return 0;
}