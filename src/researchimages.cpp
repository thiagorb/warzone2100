#include "hci.h"
#include "intdisplay.h"
#include "init.h"
#include "lib/framework/frameresource.h"
#include "lib/ivis_opengl/gfx_api_gl.h"
#include "lib/widget/bar.h"
#include "warzoneconfig.h"
#include "hci/research.h"

#define RESEARCH_IMAGES true
#define RESEARCH_IMAGES_ONLY_MODELS false
#define SCALE 5

extern std::vector<RESEARCH> asResearch;
extern ImdObject getResearchObjectImage(RESEARCH *research);
extern bool wzChangeDisplayScale(unsigned int displayScale, bool ignoreLimit);

class ResearchButton : public IntFancyButton
{
public:
    RESEARCH* research;
	std::shared_ptr<W_BARGRAPH> costBar;

    void initialize()
    {
        W_BARINIT sBarInit;
        sBarInit.x = STAT_TIMEBARX;
        sBarInit.y = STAT_TIMEBARY;
        sBarInit.width = STAT_PROGBARWIDTH;
        sBarInit.height = STAT_PROGBARHEIGHT;
        sBarInit.sCol = WZCOL_ACTION_PROGRESS_BAR_MAJOR;
        sBarInit.sMinorCol = WZCOL_ACTION_PROGRESS_BAR_MINOR;

        sBarInit.iRange = GAME_TICKS_PER_SEC;
        costBar = std::make_shared<W_BARGRAPH>(&sBarInit);
        if (!RESEARCH_IMAGES_ONLY_MODELS)
        {
            attach(costBar);
        }
        costBar->setBackgroundColour(WZCOL_BLACK);
    }

    void displayClear(int xOffset, int yOffset) override
    {
        pie_UniTransBoxFill(0, 0, STAT_BUTWIDTH, STAT_BUTHEIGHT, { 255, 0, 255, 255 });

        if (!RESEARCH_IMAGES_ONLY_MODELS)
        {
            IntFancyButton::displayClear(xOffset, yOffset);
        }
    }

    void display(int xOffset, int yOffset) override
    {
        ASSERT_NOT_NULLPTR_OR_RETURN(, research);

        auto cost = research ? research->researchPower : 0;
        costBar->majorSize = std::min(100, (int32_t)(cost / POWERPOINTS_DROIDDIV));

        if (RESEARCH_IMAGES_ONLY_MODELS)
        {
            displayIMD(nullptr, getResearchObjectImage(research), xOffset, yOffset);
        }
        else
        {
            auto researchIcon = research->iconID != NO_RESEARCH_ICON ? Image(IntImages, research->iconID) : Image();
            displayIMD(researchIcon, getResearchObjectImage(research), xOffset, yOffset);

            if (research->subGroup != NO_RESEARCH_ICON)
            {
                iV_DrawImage(IntImages, research->subGroup, xOffset + x() + STAT_BUTWIDTH - 16, yOffset + y() + 3);
            }
        }
    }
};

std::unique_ptr<iV_Image> generate_image(int screenX, int screenY, int width, int height)
{
	auto image = std::unique_ptr<iV_Image>(new iV_Image());
	image->width = width * SCALE;
	image->height = height * SCALE;
	image->depth = 8;
	image->bmp = (unsigned char *)malloc((size_t)3 * (size_t)image->width * (size_t)image->height);
	ASSERT_OR_RETURN(nullptr, image->bmp != nullptr, "Failed to allocate buffer");

	glPixelStorei(GL_PACK_ALIGNMENT, 1);
	glReadPixels(screenX, (screenHeight - screenY - height) * SCALE, width * SCALE, height * SCALE, GL_RGB, GL_UNSIGNED_BYTE, image->bmp);

    return image;
}

bool research_images()
{
    if (!RESEARCH_IMAGES)
    {
        return false;
    }

    static bool loaded = false;

    if (!loaded)
    {
        loaded = true;

        wzChangeDisplayScale(SCALE * 100, true);
        stageOneInitialise();
        resLoad("wrf/stats.wrf", 4);
        resLoad("wrf/multires.wrf", 5);

        auto button = std::make_shared<ResearchButton>();
        button->initialize();
        button->move(0, 0);

        for (auto& research: asResearch) {
            button->research = &research;

            glDepthMask(GL_TRUE);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

            button->displayRecursive();
            auto image = generate_image(0, 0, STAT_BUTWIDTH, STAT_BUTHEIGHT);
            auto path = astringf("screenshots/%s.png", research.name.toUtf8().c_str());
            iV_saveImage_PNG(path.c_str(), image.get());
        }
    }

    return true;
}
