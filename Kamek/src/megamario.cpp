/*
    A literal mega mario
*/

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"

const char* MegaMarioArc[] = {"obj_mega", NULL};
	
class dMegaMario_c : public daBoss {
    int onCreate();
    int onExecute();
    int onDelete();
    int onDraw();

    mHeapAllocator_c allocator;

    nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;
	nw4r::g3d::ResAnmTexPat anmPat;
	m3d::anmTexPat_c patAnimation;
    
    mEf::es2 glow;

	lineSensor_s belowSensor;
	lineSensor_s adjacentSensor;
	lineSensor_s aboveSensor;

    int timer;
    int Baseline;
	public: int texState;
	int walkTimer;
    u32 cmgr_returnValue;
	bool weAreReadyForAction;
	float originalX;
	int speedFrameIncrease;
	bool wasOnGround;
	bool canBreakThisLanding;
	bool weJumped;
    
    bool calculateTileCollisions();

	void texPat_bindAnimChr_and_setUpdateRate(const char* name);
    void updateModelMatrices();
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	float nearestPlayerDistance();

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
    public: dAcPy_c *daPlayer;
    public: void setdaPlayer(dAcPy_c *player);
	
	USING_STATES(dMegaMario_c);
	DECLARE_STATE(Walk);
	DECLARE_STATE(SpawnScale);
};

CREATE_STATE(dMegaMario_c, Walk);
CREATE_STATE(dMegaMario_c, SpawnScale);

// Add state to the player
CREATE_STATE(daPlBase_c, MegaMario);

const SpriteData MegaMarioData = {ProfileId::megamario, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile MegaMarioProfile(&dMegaMario_c::build, SpriteId::megamario, &MegaMarioData, ProfileId::megamario, ProfileId::megamario, "megamario", MegaMarioArc);

dActor_c* dMegaMario_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dMegaMario_c));
	return new(buf) dMegaMario_c;
}

void dMegaMario_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	daPlBase_c *player = (daPlBase_c*)apOther->owner;

	if(this->scale.x == 1.4f)
	{
		player->states2.setState(&daPlBase_c::StateID_MegaMario);
		doStateChange(&StateID_SpawnScale);
	}

	return;
}
void dMegaMario_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	playerCollision(apThis, apOther);
}
void dMegaMario_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) 
{
    u16 name = ((dEn_c*)apOther->owner)->name;

	// Ignore all these
	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND 
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM) 
		{ return; }
	
    ((dEn_c*)apOther->owner)->kill();

	return;
}
bool dMegaMario_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dMegaMario_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dMegaMario_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dMegaMario_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}
bool dMegaMario_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dMegaMario_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dMegaMario_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return true;
}
bool dMegaMario_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dMegaMario_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dMegaMario_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool dMegaMario_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}

void dMegaMario_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void dMegaMario_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

void dMegaMario_c::texPat_bindAnimChr_and_setUpdateRate(const char* name) {
	this->anmPat = this->resFile.GetResAnmTexPat(name);
	this->patAnimation.bindEntry(&this->bodyModel, &anmPat, 0, 1);
	this->bodyModel.bindAnim(&this->patAnimation);
}

