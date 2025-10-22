#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* clapatrapFiles[] = {"obj_jrtrap", NULL};

class daClapatrap_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	mHeapAllocator_c allocator;
    ActivePhysics::Info HitMeBaby;	

	nw4r::g3d::ResFile resFile;
	nw4r::g3d::ResFile anmFile;
	m3d::mdl_c bodyModel;
	m3d::anmChr_c chrAnimation;

	int timer;
	int type;
	
	void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
	void updateModelMatrices();
	
	void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
	void yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther);

	bool collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther);
	// bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther);
	bool collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther);

	bool meetSensorCriteria();
    void updateHitbox();

	bool loadedIn;
	
	USING_STATES(daClapatrap_c);
	DECLARE_STATE(Wait);
    DECLARE_STATE(Rattle);
	DECLARE_STATE(Strike);
	
	public: float nearestPlayerDistance();
	public: float nearestPlayerDistanceY();
	
	public: static dActor_c *build();
};

CREATE_STATE(daClapatrap_c, Wait);
CREATE_STATE(daClapatrap_c, Rattle);
CREATE_STATE(daClapatrap_c, Strike);

const SpriteData MouseTrapSpriteData = {ProfileId::clapatrap, 0, 0 , 0 , 0, 0x100, 0x100, 0, 0, 0, 0, 0 };
Profile MouseTrapProfile(&daClapatrap_c::build, SpriteId::clapatrap, &MouseTrapSpriteData, ProfileId::clapatrap, ProfileId::clapatrap, "clapatrap", clapatrapFiles);

dActor_c* daClapatrap_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daClapatrap_c));
	return new(buf) daClapatrap_c;
}

