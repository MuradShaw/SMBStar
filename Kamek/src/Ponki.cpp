#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include <actors.h>

const char* PonkiFileList[] = {"Ponki", NULL};

class daPonki_c : dEn_c {
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
	int orientation;
	
	bool sleeping;
	bool vine;
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	
	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	
	//void _vf148();
	//void _vf14C();
	//bool CreateIceActors();
	
	USING_STATES(daPonki_c);
	DECLARE_STATE(Idle);
	DECLARE_STATE(Charge);
	DECLARE_STATE(Shoot);
	DECLARE_STATE(Sleep);
	DECLARE_STATE(Die);
	
	bool loadedIn;
	
	public: float nearestPlayerDistance();
	public: static dActor_c* build();
};

CREATE_STATE(daPonki_c, Idle);
CREATE_STATE(daPonki_c, Charge);
CREATE_STATE(daPonki_c, Shoot);
CREATE_STATE(daPonki_c, Sleep);
CREATE_STATE(daPonki_c, Die);
	
const SpriteData PonkiSpriteData = {ProfileId::Ponki, 8, -8 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile PonkiProfile(&daPonki_c::build, SpriteId::Ponki, &PonkiSpriteData, ProfileId::Ponki, ProfileId::Ponki, "Ponki", PonkiFileList, 0);

dActor_c* daPonki_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daPonki_c));
	return new(buf) daPonki_c;
}

void daPonki_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
}

bool daPonki_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther)
{
	bindAnimChr_and_setUpdateRate("dead", 1, 0.0, 1); 
	doStateChange(&StateID_Die);
	return true;
}
bool daPonki_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther)
{
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}
bool daPonki_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther)
{
	return this->collisionCat1_Fireball_E_Explosion(apThis, apOther);
}

/* These handle the ice crap
void daPonki_c::_vf148() {
	dEn_c::_vf148();
	doStateChange(&StateID_Die);
}
void daPonki_c::_vf14C() {
	dEn_c::_vf14C();
	doStateChange(&StateID_Die);
}

extern "C" void sub_80024C20(void);
extern "C" void __destroy_arr(void*, void(*)(void), int, int);
//extern "C" __destroy_arr(struct DoSomethingCool, void(*)(void), int cnt, int bar);

bool daPonki_c::CreateIceActors() {
	struct DoSomethingCool my_struct = { 0, this->pos, {1.2, 1.5, 1.5}, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0 };
	this->frzMgr.Create_ICEACTORs( (void*)&my_struct, 1 );
	__destroy_arr( (void*)&my_struct, sub_80024C20, 0x3C, 1 );
	chrAnimation.setUpdateRate(0.0f);
	return true;
}*/
	
