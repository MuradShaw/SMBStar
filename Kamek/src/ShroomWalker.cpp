#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* SWFileList[] = {"toran", NULL};

void stemCollision(ActivePhysics *apThis, ActivePhysics *apOther);
void topCollision(ActivePhysics *apThis, ActivePhysics *apOther);

extern float screenTop;
extern float GameTimerB;

class daShroomWalker_c : dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
	
	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;

	int timer;
	int type;
	bool hasSpikes;
	bool spikesOut;
	bool startWithSpikes;
	
	float previousTop;
	
	ActivePhysics TopStuffs;
	ActivePhysics StimmyYimmy;
	u32 cmgr_returnValue;
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	bool calculateTileCollisions();
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	
	bool loadedIn;
	
	USING_STATES(daShroomWalker_c);
	DECLARE_STATE(Idle);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Turn);
	
	DECLARE_STATE(SpikeIn);
	DECLARE_STATE(SpikeOut);
	
	DECLARE_STATE(Stunned);
	//DECLARE_STATE(Die);
	
	//void _vf148();
	//void _vf14C();
	//bool CreateIceActors();
	void addScoreWhenHit(void *other);
	void bouncePlayerWhenJumpedOn(void *player);

	void spawnHitEffectAtPosition(Vec2 pos);
	void doSomethingWithHardHitAndSoftHitEffects(Vec pos);
	void playEnemyDownSound2();
	void playHpdpSound1(); // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void playEnemyDownSound1();
	void playEnemyDownComboSound(void *player); // AcPy_c/daPlBase_c?
	void playHpdpSound2(); // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void _vf260(void *other); // AcPy/PlBase? plays the SE_EMY_FUMU_%d sounds based on some value
	void _vf264(dStageActor_c *other); // if other is player or yoshi, do Wm_en_hit and a few other things
	void _vf268(void *other); // AcPy/PlBase? plays the SE_EMY_DOWN_SPIN_%d sounds based on some value
	void _vf278(void *other); // AcPy/PlBase? plays the SE_EMY_YOSHI_FUMU_%d sounds based on some value
	
	public: float nearestPlayerDistance();
	public: static dActor_c *build();
};

CREATE_STATE(daShroomWalker_c, Idle);
CREATE_STATE(daShroomWalker_c, Walk);
CREATE_STATE(daShroomWalker_c, Turn);
CREATE_STATE(daShroomWalker_c, SpikeIn);
CREATE_STATE(daShroomWalker_c, SpikeOut);
CREATE_STATE(daShroomWalker_c, Stunned);
//CREATE_STATE(daShroomWalker_c, Die);
	
const SpriteData ToranSpriteData = {ProfileId::ShroomWalker, 8, 0 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile ToranProfile(&daShroomWalker_c::build, SpriteId::ShroomWalker, &ToranSpriteData, ProfileId::ShroomWalker, ProfileId::ShroomWalker, "ShroomWalker", SWFileList);

dActor_c* daShroomWalker_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daShroomWalker_c));
	return new(buf) daShroomWalker_c;
}

///////////////////////
// Externs and States
///////////////////////
	extern "C" bool SpawnEffect(const char*, int, Vec*, S16Vec*, Vec*);

	//FIXME make this dEn_c->used...
	extern "C" char usedForDeterminingStatePress_or_playerCollision(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther, int unk1);
	extern "C" int SomeStrangeModification(dStageActor_c* actor);
	extern "C" void DoStuffAndMarkDead(dStageActor_c *actor, Vec vector, float unk);
	extern "C" int SmoothRotation(short* rot, u16 amt, int unk2);

	// Collision related
	extern "C" void BigHanaPlayer(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" void BigHanaYoshi(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaWeirdGP(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaGroundPound(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaFireball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	extern "C" bool BigHanaIceball(dEn_c* t, ActivePhysics *apThis, ActivePhysics *apOther);
	
	void daShroomWalker_c::addScoreWhenHit(void *other) { }
	void daShroomWalker_c::spawnHitEffectAtPosition(Vec2 pos) { }
	void daShroomWalker_c::doSomethingWithHardHitAndSoftHitEffects(Vec pos) { }
	void daShroomWalker_c::playEnemyDownSound2() { }
	void daShroomWalker_c::playHpdpSound1() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void daShroomWalker_c::playEnemyDownSound1() { }
	void daShroomWalker_c::playEnemyDownComboSound(void *player) { } // AcPy_c/daPlBase_c?
	void daShroomWalker_c::playHpdpSound2() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
	void daShroomWalker_c::_vf260(void *other) { } // AcPy/PlBase? plays the SE_EMY_FUMU_%d sounds based on some value
	void daShroomWalker_c::_vf264(dStageActor_c *other) { } // if other is player or yoshi, do Wm_en_hit and a few other things
	void daShroomWalker_c::_vf268(void *other) { } // AcPy/PlBase? plays the SE_EMY_DOWN_SPIN_%d sounds based on some value

	extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);