int dMegaMario_c::onCreate()
{
	this->texState = 2;
	this->direction = 1;
	this->speedFrameIncrease = 7;

	originalX = this->pos.x;

    allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("obj_mega", "g3d/obj_mario.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("obj_mario");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	this->anmPat = this->resFile.GetResAnmTexPat("mega_action");
	this->patAnimation.setup(mdl, anmPat, &this->allocator, 0, 1);
	this->patAnimation.bindEntry(&this->bodyModel, &anmPat, 0, 1);
	this->patAnimation.setFrameForEntry(1, 0);
	this->patAnimation.setUpdateRateForEntry(0.0f, 0);
	this->bodyModel.bindAnim(&this->patAnimation);

	allocator.unlink();
    
	this->scale = (Vec){1.4f, 1.4f, 1.4f};

    ActivePhysics::Info HitMeBaby;
	
	// Dist from center
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 64.0;
	
	// Size
	HitMeBaby.xDistToEdge = 32.0;
	HitMeBaby.yDistToEdge = 50.0;
	
	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFBAFFFE;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();

	wasOnGround = false;
	weJumped = false;
	canBreakThisLanding = false;

	// STOLEN
	
	spriteSomeRectX = 28.0f;
	spriteSomeRectY = 32.0f;
	_320 = 0.0f;
	_324 = 16.0f;

	// "These structs tell stupid collider what to collide with - these are from koopa troopa" - Very well spoken ~
	static const lineSensor_s below(-5<<12, 5<<12, 0<<12);
	static const pointSensor_s above(0<<12, 12<<12);
	static const lineSensor_s adjacent(6<<12, 9<<12, 6<<12);

	collMgr.init(this, &below, &above, &adjacent);
	collMgr.calculateBelowCollisionWithSmokeEffect();
	
	int size = 32; // MegaMario width in tiles

	// BELOW SENSOR
	belowSensor.flags =
		SENSOR_LINE |
		SENSOR_BREAK_BLOCK |
		SENSOR_BREAK_BRICK |
		SENSOR_10000000;

	belowSensor.lineA = -size << 12;
	belowSensor.lineB =  size << 12;
	belowSensor.distanceFromCenter = -15;

	int halfHeight = 50; // match ActivePhysics yDistToEdge

	// SIDE SENSOR
	adjacentSensor.flags =
		SENSOR_LINE |
		SENSOR_BREAK_BLOCK |
		SENSOR_BREAK_BRICK |
		SENSOR_10000000;

	// Vertical line spanning full body height
	adjacentSensor.lineA =  0 << 12;
	adjacentSensor.lineB =  100 << 12;

	// Horizontal offset from center (how far to the side)
	adjacentSensor.distanceFromCenter = 32 << 12; // match xDistToEdge

	// ABOVE SENSOR
	aboveSensor.flags =
		SENSOR_LINE |
		SENSOR_BREAK_BLOCK |
		SENSOR_BREAK_BRICK |
		SENSOR_10000000;
	
	aboveSensor.lineA = -size << 12;
	aboveSensor.lineB =  size << 12;
	//aboveSensor.distanceFromCenter = 50;

	// Register sensors
	collMgr.init(this, &belowSensor, &aboveSensor, &adjacentSensor);

	daPlayer = dAcPy_c::findByID(0); 
	//cmgr_returnValue = collMgr.isOnTopOfTile();
	
	this->onExecute();
	return true;
}

int dMegaMario_c::onDraw()
{
    bodyModel.scheduleForDrawing();
    return true;
}

int dMegaMario_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
	
	if(this->scale.x != 1.5f) return;

	Vec bindPos = this->pos;
	bindPos.y += 50.0f;
	daPlayer->pos = bindPos;
	
	bool onGround = collMgr.isOnTopOfTile();

	if(this->speed.y >= 5.9f)
		this->weJumped = true;

	// Detect landing from a jump
	if (this->weJumped && onGround) {
		canBreakThisLanding = true;
		this->weJumped = false;
	}
	else
		canBreakThisLanding = false;

	wasOnGround = onGround;

	// Enable brick breaking ONLY for the landing frame
	if (canBreakThisLanding) {
		belowSensor.flags |= SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK;
	}
	else {
		belowSensor.flags &= ~(SENSOR_BREAK_BLOCK | SENSOR_BREAK_BRICK);
	}

	if(this->texState == 0)
	{
		this->patAnimation.setFrameForEntry(1, 0);
	}
	else if(this->texState == 1)
	{
		if(this->timer >= speedFrameIncrease)
		{
			if(this->walkTimer < 5) {
				this->patAnimation.setFrameForEntry(this->walkTimer, 0);
				this->walkTimer++;
				this->timer = 0;
			} else {
				this->timer = 0;
				this->patAnimation.setFrameForEntry(3, 0);
				this->walkTimer = 2;
			}

			if(this->speed.x >= 2.0f || this->speed.x <= -2.0f)
				speedFrameIncrease = 3;
			else if(this->speed.x >= 1.0f || this->speed.x <= -1.0f)
				speedFrameIncrease = 5;
			else
				speedFrameIncrease = 7;
		}
		this->timer++;
	}
	else if(this->texState == 3)
		this->patAnimation.setFrameForEntry(5, 0);
	else
		this->patAnimation.setFrameForEntry(0, 0);

	return true;
}

int dMegaMario_c::onDelete()
{
    return true;
}

bool dMegaMario_c::calculateTileCollisions()
{
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
	
	if(collMgr.isOnTopOfTile())
	{
		speed.y = 0.0f;
		max_speed.x = (direction == 1) ? -speed.x : speed.x;
	}
	else
		x_speed_inc = 0.0f;
	
	collMgr.calculateAdjacentCollision(0);
	
	if (collMgr.outputMaybe & (0x15 << direction)) {
		/*if (collMgr.isOnTopOfTile()) {
			isBouncing = true;
		}*/
		return true;
	}
	return false;
}

/* 		Walk 		*/
void dMegaMario_c::beginState_Walk() 
{
	this->timer = 0;

	this->max_speed.x = (direction) ? -0.5f : 0.5f;
	this->speed.x = (direction) ? -0.5f : 0.5f;
	
	this->max_speed.y = -3.0;
	this->speed.y = -3.0;
	this->y_speed_inc = -0.1875;
	
	this->texState = 0;
}

void dMegaMario_c::executeState_Walk() 
{
	bool turn = calculateTileCollisions();
}
void dMegaMario_c::endState_Walk() 
{}

