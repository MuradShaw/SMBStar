#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

#include "path.h"
#include "boss.h"

const char* EVPFileList[] = {"obj_coin_switch", NULL};

class dEnEventCoinPath_c : public dEnPath_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
	
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;
	
	bool collected;
	bool stopRendering;
	bool timedEvent;
	int timeToEnd;
	int timer;
	u8 targetEventID;
	u8 event;
	
	u32 cmgr_returnValue;
	bool isOnTopOfTile;
	StandOnTopCollider collider;
	
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
	
	USING_STATES(dEnEventCoinPath_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Collected);
};

CREATE_STATE(dEnEventCoinPath_c, Wait);
CREATE_STATE(dEnEventCoinPath_c, Collected);

const SpriteData EventcoinPathData = {ProfileId::eventCoinPath, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile EventcoinPathProfile(&dEnEventCoinPath_c::build, SpriteId::eventCoinPath, &EventcoinPathData, ProfileId::eventCoinPath, ProfileId::eventCoinPath, "eventCoinPath", EVPFileList);

dActor_c* dEnEventCoinPath_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dEnEventCoinPath_c));
	return new(buf) dEnEventCoinPath_c;
}

extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

void dEnEventCoinPath_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	if(!collected)
		doStateChange(&StateID_Collected);
}
void dEnEventCoinPath_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	if(!collected)
		doStateChange(&StateID_Collected);
}
void dEnEventCoinPath_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

}
bool dEnEventCoinPath_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dEnEventCoinPath_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dEnEventCoinPath_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dEnEventCoinPath_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dEnEventCoinPath_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dEnEventCoinPath_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dEnEventCoinPath_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	if(!collected)
		doStateChange(&StateID_Collected);
	
	return true;
}
bool dEnEventCoinPath_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dEnEventCoinPath_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dEnEventCoinPath_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dEnEventCoinPath_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}

void dEnEventCoinPath_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

/*void dEnEventCoinPath_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->mdl, anmChr, unk);
	this->mdl.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}*/

int dEnEventCoinPath_c::onCreate()
{
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("obj_coin_switch", "g3d/obj_coin_switch.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("switch_coin");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_MapObj(&bodyModel, 0);

	/*this->anmFile.data = getResource("power_star", "g3d/power_star.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("idle");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);*/
	
	allocator.unlink();
	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	
	HitMeBaby.xDistToEdge = 12.0;
	HitMeBaby.yDistToEdge = 12.0;
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	this->collected = false;
	
	this->timeToEnd = (this->settings >> 8 & 0b11111111);	
	this->timeToEnd = this->timeToEnd * 60;
	
	if(this->timeToEnd > 1)
		timedEvent = true;
	else
		timedEvent = false;
	
	targetEventID = ((this->settings >> 28 & 0xF) - 1);
	//this->targetEventID = (this->targetEventID == 0) ? 0 : (this->targetEventID - 1);	// For some bloody reason the damn thing is bloody pressed insnit
	
	collider.init(this,
			/*xOffset=*/0.0f, /*yOffset=*/0.0f,
			/*topYOffset=*/0,
			/*rightSize=*/0, /*leftSize=*/0,
			/*rotation=*/0, /*_45=*/1
			);

	collider._47 = 0xA;
	collider.flags = 0x80180 | 0xC00;

	collider.addToList();
	
	OSReport("Event coin path \n");
	
	doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int dEnEventCoinPath_c::onDraw() {
	if(!stopRendering) bodyModel.scheduleForDrawing();
	return true;
}

int dEnEventCoinPath_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	
	this->rot.y += 0x400;
	waitForPlayer = 0;
	
	//dStateBase_c* currentState = this->acState.getCurrentState();
	//if(currentState == &dEnPath_c::StateID_Done)
	//{
	//	doStateChange(&StateID_Init);
	//}
	
	return true;
}

int dEnEventCoinPath_c::onDelete() {
	return true;
}

void dEnEventCoinPath_c::beginState_Wait() {}
void dEnEventCoinPath_c::executeState_Wait() 
{
	doStateChange(&StateID_Init);
}
void dEnEventCoinPath_c::endState_Wait() {}

void dEnEventCoinPath_c::beginState_Collected() 
{
	this->stopRendering = true;
	
    this->removeMyActivePhysics();
	this->timer = 0;
	
	event = targetEventID;
}
void dEnEventCoinPath_c::executeState_Collected() 
{
	// If our event is already active we'll kill it later
	bool alreadyActivated = dFlagMgr_c::instance->active(event);
	
	// "later"
	if(!collected) dFlagMgr_c::instance->set(event, 0, !alreadyActivated, false, false);
	this->collected = true;
	
	if(!timedEvent)
		this->Delete(1);
	else
	{
		if(this->timer > timeToEnd)
		{
			dFlagMgr_c::instance->set(event, 0, false, false, false);
			this->Delete(1);
		}
	}
	
	this->timer++;
}
void dEnEventCoinPath_c::endState_Collected() {}


