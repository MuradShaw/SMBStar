#include "MONKEYDLUFFY/origin.h"

dOrigin_c *dOrigin_c::instance = 0;

CREATE_STATE(dOrigin_c, SelectLevel);
CREATE_STATE(dOrigin_c, LevelSelectionMadeWait);

dOrigin_c *dOrigin_c::build() {
	OSReport("Creating Origin.. \n");

	void *buffer = AllocFromGameHeap1(sizeof(dOrigin_c));
	dOrigin_c *c = new(buffer) dOrigin_c;

	OSReport("Created Origin... \n");

	instance = c;
	return c;
}

bool WM_Loading(void *ptr) {
	OSReport("WM_Loading \n");
	DVD_Start();

	return true;
}

bool WM_EndLoading(void *ptr) {
	OSReport("WM_EndLoading \n");
	
	if (DVD_StillLoading(GetDVDClass2()))
	{
		return false;
	}

	DVD_End();
	return true;

	//return (DVD_StillLoading(GetDVDClass2())) ? false : true;
}

bool WM_SetupDependencies(void *ptr) {
	OSReport("WM_SetupDependencies \n");

	dWorldCamera_c::instance->nPos = cNode->camPos;
	dWorldCamera_c::instance->nRot = cNode->camRot;
	dWorldCamera_c::instance->nRot.x -= 90.0f;
	
	dOrigin_c *wm = (dOrigin_c *)ptr;
	wm->map = (dWMMap_c *)CreateParentedObject(WM_MAP, wm, 0, 0);

	return true;
}

ChainedFunc initFunctions[] =
	{
		WM_Loading,
		WM_EndLoading,
		WM_SetupDependencies
	};

dOrigin_c::dOrigin_c() : state(this)
{
	initChain.setup(initFunctions, 3);
	setInitChain(initChain);
}

int dOrigin_c::onCreate() 
{
	OSReport("Origin onCreate \n");

	return true;
}

int dOrigin_c::onDraw() {
	return true;
}

int dOrigin_c::onDelete() {
    return true;
}

int dOrigin_c::onExecute() {
	state.execute();
	
	return true;
}