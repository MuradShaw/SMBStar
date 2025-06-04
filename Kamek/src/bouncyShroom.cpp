#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>
#include "boss.h"

extern dStateBase_c StateID_Walk__Goomba;

const char* BSarcList[] = {
	"lift_kinoko_bounce",
	NULL
};

class daBouncyShroom_c : public dEn_c {
	public:
		static dActor_c* build();
		
		int onCreate();
		int onExecute();
		int onDelete();
		int onDraw();
		
		mHeapAllocator_c allocator;
		
		nw4r::g3d::ResFile resFile;
		nw4r::g3d::ResFile anmFile;
		m3d::mdl_c bodyModel;
		m3d::anmChr_c chrAnimation;
		
		void updateModelMatrices();
		void bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate);
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
		
		void playerCollision(ActivePhysics *apThis, ActivePhysics *apOther);
		void spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther);
		
		bool collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther);
		bool collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther);
		bool collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther); 
		
		int timer;
		int type;
		dStageActor_c* enemy;
		dStageActor_c* previousEnemy;
		
		USING_STATES(daBouncyShroom_c);
		DECLARE_STATE(Wait);
		DECLARE_STATE(BounceEnemy);
};

CREATE_STATE(daBouncyShroom_c, Wait);
CREATE_STATE(daBouncyShroom_c, BounceEnemy);

