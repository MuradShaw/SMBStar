#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include <dCourse.h>

const char *ThwompFileList[] = {"dossun_neo", 0};

class dThwomp3DW_c : public dEn_c {
	public:
		Physics::Info physicsInfo;

		int onCreate();
		int onDelete();
		int onExecute();
		int onDraw();
			
		bool doTexAnim;
		int timer;
		int texTimer;
		u32 cmgr_returnValue;
		float originalPosY;
		float originalPosX;
		u8 moveDirection;
		int fallDirection;

		mHeapAllocator_c allocator;
		nw4r::g3d::ResFile resFile;
		nw4r::g3d::ResFile anmFile;
		m3d::mdl_c model;
		m3d::anmChr_c chrAnimation;
		
		nw4r::g3d::ResAnmTexPat anmPat;
		m3d::anmTexPat_c patAnimation;
		
		Physics physics;
		
		void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
		void texPat_bindAnimChr_and_setUpdateRate(const char* name);
		float nearestPlayerDistance();

		USING_STATES(dThwomp3DW_c);
		DECLARE_STATE(Wait);
		DECLARE_STATE(Shake);
		DECLARE_STATE(Move);
		DECLARE_STATE(MoveHorizontal);
		DECLARE_STATE(Rise);
		DECLARE_STATE(Retract);

		static dActor_c *build();
};


CREATE_STATE(dThwomp3DW_c, Wait);
CREATE_STATE_E(dThwomp3DW_c, Shake);
CREATE_STATE(dThwomp3DW_c, Move);
CREATE_STATE(dThwomp3DW_c, MoveHorizontal);
CREATE_STATE(dThwomp3DW_c, Rise);
CREATE_STATE(dThwomp3DW_c, Retract);

const SpriteData ThwompData = {ProfileId::thwomp3DW, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile ThwompProfile(&dThwomp3DW_c::build, SpriteId::thwomp3DW, &ThwompData, ProfileId::thwomp3DW, ProfileId::thwomp3DW, "thwomp3DW", ThwompFileList);

extern "C" void HurtMarioBecauseOfBeingSquashed(void *mario, dStageActor_c *squasher, int type);
extern "C" int posIsInZone(Vec,float*,float*,u8 zone);
extern "C" void* ScreenPositionClass;
extern "C" int SpawnThwompEffects(dEn_c *);

extern "C" void* SoundRelatedClass;
extern "C" Vec ConvertStagePositionIntoScreenPosition__Maybe(Vec);
extern "C" void AnotherSoundRelatedFunction(void*,SFX,Vec,int);
	
dActor_c  *dThwomp3DW_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dThwomp3DW_c));
	return new(buf) dThwomp3DW_c;
}

static void thwompHugePhysCB1(dThwomp3DW_c *one, dStageActor_c *two) {
	
	if (two->stageActorType != 1)
		return;

	// if left/right
	//if (one->moveDirection <= 1)
	//	return;

	if (one->pos_delta.y > 0.0f)
		HurtMarioBecauseOfBeingSquashed(two, one, 1);
	else
		HurtMarioBecauseOfBeingSquashed(two, one, 9);
}

static void thwompHugePhysCB2(dThwomp3DW_c *one, dStageActor_c *two) {	
	if (two->stageActorType != 1)
		return;

	// if left/right
	//if (one->moveDirection <= 1)
	//	return;

	if (one->pos_delta.y < 1.0f)
		HurtMarioBecauseOfBeingSquashed(two, one, 2);
	else
		HurtMarioBecauseOfBeingSquashed(two, one, 10);
}

static void thwompHugePhysCB3(dThwomp3DW_c *one, dStageActor_c *two, bool unkMaybeNotBool) {

	if (two->stageActorType != 1)
		return;

	// if up/down
	//if (one->moveDirection > 1)
	//	return;

	if (unkMaybeNotBool) {
		if (one->pos_delta.x > 0.0f)
			HurtMarioBecauseOfBeingSquashed(two, one, 6);
		else
			HurtMarioBecauseOfBeingSquashed(two, one, 12);
	} else {
		if (one->pos_delta.x < 0.0f)
			HurtMarioBecauseOfBeingSquashed(two, one, 5);
		else
			HurtMarioBecauseOfBeingSquashed(two, one, 11);
	}
}

static bool thwompHugePhysCB4(dThwomp3DW_c *one, dStageActor_c *two) {
	return (one->pos_delta.y > 0.0f);
}

static bool thwompHugePhysCB5(dThwomp3DW_c *one, dStageActor_c *two) {
	return (one->pos_delta.y < 0.0f);
}

static bool thwompHugePhysCB6(dThwomp3DW_c *one, dStageActor_c *two, bool unkMaybeNotBool) {
	if (unkMaybeNotBool) {
		if (one->pos_delta.x > 0.0f)
			return true;
	} else {
		if (one->pos_delta.x < 0.0f)
			return true;
	}
	return false;
}

