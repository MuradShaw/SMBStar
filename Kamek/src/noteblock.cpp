#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"


/**
    Note Block

    Actually should be called "Face Block"

    Blocks with faces on them that the player can walk on, and when the player jumps on them, they launch the player into the air.
    The block will slightly dip down when the player is on it, and spring up when the player jumps off of it.
 */

const char *NoteBlockFileList[] = {"block_rotate", NULL};

class daEnNoteBlock_c : public daEnBlockMain_c {
public:
    static dActor_c* build();

    Physics::Info physicsInfo;

    int onCreate();
    int onDelete();
    int onExecute();
    int onDraw();

    mHeapAllocator_c allocator;
    nw4r::g3d::ResFile resFile;
    m3d::mdl_c model;

    float originalY;
    bool jumpable;
    int timer;

    int playerOnBlock();
    void bouncePlayerWhenJumpedOn(void *player);

    USING_STATES(daEnNoteBlock_c);
    DECLARE_STATE(Wait);

    DECLARE_STATE(OnBlock);
    DECLARE_STATE(OffBlock);

    DECLARE_STATE(Spring);
};

CREATE_STATE(daEnNoteBlock_c, Wait);
CREATE_STATE(daEnNoteBlock_c, OnBlock);
CREATE_STATE(daEnNoteBlock_c, OffBlock);
CREATE_STATE(daEnNoteBlock_c, Spring);

const SpriteData NoteBlockData = {ProfileId::noteblock, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile NoteBlockProfile(&daEnNoteBlock_c::build, SpriteId::noteblock, &NoteBlockData, ProfileId::noteblock, ProfileId::noteblock, "noteblock", NoteBlockFileList);

// checks to see if a player is on the block via position check
// returns 0 if no player is on block
// returns 1 if player is walking on block
// returns 2 if player is about to jump from block
int daEnNoteBlock_c::playerOnBlock() {
    for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
            if(player->pos.x >= pos.x - 10 && player->pos.x <= pos.x + 10) {
                if(player->pos.y >= pos.y - 5 && player->pos.y <= pos.y + 12) {
                    return 1; // Player is walking on block
                }
            }
        }
    }
    
    return 0; // No player on block
}

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

    originalY = pos.y;

    doStateChange(&StateID_Wait);

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

    acState.execute();

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

dActor_c *daEnNoteBlock_c::build() {
    void *buf = AllocFromGameHeap1(sizeof(daEnNoteBlock_c));
	return new(buf) daEnNoteBlock_c;
}


/* Wait State */
void daEnNoteBlock_c::beginState_Wait() {}

void daEnNoteBlock_c::executeState_Wait() {
    if(playerOnBlock()) {
        doStateChange(&StateID_OnBlock);
    }
}

void daEnNoteBlock_c::endState_Wait() {}


/* On Block State */
void daEnNoteBlock_c::beginState_OnBlock() {
    this->pos.y -= 0.75f;
}

void daEnNoteBlock_c::executeState_OnBlock() {
    if(!playerOnBlock()) {
        doStateChange(&StateID_OffBlock);
    }

    for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
                if (!(strcmp(player->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Jump")))
                    bouncePlayer(player, 5.0f);
            }
    }
}

void daEnNoteBlock_c::endState_OnBlock() {}


/* Off Block State */
void daEnNoteBlock_c::beginState_OffBlock() {
    this->pos.y = originalY;
}

void daEnNoteBlock_c::executeState_OffBlock() {
    doStateChange(&StateID_Wait);
}

void daEnNoteBlock_c::endState_OffBlock() {}


/* Spring State */
void daEnNoteBlock_c::beginState_Spring() {
    this->pos.y += 0.75;
    this->timer = 0;

    jumpable = true;
}

void daEnNoteBlock_c::executeState_Spring() {
    this->timer++;

    if(timer >= 5) {
        jumpable = false;
        doStateChange(&StateID_OffBlock);
    }
}

void daEnNoteBlock_c::endState_Spring() {}
