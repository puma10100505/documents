#include "ode/ode.h"
#include "drawstuff/drawstuff.h"

#ifdef dDOUBLE
#define dsDrawSphere dsDrawSphereD
#define dsDrawCapsule dsDrawCapsuleD
#endif

static dWorldID world;
static dSpaceID space;
static dGeomID ground;
static dJointGroupID contactgroup;

dsFunctions fn;

typedef struct {
    dBodyID body;
    dGeomID geom;
    dReal radius; 
    dReal length;
    dReal mass;
} MyLink;

MyLink ball, pole;
dJointID joint;

static void nearCallback(void* data, dGeomID o1, dGeomID o2) {
    const int N = 10;
    dContact contact[N];

    int isGround = ((ground == o1) || (ground == o2));

    // Collision Detection
    int n = dCollide(o1, o2, N, &contact[0].geom, sizeof(dContact));

    // if (n < 2) {
    //     printf("there is no collision ocurred\n");
    //     return;
    // }

    if (isGround) {
        for (int i = 0; i < n; i++) {
            contact[i].surface.mode = dContactBounce;
            contact[i].surface.mu = dInfinity;
            contact[i].surface.bounce = 1.0f;
            contact[i].surface.bounce_vel = 0.1f;
            dJointID c = dJointCreateContact(world, contactgroup, &contact[i]);
            dJointAttach(c, dGeomGetBody(contact[i].geom.g1), dGeomGetBody(contact[i].geom.g2));            
        }
    }
}

static void step(int pause) {
    const dReal *pos1, *R1, *pos2, *R2;
    dSpaceCollide(space, 0, &nearCallback);
    dWorldStep(world, 0.01f);
    dJointGroupEmpty(contactgroup);
    dsSetColor(1.0f, 0.0f, 0.0f);

    pos1 = dBodyGetPosition(ball.body);
    R1 = dBodyGetRotation(ball.body);
    dsDrawSphere(pos1, R1, ball.radius);

    pos2 = dBodyGetPosition(pole.body);
    R2 = dBodyGetRotation(pole.body);

    dsDrawCapsule(pos2, R2, pole.length, pole.radius);
}

static void start() {
    static float xyz[3] = {0.0, -3.0, 1.0};
    static float hpr[3] = {90.0, 0.0, 0.0};
    dsSetViewpoint(xyz, hpr);
}

static void createBallAndPole() {
    dMass m1, m2;
    dReal x0 = 0.0, y0 = 0.0, z0 = 2.5;

    // ball
    ball.radius = 0.2;
    ball.mass = 1.0;
    ball.body = dBodyCreate(world);
    dsSetSphereQuality(3);
    dMassSetZero(&m1);
    dMassSetSphereTotal(&m1, ball.mass, ball.radius);
    dBodySetMass(ball.body, &m1);
    dBodySetPosition(ball.body, x0, y0, z0);

    ball.geom = dCreateSphere(space, ball.radius);
    dGeomSetBody(ball.geom, ball.body);

    // pole
    pole.radius = 0.025;
    pole.length = 1.0;
    pole.mass = 1.0;
    pole.body = dBodyCreate(world);
    dMassSetZero(&m2);
    dMassSetCapsule(&m2, pole.mass, 3, pole.radius, pole.length);
    dBodySetMass(pole.body, &m2);
    dBodySetPosition(pole.body, x0, y0, z0 - 1.0 * pole.length);

    pole.geom = dCreateCapsule(space, pole.radius, pole.length);
    dGeomSetBody(pole.geom, pole.body);

    // create joint
    joint = dJointCreateHinge(world, 0);
    dJointAttach(joint, ball.body, pole.body);
    dJointSetHingeAnchor(joint, x0, y0, z0 - ball.radius);
    dJointSetHingeAxis(joint, 1, 0, 0);
}

void setDrawStuff() {
    fn.version = DS_VERSION;
    fn.start = &start;
    fn.step = &step;
    fn.command = 0;
    fn.stop = 0;
    fn.path_to_textures = "../resources/ode/textures";
}

int main(int argc, char** argv) {
    setDrawStuff();
    dInitODE();
    world = dWorldCreate();
    space = dHashSpaceCreate(0);
    contactgroup = dJointGroupCreate(0);

    dWorldSetGravity(world, 0, 0, -9.8);
    dWorldSetERP(world, 0.5f);
    dWorldSetCFM(world, 0.0005f);

    ground = dCreatePlane(space, 0, 0, 1, 0);

    createBallAndPole();

    dsSimulationLoop(argc, argv, 640, 480, &fn);

    dWorldDestroy(world);
    dCloseODE();
    return 0;
}
