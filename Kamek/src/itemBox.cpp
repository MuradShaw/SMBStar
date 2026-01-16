/* Item box that gives an item when collected
From MK
Made by goatboy for Project Star*/

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"

const char *IBarc[] = {
    "obj_coin_big", // [TODO: Change to item box]
    
    NULL
}; 

class daItemBox_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
	
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;

	m3d::mdl_c bodyModel;
	m3d::mdl_c fireflowerModel;
	m3d::mdl_c iceflowerModel;
	m3d::mdl_c penguinModel;
	m3d::mdl_c propellerModel;
	m3d::mdl_c kinokoModel;
	m3d::mdl_c starModel;

	m3d::anmChr_c chrAnimation;

    bool collected;
    ActivePhysics *playerToFollow;
    bool stopRendering;
    bool doingItemEffect;

    int timer;
    int itemIndex;
    int overAllIndex;

    void updateModelMatrices();
    void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

    void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
    void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

    bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
    bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
    
    public: static dActor_c *build();

    USING_STATES(daItemBox_c);
    DECLARE_STATE(Wait);
    DECLARE_STATE(SpawnItemEffect);
    DECLARE_STATE(SpawnItemLegit);
};

CREATE_STATE(daItemBox_c, Wait);
CREATE_STATE(daItemBox_c, SpawnItemEffect);
CREATE_STATE(daItemBox_c, SpawnItemLegit);

const SpriteData ItemBoxData = {ProfileId::itembox, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile ItemBoxProfile(&daItemBox_c::build, SpriteId::itembox, &ItemBoxData, ProfileId::itembox, ProfileId::itembox, "itembox", IBarc);

dActor_c* daItemBox_c::build() {
    void *buf = AllocFromGameHeap1(sizeof(daItemBox_c));
    return new(buf) daItemBox_c;
}
 
extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

void daItemBox_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    if(this->collected)
        return;

    this->collected = true;

    playerToFollow = apOther;

    doStateChange(&StateID_SpawnItemEffect);
}
void daItemBox_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
}
void daItemBox_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
    if(this->collected)
        return;
}
bool daItemBox_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool daItemBox_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool daItemBox_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool daItemBox_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool daItemBox_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
    playerCollision(apThis, apOther);
    return true;
}
bool daItemBox_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
}
bool daItemBox_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
}
bool daItemBox_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
}
bool daItemBox_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
}
bool daItemBox_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
    return true;
}

void daItemBox_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daItemBox_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daItemBox_c::onCreate() {
    allocator.link(-1, GameHeaps[0], 0, 0x20);
    
    // Load the main item box model
    this->resFile.data = getResource("obj_coin_big", "g3d/coin_10.brres");
    nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("coin_10");
    bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
    
    SetupTextures_MapObj(&bodyModel, 0);
    
    allocator.unlink();
    
    ActivePhysics::Info HitMeBaby;
    HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 12.0;
	
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0x200;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;

    this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

    this->doingItemEffect = false;

    doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int daItemBox_c::onDraw() {
	if(!stopRendering) bodyModel.scheduleForDrawing();

    // For showing the item preview
    if(!this->doingItemEffect) return true;
    switch(this->itemIndex) {
        case 0:
            fireflowerModel.scheduleForDrawing();
            break;
        case 1:
            iceflowerModel.scheduleForDrawing();
            break;
        case 2:
            penguinModel.scheduleForDrawing();
            break;
        case 3:
            propellerModel.scheduleForDrawing();
            break;
        case 4:
            kinokoModel.scheduleForDrawing();
            break;
        case 5:
            starModel.scheduleForDrawing();
            break;
    }

	return true;
}

int daItemBox_c::onExecute() {
    acState.execute();
    updateModelMatrices();
    bodyModel._vf1C();

    return true;
}

int daItemBox_c::onDelete() {
    return true;
}

// [TODO: Add anim stuffs]
void daItemBox_c::beginState_Wait() {}
void daItemBox_c::executeState_Wait() {}
void daItemBox_c::endState_Wait() {}

void daItemBox_c::beginState_SpawnItemEffect() {
    this->timer = 0;
    this->itemIndex = 0;
    this->overAllIndex = 0;
    this->doingItemEffect = true;
}
void daItemBox_c::executeState_SpawnItemEffect() {
    // Show a new item every 0.5 seconds, then after 1 second if the overAll is 15+
    if(
        ((this->timer >= 30) && (this->overAllIndex < 15)) || 
        ((this->timer >= 60) && (this->overAllIndex >= 15))
    ) 
    {
        this->timer = 0;

        this->itemIndex++;
        this->overAllIndex++;
    }

    if(this->itemIndex++ >= 6)
        this->itemIndex = 0;

    this->timer++;
}
void daItemBox_c::endState_SpawnItemEffect() {}

void daItemBox_c::beginState_SpawnItemLegit() {}
void daItemBox_c::executeState_SpawnItemLegit() {}
void daItemBox_c::endState_SpawnItemLegit() {}