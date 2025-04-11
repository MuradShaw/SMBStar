#ifndef __MONKEYDLUFFY_MAP_H
#define __MONKEYDLUFFY_MAP_H

#include "MONKEYDLUFFY/origin.h"

#define GEKKO
#include "rvl/mtx.h"
#include "rvl/GXEnum.h"
#include "rvl/GXStruct.h"
#include "rvl/GXTransform.h"
#include "rvl/GXGeometry.h"
#include "rvl/GXDispList.h"
#include "rvl/GXLighting.h"
#include "rvl/GXTev.h"
#include "rvl/GXTexture.h"
#include "rvl/GXCull.h"
#include "rvl/GXPixel.h"
#include "rvl/GXBump.h"
#include "rvl/GXVert.h"
#include "rvl/vifuncs.h"

public:
    dWMMap_c();

    int onCreate();
    int onExecute();
    int onDraw();
    int onDelete();

    class renderer_c : public m3d::proc_c {
			public:
				mAllocator_c allocator;

				void drawOpa();
				void drawXlu();

			private:
				void drawLayers();

				void beginRendering();
				void endRendering();

				void renderTileLayer(dKPLayer_s *layer, dKPLayer_s::sector_s *sector);
				void renderDoodadLayer(dKPLayer_s *layer);

				void loadTexture(GXTexObj *obj);

				void loadCamera();
				void loadCamera(Mtx m);

				GXTexObj *currentTexture;
				Mtx renderMtx;
				float baseZ;

				int minX, minY, maxX, maxY;
		};
    
    // Model shazz
    mHeapAllocator_c allocator;
    
    nw4r::g3d::ResFile resFile;
    nw4r::g3d::ResMdl mdl;
    nw4r::g3d::ResAnmChr anmChr;
    mMtx matrix;

    renderer_c renderer;
	enum EffRenderCount { EFFECT_RENDERER_COUNT = 12 };
	BGGMEffectRenderer effectRenderers[EFFECT_RENDERER_COUNT];

    static dWMMap_c *build();
    static dWMMap_c *instance;
};

#endif