void stemCollision(ActivePhysics *apThis, ActivePhysics *apOther) 
{
	if (apOther->owner->name != PLAYER) { return; }
	((dEn_c*)apThis->owner)->_vf220(apOther->owner);
}

void topCollision(ActivePhysics *apThis, ActivePhysics *apOther) 
{
	dEn_c::collisionCallback(apThis, apOther); 
}

void daShroomWalker_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	char hitType;
	hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		
	if(hitType > 0) 
	{
		if(this->spikesOut)
		{
			this->dEn_c::playerCollision(apThis, apOther);
			this->_vf220(apOther->owner);
		}
		
		PlaySound(this, SE_EMY_CMN_STEP);
		this->counter_504[apOther->owner->which_player] = 0xA;
		
		if(this->spikesOut)
			bindAnimChr_and_setUpdateRate("stunned_s", 1, 0.0, 1); 
		else
			bindAnimChr_and_setUpdateRate("stunned", 1, 0.0, 1); 
		
		doStateChange(&StateID_Stunned);
	}
}

bool daShroomWalker_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther)
{
	return BigHanaGroundPound(this, apThis, apOther);
}



void daShroomWalker_c::_vf278(void *other) {
	PlaySound(this, SE_EMY_HANACHAN_STOMP);
}

void daShroomWalker_c::bouncePlayerWhenJumpedOn(void *player) 
{
	bouncePlayer(player, 5.0f);
}

float daShroomWalker_c::nearestPlayerDistance() {
	float bestSoFar = 10000.0f;

	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			if (strcmp(player->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon")) {
				float thisDist = abs(player->pos.x - pos.x);
				if (thisDist < bestSoFar)
					bestSoFar = thisDist;
			}
		}
	}

	return bestSoFar;
}

void daShroomWalker_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daShroomWalker_c::onCreate()
{
	this->type = this->settings >> 28 & 0xF;
	this->hasSpikes = (settings & 0x2000);
	this->startWithSpikes = (this->settings >> 12 & 0xF);
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	if(this->hasSpikes)
		this->resFile.data = getResource("toran", "g3d/t01.brres");
	else
		this->resFile.data = getResource("toran", "g3d/t00.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("toran_model");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_Enemy(&bodyModel, 0);

	if(this->hasSpikes)
		this->anmFile.data = getResource("toran", "g3d/t01.brres");
	else
		this->anmFile.data = getResource("toran", "g3d/t00.brres");
	
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("idle");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	this->scale = (Vec){1.0, 1.0, 1.0};

	ActivePhysics::Info HitMeBaby;	
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 20.0;

	HitMeBaby.xDistToEdge = 43.0;
	HitMeBaby.yDistToEdge = 40.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffba7ffe;
	HitMeBaby.unkShort1C = 0x20000;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	ActivePhysics::Info Stalk;

	Stalk.xDistToCenter = 0.0;
	Stalk.yDistToCenter = 21.0;

	Stalk.xDistToEdge = 15.0;
	Stalk.yDistToEdge = 26.5;

	Stalk.category1 = 0x3;
	Stalk.category2 = 0x9;
	Stalk.bitfield1 = 0x4F;
	Stalk.bitfield2 = 0;
	Stalk.unkShort1C = 0;
	Stalk.callback = &stemCollision;

	this->StimmyYimmy.initWithStruct(this, &Stalk);
	this->StimmyYimmy.addToList();
	
	static const lineSensor_s below(12<<12, 4<<12, 0<<12);
	static const lineSensor_s adjacent(14<<12, 9<<12, 14<<12);
	collMgr.init(this, &below, 0, &adjacent);
		
	collMgr.calculateBelowCollisionWithSmokeEffect();
	cmgr_returnValue = collMgr.isOnTopOfTile();
	
	previousTop = screenTop;
	
	if(this->startWithSpikes == 1)
	{
		bindAnimChr_and_setUpdateRate("spikeout", 1, 0.0, 1);
		doStateChange(&StateID_SpikeOut);
	}
	else if(type < 1)
	{
		bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
		doStateChange(&StateID_Idle);
	}
	else
	{
		bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1); 
		doStateChange(&StateID_Walk);
	}
	
	OSReport("W-Walker Create \n");
	
	this->onExecute();
	return true;
}

int daShroomWalker_c::onDelete() {
	return true;
}

int daShroomWalker_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	
	float dist = nearestPlayerDistance();
	if(dist < 0x120)
	{
		if(!loadedIn)
		{
			if(type == 1)
			{
				bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1); 
				doStateChange(&StateID_Walk);
			}
			else
			{
				bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
				doStateChange(&StateID_Idle);
			}
		}
		
		loadedIn = true;
	}
	else
	{
		bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
		doStateChange(&StateID_Idle);
		
		loadedIn = false;
	}
			
	return true;
}

