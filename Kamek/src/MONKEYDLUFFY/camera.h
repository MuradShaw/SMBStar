#ifndef __MONKEYDLUFFY_CAMERA_H
#define __MONKEYDLUFFY_CAMERA_H

#include "MONKEYDLUFFY/origin.h"

class dWorldCamera_c : public dBase_c
{
    public:
        int onCreate();
        int onDelete();
        int onExecute();
        int onDraw();

        dWorldCamera_c();

        void SetViewInterpolate(Vec newPos, Vec newRot, int frames, char easeType);

        EGG::ProjectOrtho projection2d;
        EGG::LookAtCamera camera2d;

        Vec camPos, camTarget, camUp, camRotate;
        Vec nPos, nRot, oPos, oRot;

        bool postureIsLookAt;
        //bool cameraDebug;

        int time, timeStart;
        char ease;

        bool moving;

        static dWorldCamera_c *build();
        static dWorldCamera_c *instance;
};

#endif