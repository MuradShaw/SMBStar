#include <common.h>
#include <game.h>

const char *NoteBlockFileList[] = {"block_rotate", 0};

class daEnNoteBlock_c : public daEnBlockMain_c {
public:
    Physics::Info physicsInfo;

    int onCreate();
    int onDelete();
    int onExecute();
    int onDraw();

    mHeapAllocator_c allocator;
    nw4r::g3d::ResFile resFile;
    m3d::mdl_c model;

    static daEnNoteBlock_c *build();
};

const SpriteData NoteBlockData = {ProfileId::noteblock, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile NoteBlockProfile(&daEnNoteBlock_c::build, SpriteId::noteblock, &NoteBlockData, ProfileId::noteblock, ProfileId::noteblock, "noteblock", NoteBlockFileList);

int daEnNoteBlock_c::onCreate() {
    allocator.link(-1, GameHeaps[0], 0, 0x20);

    resFile.data = getResource("block_rotate", "g3d/block_rotate.brres");
    model.setup(resFile.GetResMdl("block_rotate"), &allocator, 0, 1, 0);
    SetupTextures_MapObj(&model, 0);

    allocator.unlink();

    blockInit(pos.y);

    physicsInfo.x1 = -8;
    physicsInfo.y1 = 8;
    physicsInfo.x2 = 8;
    physicsInfo.y2 = -8;

    physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
    physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
    physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

    physics.setup(this, &physicsInfo, 3, currentLayerID);
    physics.flagsMaybe = 0x260;
    physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
    physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
    physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
    physics.addToList();

    return true;
}

int daEnNoteBlock_c::onDelete() {
    physics.removeFromList();
    return true;
}

int daEnNoteBlock_c::onExecute() {
    physics.update();
    blockUpdate();
    checkZoneBoundaries(0);
    return true;
}

int daEnNoteBlock_c::onDraw() {
    matrix.translation(pos.x, pos.y, pos.z);
    matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

    model.setDrawMatrix(matrix);
    model.setScale(&scale);
    model.calcWorld(false);
    model.scheduleForDrawing();

    return true;
}

daEnNoteBlock_c *daEnNoteBlock_c::build() {
    void *buffer = AllocFromGameHeap1(sizeof(daEnNoteBlock_c));
    daEnNoteBlock_c *c = new(buffer) daEnNoteBlock_c;
    return c;
}