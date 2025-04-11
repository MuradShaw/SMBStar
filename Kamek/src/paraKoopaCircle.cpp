#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"

const char* PKCArcList[] = { "kuribo", NULL};

class dParaKoopaCircle_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
	
	/*nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;
	
	nw4r::g3d::ResAnmTexPat anmPat;
	m3d::anmTexPat_c patAnimation;*/
	
	int centerX;
	int centerY;
	int radius;
	int numberOfKoopas;
	
	public: static dActor_c *build();
	
	USING_STATES(dParaKoopaCircle_c);
	DECLARE_STATE(Turn);
};

CREATE_STATE_E(dParaKoopaCircle_c, Turn);

const SpriteData ParaKoopaCircleData = {ProfileId::paraKoopaCircle, 8, -8, 0, 0, 0x100, 0x100, 0x40, 0x40, 0, 0, 0};
Profile ParaKoopaProfile(&dParaKoopaCircle_c::build, SpriteId::paraKoopaCircle, &ParaKoopaCircleData, ProfileId::paraKoopaCircle, ProfileId::paraKoopaCircle, "paraKoopaCircle", PKCArcList);

dActor_c* dParaKoopaCircle_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(dParaKoopaCircle_c));
	return new(buf) dParaKoopaCircle_c;
}

int dParaKoopaCircle_c::onCreate()
{
	this->radius = 5;
	this->numberOfKoopas = 3;
	
	this->centerX = this->pos.x;
	this->centerY = this->pos.y;
	
	// Let's get some koopas
	for(int i = 0; i < this->numberOfKoopas; i++)
	{
		
	}
	
	this->onExecute();
	return true;
}

int dParaKoopaCircle_c::onDraw() {
	return true;
}

int dParaKoopaCircle_c::onExecute() {
	acState.execute();
	return true;
}

int dParaKoopaCircle_c::onDelete() {
	return true;
}