/* 		Spawn Anim 		*/
void dMegaMario_c::beginState_SpawnScale() {
	timer = 0;
	scale = (Vec){0.4f, 0.4f, 0.4f};
	speed.x = speed.y = 0.0f;
	max_speed.x = max_speed.y = 0.0f;
}

void dMegaMario_c::executeState_SpawnScale() {
	timer++;
	this->patAnimation.setFrameForEntry(0, 0);

	Vec bindPos = this->pos;
	bindPos.y += 50.0f;
	daPlayer->pos = bindPos;

	if (timer == 7) {
		scale = (Vec){0.7f, 0.7f, 0.7f};
	}
	else if (timer == 14) {
		scale = (Vec){0.5f, 0.5f, 0.5f};
	}
	else if (timer == 21) {
		scale = (Vec){1.0f, 1.0f, 1.0f};
	}
	else if (timer == 28) {
		scale = (Vec){0.7f, 0.7f, 0.7f};
	}
	else if (timer == 35) {
		scale = (Vec){1.49f, 1.49f, 1.49f};
	}
	else if (timer == 42) {
		scale = (Vec){1.5f, 1.5f, 1.5f};
		doStateChange(&StateID_Walk); // transition to normal behavior
	}
}

void dMegaMario_c::endState_SpawnScale() {
}


////////////////////////////////////////////////////////////////////////////////////
// PLAYER STATES

#define MAX_MEGA_SPEED 1.5f
#define SPD_INCREMENT 0.25f
#define SPD_TURN 0.05f

Vec oldPos;
bool jump;
bool turning;

void daPlBase_c::beginState_MegaMario() {
	this->setFlag(0xBB); // invis
	this->useDemoControl();
	jump = false;
	turning = false;
}
void daPlBase_c::executeState_MegaMario() {
	dMegaMario_c* megaMario = (dMegaMario_c*)FindActorByType(mega, 0);

	if(!megaMario)
		return;

	if(megaMario->scale.x != 1.5f)
		return;

	OSReport("WE FOUND MEGA MARIO");
	
	someFlightRelatedFunction(this); // Handles x movement apparently
	Remocon* con = GetRemoconMng()->controllers[this->settings % 4];
	
	if(con->heldButtons & WPAD_LEFT) {
		if(megaMario->speed.x >= MAX_MEGA_SPEED)
			turning = true;

		if(megaMario->speed.x <= 0)
			turning = false;

		if(megaMario->speed.x <= -MAX_MEGA_SPEED)
		{
			megaMario->speed.x = -MAX_MEGA_SPEED;
			megaMario->max_speed.x = -MAX_MEGA_SPEED;
		}
		else
		{ 
			megaMario->speed.x -= (turning) ? SPD_TURN : SPD_INCREMENT;
			megaMario->max_speed.x -= (turning) ? SPD_TURN : SPD_INCREMENT;
		}

		megaMario->rot.y = 0x8000;
	}

	if(con->heldButtons & WPAD_RIGHT) {
		if(megaMario->speed.x <= -MAX_MEGA_SPEED)
			turning = true;

		if(megaMario->speed.x >= 0)
			turning = false;

		if(megaMario->speed.x >= MAX_MEGA_SPEED)
		{
			megaMario->speed.x = MAX_MEGA_SPEED;
			megaMario->max_speed.x = MAX_MEGA_SPEED;
		}
		else
		{ 
			megaMario->speed.x += (turning) ? SPD_TURN : SPD_INCREMENT;
			megaMario->max_speed.x = (turning) ? SPD_TURN : SPD_INCREMENT;
		}

		megaMario->rot.y = 0;
	}

	if(con->nowPressed & WPAD_TWO && megaMario->collMgr.isOnTopOfTile())
	{
		megaMario->speed.y = 6.0f;
		megaMario->max_speed.y = 6.0f;
		megaMario->texState = 0; // set frame to jump
	}

	if(!megaMario->collMgr.isOnTopOfTile())
	{
		megaMario->speed.y -= 0.25;
		megaMario->max_speed.y -= 0.25;
	}

	if (!(con->heldButtons & (WPAD_LEFT | WPAD_RIGHT))) {
		if (megaMario->speed.x > 0) {
			megaMario->speed.x -= SPD_INCREMENT;
			if (megaMario->speed.x < 0)
				megaMario->speed.x = 0;
		}
		else if (megaMario->speed.x < 0) {
			megaMario->speed.x += SPD_INCREMENT;
			if (megaMario->speed.x > 0)
				megaMario->speed.x = 0;
		}
	}

	megaMario->HandleXSpeed();
	megaMario->HandleYSpeed();
	megaMario->doSpriteMovement();

	// TEXTURE ANIMS FOR MEGA MARIO SPRITE
	if(megaMario->weJumped)
		megaMario->texState = 0;
	else if(turning)
		megaMario->texState = 3;
	else if(megaMario->speed.x != 0)
		megaMario->texState = 1;
	else
		megaMario->texState = 2;
}
void daPlBase_c::endState_MegaMario() {
}
