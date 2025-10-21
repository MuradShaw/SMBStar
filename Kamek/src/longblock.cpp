#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char *LongBlockFileList[] = {"obj_block_long", 0};

class daLongBlock_c : public daEnBlockMain_c {

	Physics::Info physicsInfo;

	int onCreate();
	int onDelete();
	int onExecute();
	int onDraw();
		
	bool isHit;
	int hitCount;
	int timer;
	bool isGroundPound;
	u32 powerup;
	u32 itemSettings;
	u32 coinSettings;
	dStageActor_c *item;
	dStageActor_c *bigCoin;
	
	Vec coinL;
	Vec coinR;
	
	void calledWhenUpMoveExecutes();
	void calledWhenDownMoveExecutes();

	void blockWasHit(bool isDown);

	mHeapAllocator_c allocator;
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile usedBlockResFile;
	m3d::mdl_c model;
	m3d::mdl_c usedModel;

	USING_STATES(daLongBlock_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Hit);

	public: static dActor_c *build();
};


CREATE_STATE(daLongBlock_c, Wait);
CREATE_STATE(daLongBlock_c, Hit);

const SpriteData LongBlockData = {ProfileId::tripleblock, 8, -8, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile LongBlockProfile(&daLongBlock_c::build, SpriteId::tripleblock, &LongBlockData, ProfileId::tripleblock, ProfileId::tripleblock, "tripleblock", LongBlockFileList);

extern "C" int CheckExistingPowerup(void * Player);

dActor_c  *daLongBlock_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daLongBlock_c));
	return new(buf) daLongBlock_c;
}

