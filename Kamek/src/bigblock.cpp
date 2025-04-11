/*
    *  bigblock.cpp
    *  SMFinale Project
    *
    *  Created by FruitSmasher on 5/6/2024
    *
    *  Big brick block that takes multiple hits to break. 
    *  Amount of hits depends on the size of the block, 
    *  as well as the character and their powerup state.
*/


#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char *BigBlockFileList[] = {"obj_brick_big", 0};

class daBigBlock_c : public daEnBlockMain_c {
    Physics::Info physicsInfo;

    int onCreate();
    int onDelete();
    int onExecute();
    int onDraw();
        
    bool isBroken;
    int hitCount;
    float health;
    int timer;
    int size;
    bool isGroundPound;
    int oldHitCount;
    u32 powerup;
    u32 itemSettings;
    u32 coinSettings;
    dStageActor_c *bigCoin;
    
    void calledWhenUpMoveExecutes();
    void calledWhenDownMoveExecutes();
    void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);

    void blockWasHit(bool isDown);

    mHeapAllocator_c allocator;
    nw4r::g3d::ResFile resFile;
    nw4r::g3d::ResFile anmFile;
    m3d::anmChr_c chrAnimation;
    nw4r::g3d::ResFile usedBlockResFile;
    m3d::mdl_c model;
    m3d::mdl_c usedModel;

    USING_STATES(daBigBlock_c);
    DECLARE_STATE(Wait);

    public: static dActor_c *build();
};

CREATE_STATE(daBigBlock_c, Wait);

