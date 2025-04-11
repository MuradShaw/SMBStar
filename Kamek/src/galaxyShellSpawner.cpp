#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <profile.h>

const char* GalaxyShellSpawnerArcList[] = {"kuribo", NULL};

class daGalaxyShellSpawner_c : public dEn_c {
	int onCreate();
	int onExecute();
	int onDelete();
	int onDraw();
	
	int timer;
	Vec originalPosition;
	Vec previousPosition;
	u32 shellOnlySettings;
	dStageActor_c* daShell;
	u32 shellID;
	
	public: static dActor_c *build();
	
	bool matchingPositions(Vec pos1, Vec pos2);
	
	USING_STATES(daGalaxyShellSpawner_c);
	DECLARE_STATE(Wait);
	DECLARE_STATE(Respawn);
};

CREATE_STATE(daGalaxyShellSpawner_c, Wait);
CREATE_STATE(daGalaxyShellSpawner_c, Respawn);

const SpriteData GalaxyShellSpawnerData = {ProfileId::galaxyShellSpawner, 8, -0x10, 0, 0, 0x100, 0x100, 0, 0, 0, 0, 0};
Profile GalaxyShellSpawnerProfile(&daGalaxyShellSpawner_c::build, SpriteId::galaxyShellSpawner, &GalaxyShellSpawnerData, ProfileId::galaxyShellSpawner, ProfileId::galaxyShellSpawner, "galaxyShellSpawner", GalaxyShellSpawnerArcList);

dActor_c* daGalaxyShellSpawner_c::build() {
	void *buf = AllocFromGameHeap1(sizeof(daGalaxyShellSpawner_c));
	return new(buf) daGalaxyShellSpawner_c;
}

int daGalaxyShellSpawner_c::onCreate()
{
	shellOnlySettings = (0 | (0 << 2) | (1 << 4) | (0 << 6) | (0 << 8) | (0 << 10) | (0 << 12) | (0 << 14));
	daShell = (dStageActor_c*)CreateActor(54, shellOnlySettings, this->pos, 0, 0);
	shellID = daShell->id;
	
	this->originalPosition = (Vec){ this->pos.x, this->pos.y, this->pos.z };
	
	doStateChange(&StateID_Wait);
	
	this->onExecute();
	return true;
}

int daGalaxyShellSpawner_c::onDraw() {
	return true;
}

int daGalaxyShellSpawner_c::onExecute() {
	dStageActor_c *ac = (dStageActor_c*)fBase_c::search(shellID);

	if (!ac) {
		daShell = (dStageActor_c*)CreateActor(54, shellOnlySettings, this->pos, 0, 0);
		shellID = daShell->id;
	}
			
	acState.execute();
	return true;
}

int daGalaxyShellSpawner_c::onDelete() {
	return true;
}

/*	Wait		*/
void daGalaxyShellSpawner_c::beginState_Wait() 
{
	this->timer = 0;
}
void daGalaxyShellSpawner_c::executeState_Wait() 
{
	if(this->timer > 180)
	{
		daShell->pos.x = this->pos.x;
		daShell->pos.y = this->pos.y;
		daShell->pos.z = this->pos.z;
		this->timer = 0;
	}
	
	// We're far from home
	if(!matchingPositions(daShell->pos, pos))
	{
		// We've been sitting here
		if(matchingPositions(daShell->pos, this->previousPosition))
			this->timer++;
		else
			this->timer = 0;
	}
	
	this->previousPosition = (Vec){ daShell->pos.x, daShell->pos.y, daShell->pos.z };
}
void daGalaxyShellSpawner_c::endState_Wait() 
{}

/*	Respawn		*/
void daGalaxyShellSpawner_c::beginState_Respawn() 
{
	this->timer = 0;
	daShell->Delete(1);
}
void daGalaxyShellSpawner_c::executeState_Respawn() 
{
	daShell = (dStageActor_c*)CreateActor(54, shellOnlySettings, this->pos, 0, 0);
	doStateChange(&StateID_Wait);
}
void daGalaxyShellSpawner_c::endState_Respawn() 
{}

bool daGalaxyShellSpawner_c::matchingPositions(Vec pos1, Vec pos2)
{
	return ((pos1.x == pos2.x) && (pos1.y == pos2.y) && (pos1.z == pos2.z));
}