#include <game.h>
#include <common.h>
#include <dCourse.h>

/* Sets text on the screen to the zone name set */
class daEnZoneNameHandler_c : public dEn_c {
    public:
        static daEnZoneNameHandler_c *build();

        int onCreate();
        int onDelete();
        int onExecute();

        dStateWrapper_c<daEnZoneNameHandler_c> state;

        static daEnZoneNameHandler_c *instance;
        static daEnZoneNameHandler_c *build();

        USING_STATES(daEnZoneNameHandler_c);
        DECLARE_STATE(LoadRes);
        DECLARE_STATE(Wait);
        DECLARE_STATE(FadeIn);
        DECLARE_STATE(ShowWait);
        DECLARE_STATE(FadeOut);

        m2d::EmbedLayout_c layout;
        dDvdLoader_c zoneNameLoader;

        bool layoutLoaded;
        int zoneNameID;

    private:
        struct entry_s {
            u32 nameOffset;
        };

        struct header_s {
            u32 count;
            entry_s entry[1];
        };
};