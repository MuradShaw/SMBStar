#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* unagiFileList[] = {"unagi", NULL};

class daUnagi_c : public dEn_c {
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
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	 
	bool loadedIn;
	
	USING_STATES(daUnagi_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Strike);
	//DECLARE_STATE(Return);
	DECLARE_STATE(Swim);
	//DECLARE_STATE(Die); 
	
	public: float nearestPlayerDistance();
	public: float nearestPlayerDistanceY();
	
	public: static dActor_c *build();
};

CREATE_STATE(daUnagi_c, Wait);
CREATE_STATE(daUnagi_c, Strike);
//CREATE_STATE(daUnagi_c, Return);

CREATE_STATE(daUnagi_c, Swim);
//CREATE_STATE(daUnagi_c, Die);

const SpriteData UnagiSpriteData = {ProfileId::unagi, 0, 0 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile UnagiProfile(&daUnagi_c::build, SpriteId::unagi, &UnagiSpriteData, ProfileId::unagi, ProfileId::unagi, "Unagi", unagiFileList);

dActor_c* daUnagi_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daUnagi_c));
	return new(buf) daUnagi_c;
}

void daUnagi_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
}

float daUnagi_c::nearestPlayerDistance() {
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

float daUnagi_c::nearestPlayerDistanceY() {
	float bestSoFar = 10000.0f;

	for (int i = 0; i < 4; i++) {
		if (dAcPy_c *player = dAcPy_c::findByID(i)) {
			if (strcmp(player->states2.getCurrentState()->getName(), "dAcPy_c::StateID_Balloon")) {
				float thisDist = player->pos.y - pos.y;
				if (thisDist < bestSoFar)
					bestSoFar = thisDist;
			}
		}
	}

	return bestSoFar;
}

void daUnagi_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daUnagi_c::onCreate()
{
	this->type = this->settings >> 28 & 0xF;
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("unagi", "g3d/unagi.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("unagi_model");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_Enemy(&bodyModel, 0);

	this->anmFile.data = getResource("unagi", "g3d/unagi.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("wait");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	this->scale = (Vec){1.0, 1.0, 1.0};

	this->rot.x = 0;
	this->rot.y = (type) ? 0xC400 : 0;
	this->rot.z = 0;
	
	this->direction = 1; // Keeping this here because I might make this a setting
	
	ActivePhysics::Info HitMeBaby;	
	if(type)
	{
		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 1.0;

		HitMeBaby.xDistToEdge = 80.0;
		HitMeBaby.yDistToEdge = 5.0;
	}
	else
	{
		// This is going to be pain
		HitMeBaby.xDistToCenter = 0.0;
		HitMeBaby.yDistToCenter = 0.0;

		HitMeBaby.xDistToEdge = 7.0;
		HitMeBaby.yDistToEdge = 7.0;
	}

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffba7ffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	if(!type)
	{
		bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1);
		doStateChange(&StateID_Wait);
	}
	else
	{
		bindAnimChr_and_setUpdateRate("swim", 1, 0.0, 1);
		doStateChange(&StateID_Swim);
	}
	
	this->onExecute();
	return true;
}

int daUnagi_c::onDelete() {
	return true;
}

int daUnagi_c::onExecute() {
	float dist = nearestPlayerDistance();
	if(dist > 0x200)
		loadedIn = false;
	else
		loadedIn = true;
	
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
			
	return true;
}

int daUnagi_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daUnagi_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

/*		Wait		*/

void daUnagi_c::beginState_Wait()
{
	this->timer = 0;
}
void daUnagi_c::executeState_Wait()
{	
	//Minimum reps met
	if(this->timer >= 1)
	{
		float distance = this->nearestPlayerDistance();
		float distanceY = this->nearestPlayerDistanceY();
		
		if(((abs(distance)) <= 80) && ((abs(distanceY)) <= 80))
		{
			if(distance > 0)
				bindAnimChr_and_setUpdateRate("bite_r", 1, 0.0, 1);
			else
				bindAnimChr_and_setUpdateRate("bite_l", 1, 0.0, 1);
			
			doStateChange(&StateID_Strike);
		}
	}
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->timer++;
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void daUnagi_c::endState_Wait()
{}

/*		Strike		*/

void daUnagi_c::beginState_Strike()
{}
void daUnagi_c::executeState_Strike()
{
	if(this->chrAnimation.isAnimationDone())
	{
		bindAnimChr_and_setUpdateRate("wait", 1, 0.0, 1);
		doStateChange(&StateID_Wait);
	}
}
void daUnagi_c::endState_Strike()
{}

/*		Non-wall Unagi's or "Free Thinkers" starting now	*/

/*		Swim		*/

void daUnagi_c::beginState_Swim()
{
	this->max_speed.x = (this->direction) ? -0.6f : 0.6f;
	this->speed.x = (direction) ? -0.6f : 0.6f;
}
void daUnagi_c::executeState_Swim()
{
	HandleXSpeed();
	doSpriteMovement();
	
	if(this->chrAnimation.isAnimationDone())
	{
		this->chrAnimation.setCurrentFrame(0.0);
	}
}
void daUnagi_c::endState_Swim()
{}