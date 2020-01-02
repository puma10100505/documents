#include "ode/ode.h"
#include "drawstuff/drawstuff.h"

static dWorldID world;  // hold the rigidbodies for display
static dSpaceID space;  // hold the geometries for collision detection

static dGeomID ground;
static dJointGroupID contactgroup;

static int flag = 0;
dsFunctions fn;

const dReal radius = 0.2;
const dReal mass = 1.0;

typedef struct {
    dBodyID body;
    dGeomID geom;
} MyObject;

MyObject ball;

static void nearCallback(void *data, dGeomID o1, dGeomID o2) {
    const int N = 10;
    dContact contact[N];

    int isGround = ((o1 == ground) || (o2 == ground));
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    if (isGround){
        if (n >= 1) flag = 1; else flag = 0;
        for (int i = 0; i < n; i++) {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.bounce = 0.3f;
            contact[i].surface.bounce_vel = 0.3f;
            dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
            dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));
        }
    }
}

static void step(int pause) {
    const dReal *pos, *R;
    flag = 0; 
    dsSetSphereQuality(3);
    dSpaceCollide(space, 0, &nearCallback);

    dWorldStep(world, 0.01);
    dJointGroupEmpty(contactgroup);
       

    if (flag == 0) {
        dsSetColor(1.0, 0.0f, 0.0f);
    } else {
        dsSetColor(0.0f, 0.0f, 1.0f);
    }

    pos = dBodyGetPosition(ball.body);
    R = dBodyGetRotation(ball.body);
    dsDrawSphere(pos, R, radius);
}

static void start() {
    static float xyz[3] = {0.0f, -10.0f, 1.0f};
    static float hpr[3] = {90.0f, 10.0f, 0.0f};
    dsSetViewpoint(xyz, hpr);
}

static void prepDrawstuff() {
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &step;
    fn.command = 0;
    fn.stop = 0;
    fn.path_to_textures = "../resources/ode/textures";
}

int main(int argc, char** argv) {
    dReal x0 = 0.0f, y0 = 0.0f, z0 = 5.0f;
    dMass m1;

    prepDrawstuff();

    dInitODE();
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);
    dWorldSetGravity(world, 0, 0, -9.8f);
    ground = dCreatePlane(space, 0, 0, 1, 0);
    ball.body = dBodyCreate(world);
    dMassSetZero(&m1);
    dMassSetSphereTotal(&m1, mass, radius);
    dBodySetMass(ball.body, &m1);
    dBodySetPosition(ball.body, x0, y0, z0);
    ball.geom = dCreateSphere(space, radius);
    dGeomSetBody(ball.geom, ball.body); // bind the body with geom
    dsSimulationLoop(argc, argv, 640, 480, &fn);
    dWorldDestroy(world);
    dCloseODE();
    return 0;
}