const SpriteData BigBlockData = {ProfileId::bigblock, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile BigBlockProfile(&daBigBlock_c::build, SpriteId::bigblock, &BigBlockData, ProfileId::bigblock, ProfileId::bigblock, "bigblock", BigBlockFileList);

extern "C" int CheckExistingPowerup(void * Player);

dActor_c  *daBigBlock_c::build() {
    void *buf = AllocFromGameHeap1(sizeof(daBigBlock_c));
    return new(buf) daBigBlock_c;
}

void daBigBlock_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daBigBlock_c::onCreate() {
    this->size = (this->settings >> 12 & 0xF);

    allocator.link(-1, GameHeaps[0], 0, 0x20);

    if(size == 0) // 2x2
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_big.brres");
    else if(size == 1) // 3x3
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_bigger.brres");
    else if(size == 2) // 4x4
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_even_bigger.brres");
    else if(size == 3) // 5x5
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_way_too_big.brres");
    else if(size == 4) // 6x6
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_huge.brres");
    else // 7x7
        this->resFile.data = getResource("obj_brick_big", "g3d/obj_brick_biggest.brres");
    
    nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("obj_brick_big");
    model.setup(mdl, &allocator, 0x108, 1, 0);

    this->anmFile.data = getResource("obj_brick_big", "g3d/obj_brick_big.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("damage1");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);

    allocator.unlink();

    blockInit(pos.y);

    physicsInfo.x1 = -8 * (size + 2);
    physicsInfo.y1 = 8 * (size + 2);
    physicsInfo.x2 = 8 * (size + 2);
    physicsInfo.y2 = -8 * (size + 2);

    physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
    physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
    physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

    physics.setup(this, &physicsInfo, 3, currentLayerID);

    this->hitCount = 0;
    this->scale = (Vec){2.0f, 2.0f, 2.0f};

    physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();

    this->hitCount = 0;
    this->isBroken = false;
    
    this->health = this->size + 2;

    doStateChange(&daBigBlock_c::StateID_Wait);

    return true;
}

int daBigBlock_c::onDelete() {
    physics.removeFromList();

    return true;
}

int daBigBlock_c::onExecute() {
    model._vf1C();
	acState.execute();
	physics.update();
    
    this->scale = (Vec){2.0f, 2.0f, 2.0f};

	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

    model.setDrawMatrix(matrix);
    model.setScale(&scale);
    model.calcWorld(false);

    // Delete when broken
    if(this->isBroken)
        if(this->chrAnimation.isAnimationDone())
            this->Delete(1);

    return true;
}

int daBigBlock_c::onDraw() {
    this->model.scheduleForDrawing();

    return true;
}

void daBigBlock_c::blockWasHit(bool isDown) {
    pos.y = initialY;

	this->scale = (Vec){2.0f, 2.0f, 2.0f};
    
    // Characters: 0 = Mario, 1 = Luigi, 2 = Wario, 3 = Waluigi
    int player_id = Player_ID[this->playerID];
    // Powerups: 0 = small 1 = big 2 = fire 3 = mini 4 = prop 5 = peng 6 = ice
    
    dAcPy_c *player = dAcPy_c::findByID(3);
	int p = CheckExistingPowerup(player);

    // Handle hit calculations
    /*
        Mario = 1 hit point 
        Luigi = 0.75 hit points
        Wario = 2 hit points
        Waluigi = 0.5 hit points

        Small debuff = -0.10 hit point

        Health for blocks:
        2x2 = 2 hp
        3x3 = 3 hp
        ...
        7x7 = 7 hp
    */

    OSReport("PlayerID: %d\n", p);

    // Calculate hit points
    float hitPoints = 1.0f;

    if(player_id == 1) hitPoints = 0.75f;
    else if(player_id == 2) hitPoints = 2.0f;
    else if(player_id == 3) hitPoints = 0.5f;

    if(p == 0) 
        //if((player_id != 2) && (hitCount != 0)) // Skip small debuff for wario on the first hit
            hitPoints = 0.0f;

    // Calculate health
    this->health -= hitPoints;
    this->hitCount++;

    // Check: if block is not broken, return
    if(this->health > 0) {
        //Check if health is halfway
        if(this->health >= ((this->size + 2) / 2))
        {
            if(this->hitCount == 0)
                bindAnimChr_and_setUpdateRate("damage1", 1, 0.0, 1.0);
            else
                bindAnimChr_and_setUpdateRate("damage1b", 1, 0.0, 1.0);

            this->oldHitCount = this->hitCount;
        }
        else
            if(this->hitCount - this->oldHitCount == 1)
                bindAnimChr_and_setUpdateRate("damage2", 1, 0.0, 1.0);
            else
                bindAnimChr_and_setUpdateRate("damage2b", 1, 0.0, 1.0);
        
        doStateChange(&StateID_Wait);
        return;
    }

    this->isBroken = true;
    physics.removeFromList();
    bindAnimChr_and_setUpdateRate("break", 1, 0.0, 1.0);
	
	// Big coin custom stuffs
	if(powerup > 10)
	{
		//				   type														                   gravity 
		u32 bcSettings = ((powerup - 10) | (0 << 2) | (0 << 4) | (0 << 6) | (0 << 8) | (0 << 10) | (2 << 12) | (0 << 14));
		Vec bcPos = (Vec){this->pos.x, ((isDown) ? this->pos.y - 5 : this->pos.y + 25), this->pos.z};
			
		bigCoin = dStageActor_c::create(bigcoin, bcSettings, &bcPos, 0, 0);
		
		if(!isDown) { 
			bigCoin->speed.y = 6.0f; 
			bigCoin->speed.x = 2.5f; 
		}
		
		// We are OUT of here
		return;
	}
	
	this->powerup = (this->settings >> 28 & 0xF);
	static u32 ingamePowerupIDs[] = {0x0,0x1,0x2,0x7,0x9,0xE,0x11,0x15,0x19,0x6,0x2}; //Mushroom, Star, Coin, 1UP, Fire Flower, Ice Flower, Penguin, Propeller, Mini Shroom, Hammer, 10 Coin
	this->powerup = ingamePowerupIDs[this->powerup];
	
	// Play sounds
	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_GET_COIN, 1);
	
	if((powerup != 0x2 && powerup != 0x15) || p == 0 || p == 3) 
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1); //Item sound

	if(powerup == 0x15 && p != 0) 
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_PRPL_APPEAR, 1); //Propeller sound
	
	// Set pos's of things based on groundpound or not
	this->itemSettings = 0 | (powerup << 0) | (((isDown) ? 3 : 2) << 18) | (0 << 9) | (0 << 10) | (this->playerID + 8 << 16);
	this->coinSettings = 0 | (0x2 << 0) | (((isDown) ? 3 : 2) << 18) | (4 << 9) | (2 << 10) | (this->playerID + 8 << 16); //Setting non-GP settings
	
	dStageActor_c *item = dStageActor_c::create(EN_ITEM, itemSettings, &this->pos, 0, 0);
	//dStageActor_c *leftCoin = dStageActor_c::create(EN_ITEM, coinSettings, &coinL, 0, 0);
	//dStageActor_c *rightCoin = dStageActor_c::create(EN_ITEM, coinSettings, &coinR, 0, 0); 
	
	if(powerup != 0x15 || p == 0 || p == 3) 
		item->pos.z = 100.0f; 
}

void daBigBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daBigBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}

void daBigBlock_c::beginState_Wait() {
}
void daBigBlock_c::endState_Wait() {
}
void daBigBlock_c::executeState_Wait() {
    int result = blockResult();

    if (result == 0)
        return;

    if (result == 1) {
        doStateChange(&daEnBlockMain_c::StateID_UpMove);
        anotherFlag = 2;
        isGroundPound = false;
    } else {
        doStateChange(&daEnBlockMain_c::StateID_DownMove);
        anotherFlag = 1;
        isGroundPound = true;
    }
}