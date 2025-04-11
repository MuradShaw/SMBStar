#include "MONKEYDLUFFY/map.h"
#include "MONKEYDLUFFY/camera.h"

dWMMap_c *dWMMap_c::instance = 0;

dWMMap_c *dWMMap_c::build() {
    OSReport("Creating Map... \n");

    void *buffer = AllocFromGameHeap1(sizeof(dWMMap_c));
    dWMMap_c *c = new(buffer) dWMMap_c;

    OSReport("Created Map... \n");

    instance = c;
    return c;
}

dWMMap_c::dWMMap_c() : state(this) {}

int dWMMap_c::onCreate()
{
    renderer.allocator.setup(GameHeaps[0], 0x20);
    allocator.link(-1, GameHeaps[0], 0, 0x20);

    nw4r::g3d::ResFile rf(getResource("kuribo", "g3d/kuribo.brres"));
    nw4r::g3d::ResMdl mdlRes = rf.GetResMdl("kuribo");
    mdl.setup(mdlRes, &allocator, 0x224, 1, 0);

    allocator.unlink();

    return true;
}

int dWMMap_c::onDraw()
{
    renderer.scheduleForDrawing();

    mdl.vf1C();
    //mdl.translation(x, y, z);
    mdl.setDrawMatrix(matrix);
    mdl.setScale(1.0f, 1.0f, 1.0f);
    mdl.calcWorld(false);
    mdl.scheduleForDrawing();

    return true;
}

int dWMMap_c::onExecute()
{
    return true;
}
int dWMMap_c::onDelete() { return true; }