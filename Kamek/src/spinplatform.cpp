#include <game.h>
#include <common.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* SpinPlatformList[] = {"obj_spin_platform", NULL};

class daSpinPlatform_c : public dEn_c {
    public:
        int onCreate();
        int onExecute();
        int onDelete();
        int onDraw();

        mHeapAllocator_c allocator;

        nw4r::g3d::ResFile resFile;
        m3d::mdl_c baseModel;
        m3d::mdl_c topModel;

        int timer;
        int rotTimerStuffizoids;

        static dActor_c *build();

        void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);

        USING_STATES(daSpinPlatform_c);
        DECLARE_STATE(Wait);
        DECLARE_STATE(Stand);
        DECLARE_STATE(Launch);
};

CREATE_STATE(daSpinPlatform_c, Wait);
CREATE_STATE(daSpinPlatform_c, Stand);
CREATE_STATE(daSpinPlatform_c, Launch);

// Add state to the player
CREATE_STATE(daPlBase_c, PlatformSpinWait);
CREATE_STATE(daPlBase_c, PlatformSpinFly);

extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

int daSpinDrift_c_platformTimer;

void daPlBase_c::beginState_PlatformSpinWait() {
    daSpinDrift_c_platformTimer = 0;
}
void daPlBase_c::executeState_SpinFly() {
    // Get our controller
    Remocon* con = GetRemoconMng()->controllers[this->settings % 4];
            
    // Drill?
    if(con->heldButtons & WPAD_TWO) {
        // Set player to our spinfly state
        this->states2.setState(&daPlBase_c::StateID_PlatformSpinFly);
    }

    daSpinDrift_c_platformTimer++;
    int spinMultiplier = 1;
    if(daSpinDrift_c_platformTimer > 15) {
        spinMultiplier = 2;
    }
    if(daSpinDrift_c_platformTimer > 30) {
        spinMultiplier = 3;
    }
    if(daSpinDrift_c_platformTimer > 45) {
        spinMultiplier = 4;
    }
    if(daSpinDrift_c_platformTimer > 60) {
        spinMultiplier = 5;
    }

    // Spin!
    rot.y += (0x800 * spinCounter * spinMultiplier);
}

void daPlBase_c::endState_SpinFly() { daSpinDrift_c_platformTimer = 0; }

void daPlBase_c::beginState_PlatformSpinFly() {
    this->speed.y = 16.0f;

    playerFlyTime = 0;
    playerIsFallingState = false;

    // Start our anim
    dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)this) + 0x2A60);
    pmh->mdlClass->startAnimation(71, 1.0f, 0.0f, 0.0f); // Spin
}

void daPlBase_c::executeState_PlatformSpinFly() {
    
    someFlightRelatedFunction(this); // Handles x movement apparently
    //changePlayerAnimation(this);
    //hipAttackStuffizoids(this);

    int spinCounter = 7;
    
    // Already boosted up, now fall slowly
    if(playerIsFallingState)
    {
        if(this->speed.y < -1.0f)
            //this->speed.y -= 0.0025f;
        //else
            this->speed.y = -0.5f;

        // Only allow drill if we're in terminal velocity
        if(this->speed.y == -0.5f)
        {
            dPlayerModelHandler_c *pmh = (dPlayerModelHandler_c*)(((u32)this) + 0x2A60);

            // Get our controller
            Remocon* con = GetRemoconMng()->controllers[this->settings % 4];
            
            // Drill?
            if(con->heldButtons & WPAD_DOWN)
            {
                this->speed.y = -4.0f; // Drill.

                pmh->mdlClass->startAnimation(75, 1.0f, 0.0f, 0.0f); // Drill Spin
            }
            else
            {
                this->speed.y = -0.5f;

                pmh->mdlClass->startAnimation(71, 1.0f, 0.0f, 0.0f); // Spin
            }
        }

        spinCounter = 3;
    }
    else
    {   
        // Gravity
        if(this->speed.y <= 1.0f)
            playerIsFallingState = true;
        else if(playerFlyTime <= 10)
            this->speed.y = 16.0f;
        //else
        //    this->speed.y -= 0.005f;
    }

    // Is our player grounded?
    if(this->collMgr.isOnTopOfTile())
        this->states2.setState(&daPlBase_c::StateID_Jump);  // End spin fly

    // Spin!
    rot.y += (0x800 * spinCounter);

    // Increment timer
    playerFlyTime++;
}

void daPlBase_c::endState_PlatformSpinFly() {
    playerIsFallingState = false;
    playerFlyTime = 0;
}

// Add spin platform to the actor profile
const SpriteData spinPlatformData = { ProfileId::SpinPlatform, 0, 0 , 0 , 0, 4, 4, 0, 0, 0, 0, 0 };
Profile spinPlatformProfile(&daSpinPlatform_c::build, SpriteId::SpinPlatform, &spinPlatformData, ProfileId::SpinPlatform, ProfileId::SpinPlatform, "SpinPlatform", SpinPlatformList);

dActor_c* daSpinPlatform_c::build()
{
    void *buffer = AllocFromGameHeap1(sizeof(daSpinPlatform_c));
    return new(buffer) daSpinPlatform_c;
}

void daSpinPlatform_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	char hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
	
	if(hitType == 1 | hitType == 3)
	{
        if(abs(apOther->speed.x < 1.0f))
        {
            // Set player to our spinfly state
            daPlBase_c *player = (daPlBase_c*)apOther->owner;
            player->states2.setState(&daPlBase_c::StateID_SpinFly);
        }

        doStateChange(&StateID_Stand);
	}
}

void daSpinPlatform_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	baseModel.setDrawMatrix(matrix);
	baseModel.setScale(&scale);
	baseModel.calcWorld(false);

    topModel.setDrawMatrix(matrix);
    topModel.setScale(&scale);
    topModel.calcWorld(false);
}

int daSpinPlatform_c::onCreate()
{
    allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("obj_spin_platform", "g3d/obj_spin_platform_base.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("base");

	baseModel.setup(mdl, &allocator, 0x224, 1, 0);
    SetupTextures_Enemy(&baseModel, 0);

    topModel.setup(this->resFile.GetResMdl("top"), &allocator, 0x224, 1, 0);
	SetupTextures_Enemy(&topModel, 0);

	allocator.unlink();

    ActivePhysics::Info HitMeBaby;
	
	// Dist from center
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 8.0;
	
	// Size
	HitMeBaby.xDistToEdge = 7.0;
	HitMeBaby.yDistToEdge = 7.0;
	
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFBAFFFE;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

    this->onExecute();
    return true;
}

int daSpinPlatform_c::onExecute()
{
    baseModel._vf1C();
    topModel._vf1C();

    /* DO rotation anim */
    // Rotates 90 degrees in nsmbds
    if(this->rotTimerStuffizoids > 7)
    {
        if(SmoothRotation(&this->rot.y, 0x6C00, 0x800)) {
            this->rotTimerStuffizoids = 0;
        }
    }

    this->rotTimerStuffizoids++;

    return true;
}

int daSpinPlatform_c::onDelete()
{
    return true;
}

int daSpinPlatform_c::onDraw()
{
    baseModel.scheduleForDrawing();
    topModel.scheduleForDrawing();

    return true;
}
