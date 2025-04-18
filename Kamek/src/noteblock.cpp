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
    int timer;

    bool playersGoUp[4];

    int updatePlayersGoUp();
    bool playerIsGoUp(int playerID);
    void bouncePlayerWhenJumpedOn(void *player);

    USING_STATES(daEnNoteBlock_c);
    DECLARE_STATE(Wait);

    DECLARE_STATE(GoUp);
    DECLARE_STATE(GoDown);

    DECLARE_STATE(Spring);
};

CREATE_STATE(daEnNoteBlock_c, Wait);
CREATE_STATE(daEnNoteBlock_c, GoUp);

CREATE_STATE(daEnNoteBlock_c, GoDown);
CREATE_STATE(daEnNoteBlock_c, Spring);

// Add state to the player
CREATE_STATE(daPlBase_c, NoteBlockLaunch);

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
                player->states2.setState(&daPlBase_c::StateID_NoteBlockLaunch);
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
void daEnNoteBlock_c::beginState_Wait() {}

void daEnNoteBlock_c::executeState_Wait() {}

void daEnNoteBlock_c::endState_Wait() {}


/* Going up after going down */
void daEnNoteBlock_c::beginState_GoUp() {}

void daEnNoteBlock_c::executeState_GoUp() {
    this->pos.y += 0.25f;

    if(this->pos.y >= originalY + 1.50f)
        doStateChange(&StateID_GoDown);
}

void daEnNoteBlock_c::endState_GoUp() {}


/* Going down after going up */
void daEnNoteBlock_c::beginState_GoDown() {}

void daEnNoteBlock_c::executeState_GoDown() {
    this->pos.y -= 0.25f;

    if(this->pos.y <= originalY) {
        doStateChange(&StateID_Wait);
    }
}

void daEnNoteBlock_c::endState_GoDown() {
    this->pos.y = originalY;
}


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
        doStateChange(&StateID_GoDown);
    }
}

void daEnNoteBlock_c::endState_Spring() {}

// Stolen from Spindrift.cpp, change later
bool playerIsFallingState2;
int playerFlyTime2;

void daPlBase_c::beginState_NoteBlockLaunch() {
    this->speed.y = 5.0f;

    playerFlyTime2 = 0;
    playerIsFallingState2 = false;

    // Start our anim
    dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)this) + 0x2A60);
    pmh->mdlClass->startAnimation(71, 1.0f, 0.0f, 0.0f); // Spin
}

void daPlBase_c::executeState_NoteBlockLaunch() {
    
    someFlightRelatedFunction(this); // Handles x movement apparently
    //changePlayerAnimation(this);
    //hipAttackStuffizoids(this);

    int spinCounter = 7;
    
    // Already boosted up, now fall slowly
    if(playerIsFallingState2)
    {
        if(this->speed.y < -1.0f)
            //this->speed.y -= 0.0025f;
        //else
            this->speed.y = -0.35f;

        // Only allow drill if we're in terminal velocity
        if(this->speed.y == -0.35f)
        {
            dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)this) + 0x2A60);

            // Get our controller
            Remocon* con = GetRemoconMng()->controllers[this->settings % 4];
            
            // Drill?
            if(con->heldButtons & WPAD_DOWN)
            {
                this->speed.y = -4.35f; // Drill.

                pmh->mdlClass->startAnimation(75, 1.0f, 0.0f, 0.0f); // Drill Spin
            }
            else
            {
                this->speed.y = -0.35f;

                pmh->mdlClass->startAnimation(71, 1.0f, 0.0f, 0.0f); // Spin
            }
        }

        spinCounter = 3;
    }
    else
    {   
        // Gravity
        if(this->speed.y <= 1.0f)
            playerIsFallingState2 = true;
        else if(playerFlyTime2 <= 18)
            this->speed.y = 5.0f;
        else if(playerFlyTime2 > 18 && playerFlyTime2 <= 28)
            this->speed.y = 3.5f;
        else if(playerFlyTime2 > 28 && playerFlyTime2 <= 32)
            this->speed.y = 1.5f;
        //else
        //    this->speed.y -= 0.005f;
    }

    // Is our player grounded?
    if(this->collMgr.isOnTopOfTile())
        this->states2.setState(&daPlBase_c::StateID_Jump);  // End spin fly

    // Spin!
    rot.y += (0x800 * spinCounter);

    // Increment timer
    playerFlyTime2++;
}

void daPlBase_c::endState_NoteBlockLaunch() {
    playerIsFallingState2 = false;
    playerFlyTime2 = 0;
}