void daClapatrap_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	this->dEn_c::playerCollision(apThis, apOther);
	this->_vf220(apOther->owner);
}
void daClapatrap_c::yoshiCollision(ActivePhysics *apThis, ActivePhysics *apOther) {
	this->playerCollision(apThis, apOther);
}
bool daClapatrap_c::collisionCatD_Drill(ActivePhysics *apThis, ActivePhysics *apOther) {
	return false;
}
bool daClapatrap_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCat7_GroundPoundYoshi(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCat9_RollingObject(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCatA_PenguinMario(ActivePhysics *apThis, ActivePhysics *apOther){
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther){
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCat11_PipeCannon(ActivePhysics *apThis, ActivePhysics *apOther){
	return this->collisionCatD_Drill(apThis, apOther);
}
bool daClapatrap_c::collisionCat13_Hammer(ActivePhysics *apThis, ActivePhysics *apOther) {
	return this->collisionCatD_Drill(apThis, apOther);
}

bool daClapatrap_c::collisionCat3_StarPower(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daClapatrap_c::collisionCat14_YoshiFire(ActivePhysics *apThis, ActivePhysics *apOther){
	return false;
}

bool daClapatrap_c::collisionCat1_Fireball_E_Explosion(ActivePhysics *apThis, ActivePhysics *apOther) {
	SpawnEffect("Wm_en_explosion", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_en_explosion_smk", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){3.0, 3.0, 3.0});

	PlaySound(this, SE_OBJ_EMY_FIRE_DISAPP);
	
	return false;
}
bool daClapatrap_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics *apThis, ActivePhysics *apOther) { 
	SpawnEffect("Wm_ob_cmnicekira", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});
	SpawnEffect("Wm_ob_icebreakwt", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
	SpawnEffect("Wm_ob_iceattack", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.5, 1.5, 1.5});

	PlaySound(this, SE_OBJ_PNGN_ICE_BREAK);

	return true; 
}

float daClapatrap_c::nearestPlayerDistance() {
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

float daClapatrap_c::nearestPlayerDistanceY() {
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

void daClapatrap_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daClapatrap_c::onCreate()
{
	this->type = this->settings >> 28 & 0xF;
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	this->resFile.data = getResource("obj_jrtrap", "g3d/obj_jrtrap.brres");
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("obj_jrtrap");
	bodyModel.setup(mdl, &allocator, 0x224, 1, 0);

	SetupTextures_Enemy(&bodyModel, 0);

	this->anmFile.data = getResource("obj_jrtrap", "g3d/obj_jrtrap.brres");
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("trap_set");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	this->scale = (Vec){5.0, 5.0, 5.0};

	this->rot.x = 0;
	this->pos.z += 100.0f;

    switch(type)
    {
        case 0:
	        this->rot.z = 0;
            break;
        case 1:
	        this->rot.z = 0x4000;
            break;
        case 2:
	        this->rot.z = 0x8000;
            break;
        case 3:
	        this->rot.z = 0xC000;
            break;
        default:
            this->rot.z = 0;
            break;
    }

	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;

	HitMeBaby.xDistToEdge = 0.0;
	HitMeBaby.yDistToEdge = 0.0;

	HitMeBaby.category1 = 0x3;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x6F;
	HitMeBaby.bitfield2 = 0xffba7ffe;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int daClapatrap_c::onDelete() {
	return true;
}

int daClapatrap_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
			
	return true;
}

int daClapatrap_c::onDraw() {
	bodyModel.scheduleForDrawing();

	return true;
}

void daClapatrap_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

bool daClapatrap_c::meetSensorCriteria()
{
    float distance = this->nearestPlayerDistance();
	float distanceY = this->nearestPlayerDistanceY();

    bool expression;
    
    switch(type)
    {
        case 0:
        case 2:
            expression = (((abs(distance)) <= 35) && ((abs(distanceY)) <= 80));
            break;
        
        case 1:
        case 3:
            expression = (((abs(distance)) <= 80) && ((abs(distanceY)) <= 35));
            break;
    }
    
    return expression;
}

void daClapatrap_c::updateHitbox()
{
    switch(type)
    {
        case 0:
            HitMeBaby.xDistToCenter = 0.0;
            HitMeBaby.yDistToCenter = -60.0;
            break;
        case 1:
            HitMeBaby.xDistToCenter = 60.0;
            HitMeBaby.yDistToCenter = 0.0;
            break;
        case 2:
            HitMeBaby.xDistToCenter = 0.0;
            HitMeBaby.yDistToCenter = 60.0;
            break;
        case 3:
            HitMeBaby.xDistToCenter = -60.0;
            HitMeBaby.yDistToCenter = 0.0;
            break;
    }

    HitMeBaby.xDistToEdge = 15.0;
    HitMeBaby.yDistToEdge = 15.0;
    HitMeBaby.callback = &dEn_c::collisionCallback;

    this->aPhysics.initWithStruct(this, &HitMeBaby);
    this->aPhysics.addToList();
}

/*		Wait		*/

void daClapatrap_c::beginState_Wait()
{
	this->timer = 0;
}
void daClapatrap_c::executeState_Wait()
{	
	//Minimum reps met
	if(this->timer >= 30)
	{
		if(this->meetSensorCriteria())
		{
            bindAnimChr_and_setUpdateRate("trap_shake", 1, 0.0, 1);
			doStateChange(&StateID_Rattle);
		}
	}

    if(this->timer < 30)
	    this->timer++;
}
void daClapatrap_c::endState_Wait()
{}

/*		Rattle		*/

void daClapatrap_c::beginState_Rattle()
{
    this->timer = 0;
}
void daClapatrap_c::executeState_Rattle()
{
	if(this->chrAnimation.isAnimationDone())
	{
        this->timer += 1;  // give a few shakes before the strike

        if(this->timer > 15)
        {
            bindAnimChr_and_setUpdateRate("trap_set", 1, 0.0, 1);
            doStateChange(&StateID_Strike);
        }
	}
}
void daClapatrap_c::endState_Rattle()
{}

/*		Strike		*/

void daClapatrap_c::beginState_Strike()
{}
void daClapatrap_c::executeState_Strike()
{
    if(this->chrAnimation.getCurrentFrame() == 14.0)
	{
        this->updateHitbox();

		//SpawnEffect("Wm_ob_starcoinget", 0, &this->pos, &(S16Vec){0,0,0}, &(Vec){1.0, 1.0, 1.0});
		PlaySound(this, SE_MG_IH_FENCE_HIT);
	}
    else
    {
        HitMeBaby.xDistToCenter = 0.0;
        HitMeBaby.yDistToCenter = 0.0;

        HitMeBaby.xDistToEdge = 0.0;
        HitMeBaby.yDistToEdge = 0.0;
        HitMeBaby.callback = &dEn_c::collisionCallback;

        this->aPhysics.initWithStruct(this, &HitMeBaby);
        this->aPhysics.addToList();
    }

	if(this->chrAnimation.isAnimationDone())
	{
		doStateChange(&StateID_Wait);
	}
}
void daClapatrap_c::endState_Strike()
{}