const SpriteData BouncyMushroomData = {ProfileId::bouncyshroom, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile BouncyMushroomProfile(&daBouncyShroom_c::build, SpriteId::bouncyshroom, &BouncyMushroomData, ProfileId::bouncyshroom, ProfileId::bouncyshroom, "bouncyshroom", BSarcList);

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

void daBouncyShroom_c::addScoreWhenHit(void *other) { }
void daBouncyShroom_c::spawnHitEffectAtPosition(Vec2 pos) { }
void daBouncyShroom_c::doSomethingWithHardHitAndSoftHitEffects(Vec pos) { }
void daBouncyShroom_c::playEnemyDownSound2() { }
void daBouncyShroom_c::playHpdpSound1() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
void daBouncyShroom_c::playEnemyDownSound1() { }
void daBouncyShroom_c::playEnemyDownComboSound(void *player) { } // AcPy_c/daPlBase_c?
void daBouncyShroom_c::playHpdpSound2() { } // plays PLAYER_SE_EMY/GROUP_BOOT/SE_EMY_DOWN_HPDP_S or _H
void daBouncyShroom_c::_vf260(void *other) { } // AcPy/PlBase? plays the SE_EMY_FUMU_%d sounds based on some value
void daBouncyShroom_c::_vf264(dStageActor_c *other) { } // if other is player or yoshi, do Wm_en_hit and a few other things
void daBouncyShroom_c::_vf268(void *other) { } // AcPy/PlBase? plays the SE_EMY_DOWN_SPIN_%d sounds based on some value

extern "C" void dAcPy_vf3F8(void* player, dEn_c* monster, int t);
	
void daBouncyShroom_c::playerCollision(ActivePhysics *apThis, ActivePhysics *apOther)
{
	char hitType;
	hitType = usedForDeterminingStatePress_or_playerCollision(this, apThis, apOther, 0);
		
	if(hitType != 0) 
	{
		this->counter_504[apOther->owner->which_player] = 0xA;
		
		// Do anims
		bindAnimChr_and_setUpdateRate("bounce", 1, 0.0, 1); 
	}
}

void daBouncyShroom_c::spriteCollision(ActivePhysics *apThis, ActivePhysics *apOther) 
{
	u16 name = ((dEn_c*)apOther->owner)->name;

	/* Ignore all these
	if (name == EN_COIN || name == EN_EATCOIN || name == AC_BLOCK_COIN || name == EN_COIN_JUGEM || name == EN_COIN_ANGLE
		|| name == EN_COIN_JUMP || name == EN_COIN_FLOOR || name == EN_COIN_VOLT || name == EN_COIN_WIND 
		|| name == EN_BLUE_COIN || name == EN_COIN_WATER || name == EN_REDCOIN || name == EN_GREENCOIN
		|| name == EN_JUMPDAI || name == EN_ITEM) 
		{ return; }
	
	// Let's see if this works!
	if(name == EN_KURIBO)
	{*/
		enemy = (dStageActor_c*)apOther->owner;
		enemy->speed.y = 6.0f;
		bindAnimChr_and_setUpdateRate("bounce", 1, 0.0, 1); 
		//enemy->direction = !enemy->direction;
		/*dStateBase_c *ourState;
		ourState = &daBouncyShroom_c::StateID_BounceEnemy;
		((dEn_c*)enemy)->acState.setState(ourState);*/
	//}
}

bool daBouncyShroom_c::collisionCat7_GroundPound(ActivePhysics *apThis, ActivePhysics *apOther)
{
	bindAnimChr_and_setUpdateRate("bounce", 1, 0.0, 1); 
	return BigHanaGroundPound(this, apThis, apOther);
}

void daBouncyShroom_c::bouncePlayerWhenJumpedOn(void *player) {
	bouncePlayer(player, 4.3f);
}

bool daBouncyShroom_c::collisionCat7_GroundPoundYoshi(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCatD_Drill(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCatA_PenguinMario(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBouncyShroom_c::collisionCat1_Fireball_E_Explosion(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCat2_IceBall_15_YoshiIce(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCat9_RollingObject(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCat13_Hammer(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCat14_YoshiFire(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}

bool daBouncyShroom_c::collisionCat3_StarPower(ActivePhysics* apThis, ActivePhysics* apOther) {
	return false;
}
bool daBouncyShroom_c::collisionCat5_Mario(ActivePhysics *apThis, ActivePhysics *apOther) {
	playerCollision(apThis, apOther);
	return true;
}

dActor_c  *daBouncyShroom_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daBouncyShroom_c));
	return new(buf) daBouncyShroom_c;
}

void daBouncyShroom_c::updateModelMatrices() {
	matrix.translation(pos.x, pos.y, pos.z);
	matrix.applyRotationYXZ(&rot.x, &rot.y, &rot.z);

	bodyModel.setDrawMatrix(matrix);
	bodyModel.setScale(&scale);
	bodyModel.calcWorld(false);
}

void daBouncyShroom_c::bindAnimChr_and_setUpdateRate(const char* name, int unk, float unk2, float rate) {
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr(name);
	this->chrAnimation.bind(&this->bodyModel, anmChr, unk);
	this->bodyModel.bindAnim(&this->chrAnimation, unk2);
	this->chrAnimation.setUpdateRate(rate);
}

int daBouncyShroom_c::onCreate()
{
	this->type = this->settings >> 28 & 0xF;
	
	allocator.link(-1, GameHeaps[0], 0, 0x20);
	
	if(type == 0)
		this->resFile.data = getResource("lift_kinoko_bounce", "g3d/lift_kinoko_bounce_s.brres");
	else
		this->resFile.data = getResource("lift_kinoko_bounce", "g3d/lift_kinoko_bounce_l.brres");
	
	nw4r::g3d::ResMdl mdl = this->resFile.GetResMdl("lift_kinoko_bounce");
	bodyModel.setup(mdl, &allocator, 0x108, 1, 0);

	SetupTextures_MapObj(&bodyModel, 0);
	
	if(type == 0)
		this->anmFile.data = getResource("lift_kinoko_bounce", "g3d/lift_kinoko_bounce_s.brres");
	else
		this->anmFile.data = getResource("lift_kinoko_bounce", "g3d/lift_kinoko_bounce_l.brres");
	
	nw4r::g3d::ResAnmChr anmChr = this->anmFile.GetResAnmChr("bounce");
	this->chrAnimation.setup(mdl, anmChr, &this->allocator, 0);
	
	allocator.unlink();
	
	// Dist from center
	ActivePhysics::Info HitMeBaby;
	HitMeBaby.xDistToCenter = 0.0;
	HitMeBaby.yDistToCenter = 0.0;
	
	// Size
	if(type == 0)
	{
		HitMeBaby.xDistToEdge = 40.0;
		HitMeBaby.yDistToEdge = 10.0;
	}
	else
	{
		HitMeBaby.xDistToEdge = 60.0;
		HitMeBaby.yDistToEdge = 10.0;
	}
	
	HitMeBaby.category1 = 0x5;
	HitMeBaby.category2 = 0x0;
	HitMeBaby.bitfield1 = 0x4F;
	HitMeBaby.bitfield2 = 0xFFFFFFFF;
	HitMeBaby.unkShort1C = 0;
	HitMeBaby.callback = &dEn_c::collisionCallback;
	
	this->aPhysics.initWithStruct(this, &HitMeBaby);
	this->aPhysics.addToList();
	
	doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int daBouncyShroom_c::onDraw() {
	bodyModel.scheduleForDrawing();
	return true;
}

int daBouncyShroom_c::onExecute() {
	acState.execute();
	updateModelMatrices();
	bodyModel._vf1C();
		
	return true;
}

int daBouncyShroom_c::onDelete() {
	return true;
}

/*	Wait		*/
void daBouncyShroom_c::beginState_Wait()
{}
void daBouncyShroom_c::executeState_Wait()
{
	if(this->chrAnimation.isAnimationDone())
	{
		this->chrAnimation.setCurrentFrame(0.0);
		bindAnimChr_and_setUpdateRate("bounce", 1, 0.0, 0); 
	}
}
void daBouncyShroom_c::endState_Wait()
{}

/* Bouncing Enemies */
void daBouncyShroom_c::beginState_BounceEnemy()
{
	//enemy->speed.y = 8.0f;
}
void daBouncyShroom_c::executeState_BounceEnemy()
{
	//if(enemy->speed.y > -4.0)
	//{
	//	enemy->speed.y -= 0.1f;
	//}
}
void daBouncyShroom_c::endState_BounceEnemy()
{}