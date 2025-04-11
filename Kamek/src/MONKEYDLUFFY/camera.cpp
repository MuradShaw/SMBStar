#include "MONKEYDLUFFY/camera.h"

dWorldCamera_c *dWorldCamera_c::instance = 0;

dWorldCamera_c *dWorldCamera_c::build()
{
	void *buffer = AllocFromGameHeap1(sizeof(dWorldCamera_c));
	dWorldCamera_c *c = new (buffer) dWorldCamera_c;

	instance = c;
	return c;
}

dWorldCamera_c::dWorldCamera_c() {}

int dWorldCamera_c::onCreate()
{
	this->camPos = (Point3d){0.0f, 500.0f, 1000.0f};
	this->camTarget = (Point3d){0.0f, 0.0f, 0.0f};
	this->camUp = (Point3d){0.0f, 1.0f, 0.0f};
	this->camRotate = (Point3d){-40.0f, 0.0f, 0.0f}; // ZXY order

	//oPos = nPos = camPos;
	//oRot = nRot = camRotate;

	time = -1;
	timeStart = -1;

	this->postureIsLookAt = false;

	return true;
}

int dWorldCamera_c::onDelete() { return true; }

int dWorldCamera_c::onExecute()
{
	if (timeStart > -1 && time == -1)
		time = timeStart;
	if (time > -1)
	{
		float prg = 1 - ((float)time / (float)timeStart);
		float mEase = 0.0f;

		switch (ease)
		{
		case 0: //linear
			mEase = prg;
			break;
		case 1: //easeInOutSine (thank you easings.net for the easing info!)
			mEase = -(cos(M_PI * prg) - 1) / 2;
			break;
		case 2:
		case 4: //easeInQuad
			mEase = prg * prg;
			break;
		case 3:
		case 5: //easeOutQuad
			mEase = 1 - (1 - prg) * (1 - prg);
			break;
		default:
			mEase = prg;
			break;
		}

		camPos.x = oPos.x + (nPos.x - oPos.x) * mEase;
		camPos.y = oPos.y + (nPos.y - oPos.y) * mEase;
		camPos.z = oPos.z + (nPos.z - oPos.z) * mEase;

		camRotate.x = oRot.x + (nRot.x - oRot.x) * mEase;
		camRotate.y = oRot.y + (nRot.y - oRot.y) * mEase;
		camRotate.z = oRot.z + (nRot.z - oRot.z) * mEase;

		time--;
	}
	if (time == -1)
	{
		camPos = nPos;
		camRotate = nRot;

		time = -1;
		timeStart = -1;

		moving = false;
	}

	int nowHeld = Remocon_GetButtons(GetActiveRemocon());
	int debugNowPressed = Remocon_GetPressed(GetActiveRemocon());
	/*
	if (debugNowPressed & WPAD_TWO)
	{
		cameraDebug = !cameraDebug;
	}
	if (cameraDebug) {
    if(postureIsLookAt || !(nowHeld & WPAD_B)) {
	if (nowHeld & WPAD_LEFT) camPos.x -= 5;
	else if (nowHeld & WPAD_RIGHT) camPos.x += 5;
	if (nowHeld & WPAD_UP) camPos.z -= 5;
	else if (nowHeld & WPAD_DOWN) camPos.z += 5;
	}

    if(nowHeld & WPAD_PLUS) camPos.y += 5;
	else if(nowHeld & WPAD_MINUS) camPos.y -= 5;

	//if(debugNowPressed & WPAD_A) postureIsLookAt = !postureIsLookAt;
	
	if(!postureIsLookAt && nowHeld & WPAD_B) {
		if (nowHeld & WPAD_LEFT) camRotate.y += 1;
		else if (nowHeld & WPAD_RIGHT) camRotate.y -= 1;
		if (nowHeld & WPAD_UP) camRotate.x -= 1;
		else if (nowHeld & WPAD_DOWN) camRotate.x += 1;
	}
	}
	*/
	return true;
}

int dWorldCamera_c::onDraw()
{
	GXRenderModeObj *rmode = nw4r::g3d::G3DState::GetRenderModeObj();

	nw4r::g3d::Camera cam3d(GetCameraByID(0));

	if (rmode->field_rendering != 0)
		cam3d.SetViewportJitter(VIGetNextField());

	cam3d.SetPerspective(16, 832.0f/456.0f , 0.1f, 6000.0f);

	nw4r::g3d::Camera::PostureInfo posture;

	if (!postureIsLookAt)
	{
		posture.tp = nw4r::g3d::Camera::POSTURE_ROTATE;
		posture.cameraRotate = this->camRotate;
	}
	else
	{
		posture.tp = nw4r::g3d::Camera::POSTURE_LOOKAT;
		posture.cameraUp = this->camUp;
		posture.cameraTarget = this->camTarget;
	}

	cam3d.SetPosition(this->camPos);
	cam3d.SetPosture(posture);

	return true;
}

void dWorldCamera_c::SetViewInterpolate(Vec newPos, Vec newRot, int frames, char easeType)
{
	if (time > -1)
		return;
	moving = true;

	nPos = newPos;
	nRot = (Vec){newRot.x - 90.0f, newRot.y, newRot.z};

	oPos = camPos;
	oRot = camRotate;

	timeStart = frames;
	ease = easeType;
	//The magic happens in onExecute
}