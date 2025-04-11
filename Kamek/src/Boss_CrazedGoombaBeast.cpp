#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

#include "Boss Mode/bossMeter.h"

const char* daCrazedGoombaArchive[] = {"kuribo", NULL};

class crazedGoombaBeast_c_b : public dEn_c 
{
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
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	
	USING_STATES(crazedGoombaBeast_c_b);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Walk);
	DECLARE_STATE(Stomp);
	DECLARE_STATE(Die);
	
	bool willWalkOntoSuitableGround();
	
	public: static dActor_c *build();
}

CREATE_STATE(crazedGoombaBeast_c_b, Wait);
CREATE_STATE(crazedGoombaBeast_c_b, Walk);
CREATE_STATE(crazedGoombaBeast_c_b, Stomp);
DECLARE_STATE(crazedGoombaBeast_c_b, Die);

const SpriteData crazedIndividual = {ProfileId::CrazedGoombaBeast, 0, 0 , 0 , 0, 4, 4, 0, 0, 0, 0, 0 };
Profile CrazedProfile(&crazedGoombaBeast_c_b::build, SpriteId::CrazedGoombaBeast, &crazedIndividual, ProfileId::CrazedGoombaBeast, ProfileId::CrazedGoombaBeast, "CrazedGoombaBeast", crazedCrazedMan);

dActor_c* crazedGoombaBeast_c_b::build() {
	void *buf = AllocFromGameHeap1(sizeof(crazedGoombaBeast_c_b));
	return new(buf) crazedGoombaBeast_c_b;
}

void crazedGoombaBeast_c_b::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	char hitType = 0;
	hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 2);
	
	if(hitType > 0)
	{
		doStateChange(&StateID_Stomp);
	}
	else
	{
		this->dEn_c::playerCollision(apThis, apOther);
		this->_vf220(apOther->owner);
	}
}

void crazedGoombaBeast_c_b::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int crazedGoombaBeast_c_b::onCreate()
{

}

int crazedGoombaBeast_c_b::onDelete() {
	return true;
}

int crazedGoombaBeast_c_b::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();

	return true;
}
int crazedGoombaBeast_c_b::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void crazedGoombaBeast_c_b::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

bool crazedGoombaBeast_c_b::calculateTileCollisions() {
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