float daPonki_c::nearestPlayerDistance() {
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

void daPonki_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daPonki_c::onCreate()
{
	orientation = (settings & 0xF0000) >> 16;
	//sleeping = ((settings & 0x2000) != 0);
	//vine = ((settings & 0x1000) != 0);
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("Ponki", "g3d/ponki_fire.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("ponki_model");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_Enemy(&bodyModel, 0);

	this->anmFile.data = getResource("Ponki", "g3d/ponki_fire.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("idle");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	this->scale = (Vec){1.0, 1.0, 1.0};

	this->rot.x = 0;
	this->rot.y = 0;
	
	this->rot.z = 0;
	//if(this->orientation == 1) this->rot.z = 0x7D00;
	//if(this->orientation == 2) this->rot.z = 0x3E80; // Left facing
	//if(this->orientation == 3) this->rot.z = 0xBB80;
	
	
	ActivePhysics::Info ponkiInfo;
	ponkiInfo.xDistToCenter = 0.0f;
	ponkiInfo.yDistToCenter = 6.0f;
	
	ponkiInfo.xDistToEdge = 12.0f;
	ponkiInfo.yDistToEdge = 6.0f;

	ponkiInfo.category1 = 0x3;
	ponkiInfo.category2 = 0x9;
	ponkiInfo.bitfield1 = 0x4F;
	ponkiInfo.bitfield2 = 0xffba7ffe;
	ponkiInfo.unkShort1C = 0;
	
	ponkiInfo.callback = &dEn_c::collisionCallback;

	aPhysics.initWithStruct(this, &ponkiInfo);
	aPhysics.addToList();
	
	OSReport("Ponki Create \n");
	
	if(orientation < 3)
	{
		bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
		doStateChange(&StateID_Idle);
	}
	else
	{
		orientation = 0;
		bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
		doStateChange(&StateID_Sleep);
	}
	
	this->onExecute();
	return true;
}

int daPonki_c::onDelete() {
	return true;
}

int daPonki_c::onExecute() {
	
	float dist = nearestPlayerDistance();
	if(dist > 0x200)
	{
		doStateChange(&StateID_Idle);
		loadedIn = false;
	}
	
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
			
	return true;
}

int daPonki_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daPonki_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

/*	Idle State	*/
void daPonki_c::beginState_Idle() 
{
	this->timer = 0;
}
void daPonki_c::executeState_Idle() 
{ 
	if(this->timer >= 1)
	{
		bindAnimChr_and_setUpdateRate("charge", 1, 0.0, 1); 
		doStateChange(&StateID_Charge);
	}
	
	if(this->chrAnimation.isAnimationDone())
		this->timer = this->timer + 1;
}
void daPonki_c::endState_Idle() 
{}



/*	Charge State	*/
void daPonki_c::beginState_Charge() {}
void daPonki_c::endState_Charge() {}
void daPonki_c::executeState_Charge() 
{ 
	if(this->chrAnimation.isAnimationDone())
	{
		bindAnimChr_and_setUpdateRate("shoot", 1, 0.0, 1); 
		doStateChange(&StateID_Shoot);
	}
}


/*	Sleep State	*/
void daPonki_c::beginState_Sleep() {}
void daPonki_c::endState_Sleep() {}
void daPonki_c::executeState_Sleep() 
{ 
	if(this->chrAnimation.isAnimationDone())
			this->chrAnimation.setCurrentFrame(0.0);
}




/*	Shoot State		*/
void daPonki_c::beginState_Shoot() 
{
	this->timer = 0;
}
void daPonki_c::executeState_Shoot() 
{ 
	if(this->timer == 3)
	{
		float neg = -1.0;
		
		// Upside down - Rightside up
		if(orientation < 2)
		{
			if(orientation == 0) neg = 1.0;
			
			// Outer left
			dStageActor_c *spawner = CreateActor(106, 0, (Vec){this->pos.x, this->pos.y + (5 * neg), this->pos.z}, 0, 0);
			spawner->speed.x = -1.5;
			spawner->speed.y = 2 * neg;
			spawner->pos.z = 5550.0;
			
			// Inner left	
			spawner = CreateActor(106, 0, (Vec){this->pos.x, this->pos.y + (5 * neg), this->pos.z}, 0, 0);
			spawner->speed.x = -0.5;
			spawner->speed.y = 2 * neg;
			spawner->pos.z = 5550.0;
			
			// Inner right
			spawner = CreateActor(106, 0, (Vec){this->pos.x, this->pos.y + (5 * neg), this->pos.z}, 0, 0);
			spawner->speed.x = 0.5;
			spawner->speed.y = 2 * neg;
			spawner->pos.z = 5550.0;
			
			// Outer right
			spawner = CreateActor(106, 0, (Vec){this->pos.x, this->pos.y + (5 * neg), this->pos.z}, 0, 0);
			spawner->speed.x = 1.5;
			spawner->speed.y = 2 * neg;
			spawner->pos.z = 5550.0;
		}
		
		// Left facing - Right facing
		else
		{
			if(orientation == 3) neg = 1.0;
			
			// Outer left
			dStageActor_c *spawner = CreateActor(106, 0, (Vec){this->pos.x + (5 * neg), this->pos.y, this->pos.z}, 0, 0);
			spawner->speed.x = 2 * neg;
			spawner->speed.y = -1.5;
			spawner->pos.z = 5550.0;
			
			// Inner left	
			spawner = CreateActor(106, 0, (Vec){this->pos.x + (5 * neg), this->pos.y, this->pos.z}, 0, 0);
			spawner->speed.x = 2 * neg;
			spawner->speed.y = -0.5;
			spawner->pos.z = 5550.0;
			
			// Inner right
			spawner = CreateActor(106, 0, (Vec){this->pos.x + (5 * neg), this->pos.y, this->pos.z}, 0, 0);
			spawner->speed.x = 2 * neg;
			spawner->speed.y = 0.5;
			spawner->pos.z = 5550.0;
			
			// Outer right
			spawner = CreateActor(106, 0, (Vec){this->pos.x + (5 * neg), this->pos.y, this->pos.z}, 0, 0);
			spawner->speed.x = 2 * neg;
			spawner->speed.y = 1.5;
			spawner->pos.z = 5550.0;
		}
	}
	
	this->timer++;
	
	if(this->chrAnimation.isAnimationDone())
	{
		bindAnimChr_and_setUpdateRate("idle", 1, 0.0, 1); 
		doStateChange(&StateID_Idle);
	}
}
void daPonki_c::endState_Shoot() 
{}



/*	Die State	*/
void daPonki_c::beginState_Die()
{
	this->removeMyActivePhysics();
}
void daPonki_c::executeState_Die()
{
	OSReport("Ponki Death \n");
	
	if(this->chrAnimation.isAnimationDone()) 
	{
		this->kill();
		this->Delete(this->deleteForever);
	}
}
void daPonki_c::endState_Die()
{}