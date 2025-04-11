#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"

const char* PSFileList[] = {"power_star", NULL};

class daPowerStar_c : public daBoss {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
	
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::mdl_c blueStar;
	m3d::mdl_c starCage;
	m3d::anmChr_c chrAnimation;
	
	bool collected;
	bool stopRendering;
	bool spawnedIn;
	int timer;
	int pauseTimer;
	u64 triggerEventOverride;
	bool spawnedFromCage;
	
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
	
	USING_STATES(daPowerStar_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Collected);
	DECLARE_STATE(Appear);
};

CREATE_STATE(daPowerStar_c, Wait);
CREATE_STATE(daPowerStar_c, Collected);
CREATE_STATE(daPowerStar_c, Appear);

const SpriteData PowerStarData = {ProfileId::powerstar, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile PowerStarProfile(&daPowerStar_c::build, SpriteId::powerstar, &PowerStarData, ProfileId::powerstar, ProfileId::powerstar, "PowerStar", PSFileList);

dActor_c* daPowerStar_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daPowerStar_c));
	return new(buf) daPowerStar_c;
}

extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

void daPowerStar_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	if(!collected)
	{
		//apOther->owner->speed.y = 0;
		//apOther->owner->speed.x = 0;
		
		doStateChange(&StateID_Collected);
	}
}
void daPowerStar_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	if(!collected)
		doStateChange(&StateID_Collected);
}
void daPowerStar_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) {

}
bool daPowerStar_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daPowerStar_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daPowerStar_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daPowerStar_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool daPowerStar_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daPowerStar_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daPowerStar_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daPowerStar_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daPowerStar_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daPowerStar_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool daPowerStar_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}

void daPowerStar_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);
	
	if(this->spawnedFromCage)
	{
		starCage.setDrawMatrix(matrix);
		starCage.setScale(&scale);
		starCage.calcWorld(false);
	}
	
	// For cage stuff
	if(spawnedFromCage && !spawnedIn)
	{
		blueStar.setDrawMatrix(matrix);
		blueStar.setScale(&scale);
		blueStar.calcWorld(false);
	}
	else
	{
		bodyModel.setDrawMatrix(matrix);
		bodyModel.setScale(&scale);
		bodyModel.calcWorld(false);
	}
}

/*void daPowerStar_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->resFile.GetResAnmChr(name);
	this->animationChr.bind(&this->mdl, anmChr, unk);
	this->mdl.bindAnim(&this->animationChr, unk2);
	this->animationChr.setUpdateRate(rate);
}*/

int daPowerStar_c::onCreate()
{
	char triggerEvent = (settings & 0xFF);
	this->triggerEventOverride = (triggerEvent == 0) ? 0 : ((u64)1 << (triggerEvent - 1));
	
	this->spawnedFromCage = (this->triggerEventOverride == 0) ? false : true;
		
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("power_star", "g3d/power_star.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("power_star");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);
	
	blueStar.setup(resFile.GetResMdl("power_starB"), &allocator, 0x224, 1, 0);

	this->resFile.data = getResource("power_star", "g3d/star_cage.brres");
	nw4r::g3d::ResMdl cMdl = this->resFile.GetResMdl("star_cage");
	starCage.setup(cMdl, &allocator, 0x224, 1, 0);
	
	// Shading
	SetupTextures_MapObj(&bodyModel, 0);
	SetupTextures_MapObj(&blueStar, 0);
	SetupTextures_MapObj(&starCage, 0);

	/*this->anmFile.data = getResource("power_star", "g3d/power_star.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("idle");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);*/
	
	allocator.unlink();
	
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 2.0;
	
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
	
	this->spawnedIn = false;
	this->collected = (spawnedFromCage) ? true : false;
	this->stopRendering = (spawnedFromCage) ? true : false;
	
	doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int daPowerStar_c::onDraw() 
{
	if(spawnedFromCage) starCage.scheduleForDrawing();
	if(!stopRendering) bodyModel.scheduleForDrawing();
	
	// For cage stuff
	if(spawnedFromCage && !spawnedIn)
		blueStar.scheduleForDrawing();
	else if(spawnedFromCage && spawnedIn)
		bodyModel.scheduleForDrawing();
	
	return true;
}

int daPowerStar_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	
	// Let's spawn!
	if(this->spawnedFromCage)
	{
		u64 effectiveFlag = triggerEventOverride | spriteFlagMask;

		if (dFlagMgr_c::instance->flags & effectiveFlag) 
		{
			this->collected = false;
			this->stopRendering = false;
			this->spawnedIn = true;
		} 
	}
	return true;
}

int daPowerStar_c::onDelete() {
	return true;
}

void daPowerStar_c::beginState_Wait() {}
void daPowerStar_c::executeState_Wait() 
{
	this->rot.y += 0x200;
}
void daPowerStar_c::endState_Wait() {}

void daPowerStar_c::beginState_Collected() 
{
    StopBGMMusic();
	
    WLClass::instance->_4 = 5;
	WLClass::instance->_8 = 0;
	dStage32C_c::instance->freezeMarioBossFlag = 1;
	
	this->stopRendering = true;
	
    this->removeMyActivePhysics();
	this->timer = 0;
	this->pauseTimer = 0;
}
void daPowerStar_c::executeState_Collected() 
{
	bool isOnGround = true;
	for(int i = 0; i < 4; i++) {
		if(Player_Active[i] && Player_Lives[i] > 0)
			if(!(GetPlayerOrYoshi(i)->collMgr.isOnTopOfTile()))
				isOnGround = false;
	}
		
	if(!this->collected && isOnGround)
	{
		nw4r::snd::SoundHandle victorySoundHandle;
		PlaySoundWithFunctionB4(SoundRelatedClass, &victorySoundHandle, STRM_BGM_COURSE_CLEAR, 1);
		
		while (this->pauseTimer != 600) 
		{
            this->pauseTimer += 1;
            this->timer = 0;
        }
		
		this->collected = true;
	}
	
	if(isOnGround && timer < 10)
		BossGoalForAllPlayers();
	
	if(this->timer >= 180 && collected)
		ExitStage(WORLD_MAP, 0, BEAT_LEVEL, MARIO_WIPE);
	else
		this->timer += 1;
}
void daPowerStar_c::endState_Collected() {}