int dThwomp3DW_c::onCreate() 
{
	allocator.link(-1, GameHeaps[0], 0, 0x20);

	this->resFile.data = getResource("dossun_neo", "g3d/dossun_neo.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("dossun_neo");
	model.setup(mdl, &allocator, 0x108, 1, 0);
	
	SetupTextures_Enemy(&model, 0);
	
	this->anmFile.data = getResource("dossun_neo", "g3d/dossun_neo.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("shake");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	this->anmPat = this->resFile.GetResAnmTexPat("down_st");
	this->patAnimation.setup(mdl, anmPat, &this->allocator, 0, 1);
	this->patAnimation.bindEntry(&this->model, &anmPat, 0, 1);
	this->patAnimation.setFrameForEntry(0, 0);
	this->patAnimation.setUpdateRateForEntry(0.0f, 0);
	this->model.bindAnim(&this->patAnimation);

	allocator.unlink();

	// Solid collision
	physicsInfo.x1 = -48;
	physicsInfo.y1 = 106;
	physicsInfo.x2 = 48;
	physicsInfo.y2 = 0;

	physicsInfo.otherCallback1 = &thwompHugePhysCB1;
	physicsInfo.otherCallback2 = &thwompHugePhysCB2;
	physicsInfo.otherCallback3 = &thwompHugePhysCB3;
	
	physics.setup(this, &physicsInfo, 3, currentLayerID);
	physics.flagsMaybe = 0x260;
	physics.callback1 = &thwompHugePhysCB4;
	physics.callback2 = &thwompHugePhysCB5;
	physics.callback3 = &thwompHugePhysCB6;
	physics.addToList();
	
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

	cmgr_returnValue = collMgr.isOnTopOfTile();
	
	// We need these for later
	this->originalPosX = this->pos.x;
	this->originalPosY = this->pos.y;
	
	this->fallDirection = (settings & 0x3000) >> 12; // 0: down, 1: right, 2: left
	doStateChange(&StateID_Wait);

	return true;
}


int dThwomp3DW_c::onDelete() {
	physics.removeFromList();

	return true;
}


int dThwomp3DW_c::onExecute() {
	model._vf1C();
	acState.execute();
	physics.update();
	
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	this->model.setDrawMatrix(matrix);
	this->model.setScale(&scale);
	this->model.calcWorld(false);

	return true;
}


int dThwomp3DW_c::onDraw() {
	
	this->model.scheduleForDrawing();

	return true;
}

float dThwomp3DW_c::nearestPlayerDistance() {
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

void dThwomp3DW_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->model, anmChr, unk);
	this->model.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

void dThwomp3DW_c::texPat_bindAnimChr_and_setUpdateRate(const char* name) {
	this->anmPat = this->resFile.GetResAnmTexPat(name);
	this->patAnimation.bindEntry(&this->model, &anmPat, 0, 1);
	this->model.bindAnim(&this->patAnimation);
}

/*		Wait		*/
void dThwomp3DW_c::beginState_Wait() 
{
	this->timer = 0;
}
void dThwomp3DW_c::executeState_Wait() 
{
	if(this->timer >= 30)
	{
		if(this->nearestPlayerDistance() <= 90.0f)
		{
			if(this->fallDirection == 0)
				texPat_bindAnimChr_and_setUpdateRate("down_st");
			else if(this->fallDirection == 1)
				texPat_bindAnimChr_and_setUpdateRate("right_st");
			else
				texPat_bindAnimChr_and_setUpdateRate("left_st");
					
			bindAnimChr_and_setUpdateRate("shake", 1, 0.0, 1.0); 
			doStateChange(&StateID_Shake);
		}
	}
	
	this->timer += 1;
}
void dThwomp3DW_c::endState_Wait() 
{
	this->timer = 0;
}

/*		Shake		*/
void dThwomp3DW_c::executeState_Shake() 
{
	if(this->timer <= 61)
		this->patAnimation.setFrameForEntry(this->timer, 0);
	
	if(this->timer >= 61) {
		if(this->fallDirection == 0)
			doStateChange(&StateID_Move);
		else
			doStateChange(&StateID_MoveHorizontal); }
	
	this->timer += 1;
}

/*		Move		*/
void dThwomp3DW_c::beginState_Move() 
{
	this->timer = 0;
	
	this->speed.y = -9.0f;
	this->max_speed.y = -13.0f;
	this->y_speed_inc = -0.25;
	
	this->speed.x = 0.0f;
	this->moveDirection = 3;
	
	//texPat_bindAnimChr_and_setUpdateRate("down_end");
}
void dThwomp3DW_c::executeState_Move() 
{
	cmgr_returnValue = collMgr.isOnTopOfTile();
	collMgr.calculateBelowCollisionWithSmokeEffect();
	
	if(collMgr.isOnTopOfTile())
	{
		this->speed.y = 0.0f;
		this->timer++;
		
		if(this->timer < 3)
		{
			SpawnThwompEffects(this);
			Vec p = ConvertStagePositionIntoScreenPosition__Maybe(this->pos);
			AnotherSoundRelatedFunction(SoundRelatedClass, SE_EMY_BIG_DOSSUN, p, 0);

			ShakeScreen(ScreenPositionClass, 0, 1, 0, 0);
			PlaySoundAsync(this, SE_EMY_BIG_DOSSUN);
		}
	}
	
	if(this->timer == 60) texPat_bindAnimChr_and_setUpdateRate("down_end");
	if(this->timer > 60 && this->timer < 66) this->patAnimation.setFrameForEntry((this->timer - 60), 0);
	
	if(this->timer >= 120)
		doStateChange(&StateID_Rise);
			
	//HandleYSpeed();
	HandleXSpeed();
	doSpriteMovement();
}
void dThwomp3DW_c::endState_Move() {
}

/*	MoveHorizontal  */
void dThwomp3DW_c::beginState_MoveHorizontal() 
{
	this->timer = 0;
	this->speed.y = 0.0f;
	
	if(this->fallDirection == 1) this->speed.x = 9.0f; else this->speed.x = -9.0f;
	if(this->fallDirection == 1) this->max_speed.x = 13.0; else this->max_speed.x = -13.0;
	if(this->fallDirection == 1) this->direction = 0; else this->direction = 1;
	
	this->x_speed_inc = 0.25;
	
	//this->moveDirection = 3;
	//texPat_bindAnimChr_and_setUpdateRate("down_end");
}
void dThwomp3DW_c::executeState_MoveHorizontal() 
{
	collMgr.calculateAdjacentCollision(0);
	
	if (collMgr.outputMaybe & (0x15 << direction))
	{
		this->speed.x = 0.0f;
		this->timer++;
		
		if(this->timer < 3)
		{
			//SpawnThwompEffects(this);
			Vec p = ConvertStagePositionIntoScreenPosition__Maybe(this->pos);
			AnotherSoundRelatedFunction(SoundRelatedClass, SE_EMY_BIG_DOSSUN, p, 0);

			ShakeScreen(ScreenPositionClass, 0, 1, 0, 0);
			PlaySoundAsync(this, SE_EMY_BIG_DOSSUN);
		}
	}
	
	if(this->timer == 60) 
	{
		if(this->fallDirection == 1) 
			texPat_bindAnimChr_and_setUpdateRate("right_end"); 
		else 
			texPat_bindAnimChr_and_setUpdateRate("left_end");
	}
		
	if(this->timer > 60 && this->timer < 66) this->patAnimation.setFrameForEntry((this->timer - 60), 0);
	
	if(this->timer >= 120)
		doStateChange(&StateID_Retract);
			
	//HandleYSpeed();
	HandleXSpeed();
	doSpriteMovement();
}
void dThwomp3DW_c::endState_MoveHorizontal() {
}

/*		Rise		*/
void dThwomp3DW_c::beginState_Rise()
{
	this->timer = 0;
	this->moveDirection = 2;
	this->max_speed.x = 1.0f;
	//this->patAnimation.setFrameForEntry(this->timer, 0);
}
void dThwomp3DW_c::executeState_Rise() 
{
	if(this->pos.y >= this->originalPosY)
	{
		this->speed.y = 0.0f;
		this->speed.x = 0.0f;
		doStateChange(&StateID_Wait);
	}
	else
		this->speed.y = 1.0f;
	
	this->timer++;
	
	HandleYSpeed();
	HandleXSpeed();
	doSpriteMovement();
}
void dThwomp3DW_c::endState_Rise() {
}

/*		Retract		*/
void dThwomp3DW_c::beginState_Retract() 
{
	this->timer = 0;
	//this->moveDirection = 2;
	
	if(this->fallDirection == 2)
	{
		this->direction = 1;
		this->max_speed.x = -0.2f;
	}
	else
	{
		this->direction = 0;
		this->max_speed.x = 0.2f;
	}
	
	//this->patAnimation.setFrameForEntry(this->timer, 0);
}
void dThwomp3DW_c::executeState_Retract() 
{
	bool condition = (this->fallDirection == 1) ? (this->pos.x <= this->originalPosX) : (this->pos.x >= this->originalPosX);
	
	if(condition)
	{
		this->speed.y = 0.0f;
		this->speed.x = 0.0f;
		doStateChange(&StateID_Wait);
	}
	else
	{
		if(this->fallDirection == 1)
			this->speed.x = -1.0f;
		else
			this->speed.x = 1.0f;
	}
	
	this->timer++;
	
	HandleXSpeed();
	doSpriteMovement();
}
void dThwomp3DW_c::endState_Retract() {
}