int daShroomWalker_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daShroomWalker_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

bool daShroomWalker_c::calculateTileCollisions() {
	// Returns true if sprite should turn, false if not.

	HandleXSpeed();
	HandleYSpeed();
	doSpriteMovement();

	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();

	float xDelta = pos.x - last_pos.x;
	if (xDelta >= 0.0f)
		direction = 0;
	else
		direction = 1;

	if (collMgr.isOnTopOfTile()) {
		speed.y = 0.0f;
		
		max_speed.x = (direction == 1) ? -0.4 : 0.4;
	} else {
		x_speed_inc = 0.0f;
	}

	collMgr.calculateAdjacentCollision(0);

	// Switch Direction
	if (collMgr.outputMaybe & (0x15 << direction)) return true;
	
	return false;
}

/* 		Idle 		*/

void daShroomWalker_c::beginState_Idle()
{
	this->timer = 0;
}
void daShroomWalker_c::executeState_Idle()
{
	this->calculateTileCollisions();
	
	if(this->hasSpikes && this->timer >= 1)
	{
		if(spikesOut)
		{
			bindAnimChr_and_setUpdateRate("spikein", 1, 0.0, 1); 
			doStateChange(&StateID_SpikeIn);
		} 
		else 
		{
			bindAnimChr_and_setUpdateRate("spikeout", 1, 0.0, 1); 
			doStateChange(&StateID_SpikeOut);
		}
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->timer+= 1;
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void daShroomWalker_c::endState_Idle()
{}

/* 		Walk 		*/

void daShroomWalker_c::beginState_Walk()
{
	this->max_speed.x = (this->direction) ? -0.4 : 0.4;
	this->speed.x = (direction) ? -0.4 : 0.4;
	
	this->max_speed.y = -4.0;
	this->speed.y = -4.0;
	this->y_speed_inc = -0.1875;
}
void daShroomWalker_c::executeState_Walk()
{
	bool turn = calculateTileCollisions();
	if(turn) doStateChange(&StateID_Turn);
	
	if(this->hasSpikes && this->timer >= 1)
	{
		if(spikesOut)
		{
			bindAnimChr_and_setUpdateRate("spikein", 1, 0.0, 1); 
			doStateChange(&StateID_SpikeIn);
		} 
		else 
		{
			bindAnimChr_and_setUpdateRate("spikeout", 1, 0.0, 1); 
			doStateChange(&StateID_SpikeOut);
		}
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->timer+= 1;
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void daShroomWalker_c::endState_Walk()
{}

/* 		Stunned 		*/

void daShroomWalker_c::beginState_Stunned()
{}

void daShroomWalker_c::executeState_Stunned()
{
	if(this->chrAnimation.isAnimationDone())
	{
		if(type == 0)
		{
			if(spikesOut)
				bindAnimChr_and_setUpdateRate("idle_s", 1, 0.0, 1); 
			else
				bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
			
			doStateChange(&StateID_Idle);
		}
		else
		{
			if(spikesOut)
				bindAnimChr_and_setUpdateRate("walk_s", 1, 0.0, 1);
			else
				bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1); 
			
			doStateChange(&StateID_Walk);
		}
	}
}
void daShroomWalker_c::endState_Stunned()
{}

/* 		Turn 		*/

void daShroomWalker_c::beginState_Turn()
{}
void daShroomWalker_c::executeState_Turn()
{
	this->direction ^= 1;
	doStateChange(&StateID_Walk);
}
void daShroomWalker_c::endState_Turn()
{}

/* 		Spike In / Out 		*/

void daShroomWalker_c::beginState_SpikeIn()
{}
void daShroomWalker_c::executeState_SpikeIn()
{
	if(this->chrAnimation.getCurrentFrame() == 60.0)
	{
		this->spikesOut = false;
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		if(type == 0)
		{
			bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
			doStateChange(&StateID_Idle);
		}
		else
		{
			bindAnimChr_and_setUpdateRate("walk", 1, 0.0, 1); 
			doStateChange(&StateID_Walk);
		}
	}
}
void daShroomWalker_c::endState_SpikeIn()
{}

void daShroomWalker_c::beginState_SpikeOut()
{}
void daShroomWalker_c::executeState_SpikeOut()
{
	if(this->chrAnimation.getCurrentFrame() == 65.0)
	{
		this->spikesOut = true;
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		if(type == 0)
		{
			bindAnimChr_and_setUpdateRate("idle_s", 1, 0.0, 1); 
			doStateChange(&StateID_Idle);
		}
		else
		{
			bindAnimChr_and_setUpdateRate("walk_s", 1, 0.0, 1); 
			doStateChange(&StateID_Walk);
		}
	}
}
void daShroomWalker_c::endState_SpikeOut()
{}