int daLongBlock_c::onCreate() {
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("obj_block_long", "g3d/obj_block_long.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("block_long");
	model.setup(mdl, &allocator, 0x108, 1, 0);
	
	this->resFile.data = getResource("obj_block_long", "g3d/obj_block_long_empty.brres");
	nw4r::g3d::ResMdl uMdl = this->resFile.GetResMdl("block_long");
	usedModel.setup(uMdl, &allocator, 0x108, 1, 0);
	
	//usedBlockResFile.data = getResource("obj_block_long", "g3d/obj_block_long_empty.brres");
	//usedModel.setup(resFile.GetResMdl("block_long"), &allocator, 0, 1, 0);
	
	SetupTextures_MapObj(&model, 0);
	SetupTextures_MapObj(&usedModel, 0);

	allocator.unlink();

	blockInit(pos.y);

	physicsInfo.x1 = -24;
	physicsInfo.y1 = 16;
	physicsInfo.x2 = 24;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &daEnBlockMain_c::OPhysicsCallback1;
	physicsInfo.otherCallback2 = &daEnBlockMain_c::OPhysicsCallback2;
	physicsInfo.otherCallback3 = &daEnBlockMain_c::OPhysicsCallback3;

	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &daEnBlockMain_c::PhysicsCallback1;
	physics.callback2 = &daEnBlockMain_c::PhysicsCallback2;
	physics.callback3 = &daEnBlockMain_c::PhysicsCallback3;
	physics.addToList();
	
	this->_68B = 1;
	physics._D8 &= ~0b00101000;

	this->isHit = false;
	this->hitCount = 0;
	
	this->pos.z = 200.0f;
	
	doStateChange(&daLongBlock_c::StateID_Wait);
	this->onExecute();
	
	return true;
}


int daLongBlock_c::onDelete() {
	physics.removeFromList();

	return true;
}


int daLongBlock_c::onExecute() {
	model._vf1C();
	acState.execute();
	physics.update();
	
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	
	if(!this->isHit)
	{
		model.setDrawMatrix(matrix);
		model.setScale(&scale);
		model.calcWorld(false);
	}
	else
	{
		//this->pos.z = 9900;
		usedModel.setDrawMatrix(matrix);
		usedModel.setScale(&scale);
		usedModel.calcWorld(false);
	}
	
	this->blockUpdate();

	//if (acState.getCurrentState()->isEqual(&StateID_Wait)) {
	//	checkZoneBoundaries(0);
	//}

	return true;
}


int daLongBlock_c::onDraw() {
	if(!this->isHit)
	{
		model.scheduleForDrawing();
	}
	else
	{
		usedModel.scheduleForDrawing();
	}

	return true;
}

// AVERT YOUR EYES
void daLongBlock_c::blockWasHit(bool isDown) {
	pos.y = initialY;
	
	// Get our powerup from reggie
	this->powerup = (this->settings >> 28 & 0xF);
	
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
		doStateChange(&StateID_Hit);
		return;
	}
	
	// Powerups: 0 = small 1 = big 2 = fire 3 = mini 4 = prop 5 = peng 6 = ice
	int p = CheckExistingPowerup(dAcPy_c::findByID(this->playerID));
	
	/*	Powerup Settings	*/
	static u32 ingamePowerupIDs[] = {0x0,0x1,0x2,0x7,0x9,0xE,0x11,0x15,0x19,0x6,0x2}; //Mushroom, Star, Coin, 1UP, Fire Flower, Ice Flower, Penguin, Propeller, Mini Shroom, Hammer, 10 Coin
	this->powerup = ingamePowerupIDs[this->powerup];
	
	/*	Sound logic	*/
	nw4r::snd::SoundHandle handle;
	PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_GET_COIN, 1);
	
	if((powerup != 0x2 && powerup != 0x15) || p == 0 || p == 3) 
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_APPEAR, 1); //Item sound

	if(powerup == 0x15 && p != 0) 
		PlaySoundWithFunctionB4(SoundRelatedClass, &handle, SE_OBJ_ITEM_PRPL_APPEAR, 1); //Propeller sound
	
	/*	Coin/powerup positions and settings logic	*/
	this->itemSettings = 0 | (powerup << 0) | (((isDown) ? 3 : 2) << 18) | (4 << 9) | (2 << 10) | (this->playerID + 8 << 16);
	this->coinSettings = 0 | (0x2 << 0) | (((isDown) ? 3 : 2) << 18) | (4 << 9) | (2 << 10) | (this->playerID + 8 << 16);
		
	this->coinL = (Vec) {this->pos.x - 16, this->pos.y + ((isDown) ? -5 : -9), this->pos.z};
	this->coinR = (Vec) {this->pos.x + 16, this->pos.y + ((isDown) ? -5 : -9), this->pos.z};
	//Vec itemPos = (Vec){this->pos.x, ((isDown) ? this->pos.y - 20 : this->pos.y - 5), this->pos.z};
	
	/*	Create our actors	*/
	item = dStageActor_c::create(EN_ITEM, itemSettings, &this->pos, 0, 0);
	dStageActor_c *leftCoin = dStageActor_c::create(EN_ITEM, coinSettings, &coinL, 0, 0);
	dStageActor_c *rightCoin = dStageActor_c::create(EN_ITEM, coinSettings, &coinR, 0, 0); 
	
	//if(powerup != 0x15 || p == 0 || p == 3) 
	//{
		item->pos.z = 100.0f;
	//}
	
	// 10 coin
	if(powerup == 0x2 && ((this->settings >> 28 & 0xF) == 10))
		this->hitCount++;
	else
		this->hitCount = 10;
	
	if(this->hitCount >= 10)
		doStateChange(&StateID_Hit);
	else
		doStateChange(&StateID_Wait);
}

void daLongBlock_c::calledWhenUpMoveExecutes() {
	if (initialY >= pos.y)
		blockWasHit(false);
}

void daLongBlock_c::calledWhenDownMoveExecutes() {
	if (initialY <= pos.y)
		blockWasHit(true);
}

void daLongBlock_c::beginState_Wait() {
}

void daLongBlock_c::endState_Wait() {
}

void daLongBlock_c::executeState_Wait() {
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


void daLongBlock_c::beginState_Hit() {
	this->isHit = true;
}
void daLongBlock_c::executeState_Hit() {
}
void daLongBlock_c::endState_Hit() {
}
