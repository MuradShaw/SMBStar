#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* sushiPlatter[] = {"sushi", NULL};

class sushi_c : public dEn_c {
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
	bool startGoingRight;
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	
	bool loadedIn;
	
	USING_STATES(sushi_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Swim);
	DECLARE_STATE(FastSwim);
	DECLARE_STATE(ChangeStance);
	DECLARE_STATE(Dance);
	//DECLARE_STATE(Die);
	
	public: float nearestPlayerDistance();
	public: static dActor_c *build();
};

CREATE_STATE(sushi_c, Wait);
CREATE_STATE(sushi_c, Swim);
CREATE_STATE(sushi_c, FastSwim);
CREATE_STATE(sushi_c, ChangeStance);
CREATE_STATE(sushi_c, Dance);
//CREATE_STATE(sushi_c, Die);

const SpriteData SushiPlateList = {ProfileId::Sushi, 0, 0 , 0 , 0, 4, 4, 0, 0, 0, 0, 0 };
Profile SushiMenuItems(&sushi_c::build, SpriteId::Sushi, &SushiPlateList, ProfileId::Sushi, ProfileId::Sushi, "Sushi", sushiPlatter);

dActor_c* sushi_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(sushi_c));
	return new(buf) sushi_c;
}

void sushi_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
}

float sushi_c::nearestPlayerDistance() {
	float bestSoFar = 10000.0f;

	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			if (strcmp(player->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon")) {
				float thisDist = player->pos.x - pos.x;
				if (thisDist < bestSoFar)
					bestSoFar = thisDist;
			}
		}
	}

	return bestSoFar;
}

void sushi_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int sushi_c::onCreate()
{
	this->startGoingRight = this->settings >> 28 & 0xF;
	
	this->type = MakeRandomNumber(4); //0: Speeds up after a certain point | 1: Dances
	this->loadedIn = false;
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("sushi", "g3d/sushi.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("sushi_model");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_Enemy(&bodyModel, 0);

	this->anmFile.data = getResource("sushi", "g3d/sushi.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("swim");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	this->scale = (Vec){1.6, 1.6, 1.6};

	this->rot.x = 0; 
	this->rot.y = (startGoingRight) ? -0xC400 : 0xC400;
	this->rot.z = 0;
	this->direction = (startGoingRight) ? 0 : 1;
	
	ActivePhysics::Info HitMeBaby;	
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 2.0;

	HitMeBaby.xDistToEdge = 40.0;
	HitMeBaby.yDistToEdge = 5;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffba7ffe;
	HitMeBaby.unkShort1C = 0x20000;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	bindAnimChr_and_setUpdateRate("swim_charge", 1, 0.0, 1);
	doStateChange(&StateID_Wait);
	
	this->onExecute();

	return true;
}

int sushi_c::onDelete() {
	return true;
}

int sushi_c::onExecute() 
{
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
			
	return true;
}

int sushi_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void sushi_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void sushi_c::beginState_Wait() {} 
void sushi_c::executeState_Wait() 
{
	float dist = nearestPlayerDistance();
	OSReport("Wait State SUSHI GOAT>.. \n");
	
	if(abs(dist) <= 0x180)
	{
		doStateChange(&StateID_Swim);
	}
} 
void sushi_c::endState_Wait() {} 

/*		Swim		*/

void sushi_c::beginState_Swim()
{
	this->timer = 0;
	
	this->max_speed.x = (this->direction) ? -1.3f : 1.3f;
	this->speed.x = (direction) ? -1.3f : 1.3f;
}
void sushi_c::executeState_Swim()
{	
	HandleXSpeed();
	doSpriteMovement();
	
	float dist = nearestPlayerDistance();
	
	if((this->timer >= 2) && (abs(dist) <= 0x120))
	{
		if(type == 0)
		{
			bindAnimChr_and_setUpdateRate("dance", 1, 0.0, 1.3);
			doStateChange(&StateID_Dance);
		}
		else doStateChange(&StateID_ChangeStance);
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->timer++;
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void sushi_c::endState_Swim() {}

/*		Change Stance		*/

void sushi_c::beginState_ChangeStance()
{
	this->timer = 0;
	
	this->max_speed.x = (this->direction) ? -1.0f : 1.0f;
	this->speed.x = (direction) ? -1.0f : 1.0f;
}
void sushi_c::executeState_ChangeStance()
{
	HandleXSpeed();
	doSpriteMovement();
	
	if(this->timer >= 2)
	{
		bindAnimChr_and_setUpdateRate("swim", 1, 0.0, 1.5);
		doStateChange(&StateID_FastSwim);
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->timer++;
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void sushi_c::endState_ChangeStance(){}

/*		Swim Faster		*/

void sushi_c::beginState_FastSwim()
{
	this->max_speed.x = (this->direction) ? -1.6f : 1.6f;
	this->speed.x = (direction) ? -1.6f : 1.6f;
}
void sushi_c::executeState_FastSwim()
{
	HandleXSpeed();
	doSpriteMovement();
	
	if(this->chrAnimation.isAnimationDone())
		this->chrAnimation.setCurrentFrame(0.0);
}
void sushi_c::endState_FastSwim() {}

/*		Dance		*/

void sushi_c::beginState_Dance()
{
	this->timer = 0;
	
	this->max_speed.x = (this->direction) ? -1.3f : -1.3f;
	this->speed.x = (direction) ? -1.3f : -1.3f;
}
void sushi_c::executeState_Dance()
{
	HandleXSpeed();
	doSpriteMovement();
	
	if(this->chrAnimation.isAnimationDone())
		this->chrAnimation.setCurrentFrame(0.0);
}
void sushi_c::endState_Dance() {}