#include "World.h"
#include "GameObject.h"
#include "ode/ode.h"

using namespace std;
using namespace yinpsoft;

void World::Initialize()
{
    dInitODE();


    // TODO: Create the robot cubes
    for (int i = 0; i < 16; i++)
    {
        for (int j = 0; j < 16; j++)
        {
            
        }
    }
}

void World::Destroy()
{
    dCloseODE();
}