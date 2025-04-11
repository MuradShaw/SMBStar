#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <newer.h>

#include "fileload.h"
#include "layoutlib.h"

class dLevelSelection_c : public dScene_c {
	public:
		dLevelSelection_c();
		
		dStateWrapper_c<dLevelSelection_c> state;
		USING_STATES(dLevelSelection_c);
		DECLARE_STATE(SelectLevel);
		DECLARE_STATE(LevelSelectionMadeWait);
		
		int onCreate();
		int onDelete();
		int onDraw();
		int onExecute();
		
		static dLevelSelection_c *build();
		static dLevelSelection_c *instance;
		dPlayerModelHandler_c *modelHandler;
		
		void startLevel(/*dLevelInfo_c::entry_s *level*/);
		void getItemName(int id);
		
		bool layoutLoaded;
	
		m2d::EmbedLayout_c layout; 
		nw4r::lyt::Pane *test;
		
		bool startingLevel;
		int world_num;
		int level_num;
		int item;
		
		//unsigned short world_num;
		//unsigned short level_num;
};

dLevelSelection_c *dLevelSelection_c::instance = 0;

CREATE_STATE(dLevelSelection_c, SelectLevel);
CREATE_STATE(dLevelSelection_c, LevelSelectionMadeWait);

dLevelSelection_c *dLevelSelection_c::build() {
	OSReport("Creating Level Select... \n");

	void *buffer = AllocFromGameHeap1(sizeof(dLevelSelection_c));
	dLevelSelection_c *c = new(buffer) dLevelSelection_c;

	OSReport("Created Level Select... \n");

	instance = c;
	return c;
}

dLevelSelection_c::dLevelSelection_c() : state(this) {}

int dLevelSelection_c::onCreate() 
{
	OSReport("Level Select onCreate()... \n");
	startingLevel = false;
	item = 0;
	//title = L"T";
	
	/*if (!layoutLoaded) {
		bool layoutFound = layout.loadArc("Better_Level_Select.arc", false);
		
		if(!layoutFound)
		{
			OSReport("layout not found!");
			return false;
		}
		
		layout.build("Better_Level_Select_0.brlyt");
		
		if (IsWideScreen()) {
			layout.layout.rootPane->scale.x = 0.735f;
		} else {
			layout.clippingEnabled = true;
			layout.clipX = 0;
			layout.clipY = 52;
			layout.clipWidth = 640;
			layout.clipHeight = 352;
			layout.layout.rootPane->scale.x = 0.731f;
			layout.layout.rootPane->scale.y = 0.7711f;
		}
		
		// Textboxes
		//static const char *boxes[] = { "world", "level" };
		//layout.getTextBoxes(boxes, &world, 2);
		
		layoutLoaded = true;
	}*/
	
	state.setState(&StateID_SelectLevel);
	return true;
}

int dLevelSelection_c::onDraw() {
	//layout.scheduleForDrawing();
	return true;
}

int dLevelSelection_c::onDelete() {
	return true //layout.free();
}

int dLevelSelection_c::onExecute() {
	state.execute();
	
	if(startingLevel)
	{
		Player_Active[0] = 1;	// Slot 0 is active
		Player_ID[0] = 0;	// Controllable player slot 0 is active with player 0 (Mario) by default
		
		StartLevelInfo sl;
		sl.replayTypeMaybe = 0;
		sl.entrance = 0xFF;
		sl.areaMaybe = 0;
		sl.unk4 = 0;
		sl.purpose = 0;

		sl.world1 = world_num;
		sl.world2 = world_num;
		sl.level1 = level_num;
		sl.level2 = level_num;

		//ActivateWipe(WIPE_MARIO);
		//OSReport("Fred %p", GetGameMgr());

		DoStartLevel(GetGameMgr(), &sl);
	}
	
	return true;
}

void dLevelSelection_c::startLevel(/*dLevelInfo_c::entry_s *entry*/) {
	OSReport("Start Level Called \n");
}

void dLevelSelection_c::getItemName(int id)
{
	switch(id)
	{
		case 0:
			OSReport("Small Mario");
			break;
		case 1:
			OSReport("Mushroom");
			break;
		case 2:
			OSReport("Fire Flower");
			break;
		case 3:
			OSReport("Mini Mushroom");
			break;
		case 4:
			OSReport("Propeller Mushroom");
			break;
		case 5:
			OSReport("Penguin Suit");
			break;
		case 6:
			OSReport("Ice Flower");
			break;
	}
}

// All things level selection!
void dLevelSelection_c::beginState_SelectLevel() 
{
	OSReport("Level Selection state.\nAny values entered that do not correlate to an actual level within the /stage folder will result in a CRASH. \n"); 
	OSReport("World: Up/Down, Level: Left/Right \n"); 
}
void dLevelSelection_c::executeState_SelectLevel() 
{
	// Let's get our input...
	int nowPressed = Remocon_GetPressed(GetActiveRemocon());
	bool anythingPressed = ((nowPressed & WPAD_LEFT) || (nowPressed & WPAD_RIGHT) || (nowPressed & WPAD_UP) || (nowPressed & WPAD_DOWN)); // not releasing this source code rofl
	
	// Control stuff or some shit idrk
	if (nowPressed & WPAD_LEFT) 		level_num--;
	else if (nowPressed & WPAD_RIGHT) 	level_num++;
	else if (nowPressed & WPAD_UP) 		world_num++;
	else if (nowPressed & WPAD_DOWN)	world_num--;
	
	// For updating layout
	if(anythingPressed)
	{
		// OSReport still here for debug purposes
		if(level_num <= 0) level_num = 0; if(world_num <= 0) world_num = 0;
		OSReport("World Selected: %p", (world_num+1)); OSReport(" | Level Selected: %p\n", (level_num+1));
		
		// Our layout :.^)
		//world->SetString((const wchar_t*)world, (unsigned short)world_num);
		//level->SetString((const wchar_t*)level, (unsigned short)level_num);
		
		/*world_label = static_cast<wchar_t>(world_num + L'0');
		level_label = static_cast<wchar_t>(level_num + L'0');
		
		layout.findTextBoxByName("world")->SetString(&world_label);
		layout.findTextBoxByName("level")->SetString(&level_label);*/
	}
	
	// Powerup selection
	if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_ONE)) 
	{
		//Powerups - 0 = small; 1 = big; 2 = fire; 3 = mini; 4 = prop; 5 = peng; 6 = ice; 7 = hammer
		item++; if(item >= 7) item = 0;
		Player_Powerup[0] = item;
		
		OSReport("Item Selected: ", item); /* + */ getItemName(item); /* + */ OSReport("\n");
	}
	
	// Starting game
	else if (Wiimote_TestButtons(GetActiveWiimote(), WPAD_A | WPAD_TWO)) 
	{
		startingLevel = true;
		state.setState(&StateID_LevelSelectionMadeWait);
	}
}
void dLevelSelection_c::endState_SelectLevel() 
{}

// All things waiting!
void dLevelSelection_c::beginState_LevelSelectionMadeWait()
{}
void dLevelSelection_c::executeState_LevelSelectionMadeWait()
{}
void dLevelSelection_c::endState_LevelSelectionMadeWait()
{}
