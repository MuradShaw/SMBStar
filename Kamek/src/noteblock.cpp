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
    bool wasSteppedOn;
    bool playerJumped;
    int timer;

    bool playersGoUp[4];

    int updatePlayersGoUp();
    bool playerIsGoUp(int playerID);
    void bouncePlayerWhenJumpedOn(void *player);

    USING_STATES(daEnNoteBlock_c);
    DECLARE_STATE(Wait);
    DECLARE_STATE(GoUp);
    DECLARE_STATE(GoDown);
};

CREATE_STATE_E(daEnNoteBlock_c, Wait);
CREATE_STATE_E(daEnNoteBlock_c, GoUp);
CREATE_STATE_E(daEnNoteBlock_c, GoDown);

const SpriteData NoteBlockData = {ProfileId::noteblock, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile NoteBlockProfile(&daEnNoteBlock_c::build, SpriteId::noteblock, &NoteBlockData, ProfileId::noteblock, ProfileId::noteblock, "noteblock", NoteBlockFileList);

// checks to see if a player is on the block
int daEnNoteBlock_c::updatePlayersGoUp() {
    bool anyPlayersGoUp = false;
    
    for (int i = 0; i < 4; i++) {
        if (dAcPy_c *player = dAcPy_c::findByID(i)) {
            if(player->pos.x >= pos.x - 10 && player->pos.x <= pos.x + 10) {
                if(player->pos.y >= pos.y - 5 && player->pos.y <= pos.y + 12) {
                    this->playersGoUp[i] = true;
                    anyPlayersGoUp = true;

                    wasSteppedOn = false;
                }
            }
            else 
                this->playersGoUp[i] = false;
        }
    }
    
    return anyPlayersGoUp ? 1 : 0;
}

bool daEnNoteBlock_c::playerIsGoUp(int playerID) {
    if(this->playersGoUp[playerID]) {
        return true;
    }
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

    for(int i = 0; i < 4; i++) {
        this->playersGoUp[i] = false;
    }

    wasSteppedOn = true;

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

    dStateBase_c* currentState = this->acState.getCurrentState();

    if(updatePlayersGoUp()) 
        this->pos.y = originalY - 0.75f;
    else if(!updatePlayersGoUp() && !wasSteppedOn) {
        wasSteppedOn = true;
        if(!(strcmp(currentState->getName(), "daEnNoteBlock_c::StateID_Wait")))
            doStateChange(&StateID_GoUp);
    }

    for (int i = 0; i < 4; i++) {
        if (dAcPy_c *player = dAcPy_c::findByID(i)) {
            //OSReport("Player %d is on block: %d\n", i, playerIsGoUp(i));
            //OSReport("player current state: %s\n", player->states2.getCurrentState()->getName());

            if(playerIsGoUp(i) && !(strcmp(player->states2.getCurrentState()->getName(), "daPlBase_c::StateID_Jump"))) {
                bouncePlayer(player, 4.5f);
                playerJumped = true;
            }
        }
    }

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
void daEnNoteBlock_c::executeState_Wait() {}

/* Going up after going down */
void daEnNoteBlock_c::executeState_GoUp() {
    this->pos.y += playerJumped ? 1.25f : 0.50f;

    float targetHeight = playerJumped ? originalY + 6.0f : originalY + 2.0f;
    if(this->pos.y >= targetHeight) {
        playerJumped = false;
        doStateChange(&StateID_GoDown);
    }
}


/* Going down after going up */
void daEnNoteBlock_c::executeState_GoDown() {
    this->pos.y -= 0.50f;

    if(this->pos.y <= originalY) {
        this->pos.y = originalY;
        doStateChange(&StateID_Wait);
    }
}