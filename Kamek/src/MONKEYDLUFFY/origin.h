#ifndef __MONKEYDLUFFY_ORIGIN_H
#define __MONKEYDLUFFY_ORIGIN_H

#include <common.h>
#include <game.h>
#include <g3dhax.h>
#include <sfx.h>
#include <newer.h>

#include <stage.h>

#include "fileload.h"
#include "MONKEYDLUFFY/camera.h"
//#include "MONKEYDLUFFY/levelSelect.cpp"

class dWorldCamera_c;
class dWMMap_c;

class dOrigin_c : public dScene_c {
	public:
        dOrigin_c();

		FunctionChain initChain;
		dStateWrapper_c<dOrigin_c> state;
		dWMMap_c *map;
		
		USING_STATES(dOrigin_c);
		DECLARE_STATE(SelectLevel);
		DECLARE_STATE(LevelSelectionMadeWait);

		//dLevelSelection_c *levelSelect;

		/*	Previously From dLevelSelection_c */
		void getItemName(int id);
		
		bool layoutLoaded;
	
		m2d::EmbedLayout_c layout; 
		nw4r::lyt::Pane *test;
		
		bool startingLevel;
		int world_num;
		int level_num;
		int item;
		/*--								--*/

		int onCreate();
		int onDelete();
		int onDraw();
		int onExecute();
        
        static dOrigin_c *build();
		static dOrigin_c *instance;
};

#endif