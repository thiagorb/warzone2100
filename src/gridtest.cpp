#define GRIDTEST false

#include "lib/ivis_opengl/pieblitfunc.h"
#include "lib/widget/gridlayout.h"
#include "lib/widget/form.h"
#include "lib/widget/label.h"
#include "lib/widget/margin.h"
#include "lib/widget/scrollablelist.h"
#include "lib/widget/alignment.h"
#include "lib/widget/image.h"
#include "lib/widget/tree.h"
#include "hci.h"
#include "init.h"
#include "frend.h"
#include "lib/ivis_opengl/bitimage.h"

class BackgroundWidget: public WIDGET
{
public:
    BackgroundWidget(PIELIGHT color): color(color)
    {
    }

protected:
    void geometryChanged() override
    {
        if (!children().empty())
        {
            auto &child = children().front();
            child->setGeometry(0, 0, width(), height());
        }
    }

    void display(int xOffset, int yOffset) override
    {
        auto x0 = xOffset + x();
        auto y0 = yOffset + y();
	    pie_UniTransBoxFill(x0, y0, x0 + width(), y0 + height(), color);
    }

    int32_t idealWidth() override
    {
        return children().empty()? WIDGET::idealWidth() : children().front()->idealWidth();
    }

    int32_t idealHeight() override
    {
        return children().empty()? WIDGET::idealHeight() : children().front()->idealHeight();
    }

private:
    PIELIGHT color;
};

std::shared_ptr<WIDGET> newTest()
{
    auto grid = std::make_shared<GridLayout>();

    auto makeLabel = [](const char *text) {
        auto label = std::make_shared<W_LABEL>();
        label->setFormattedString(text, 2000, font_regular);
        label->setCanTruncate(true);
        label->setFontColour({255, 255, 255, 255});
        return label;
    };

    auto makeContainer = [](PIELIGHT color, WidgetTree inner)
    {
        return WidgetTree
        {
            std::make_shared<MarginWidget>(5),
            {{
                std::make_shared<BackgroundWidget>(color),
                {{
                    std::make_shared<MarginWidget>(10),
                    {{
                        std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 0, 32}),
                        {{
                            inner
                        }}
                    }}
                }}
            }}
        };
    };

    grid->place(
        {0, 6},
        {0, 1, false},
        makeContainer(
            PIELIGHT{128, 0, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Top, HorizontalAlignment::Center),
                {{
                    makeLabel("Top")
                }}
            }
        )
    );

    grid->place(
        {2, 2},
        {1, 1},
        makeContainer(
            PIELIGHT{128, 128, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Center),
                {{
                    makeLabel("Hello World!\nThis is a test string.\nWith a little more\ncontent, to make it\na bit taller than the\nother ones.")
                }}
            }
        )
    );

    grid->place(
        {0, 2, false},
        {1, 1},
        makeContainer(
            PIELIGHT{0, 0, 128, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Left),
                {{
                    makeLabel("Left")
                }}
            }
        )
    );

    grid->place(
        {4, 2, false},
        {1, 1},
        makeContainer(
            PIELIGHT{0, 128, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Right),
                {{
                    makeLabel("Right")
                }}
            }
        )
    );

    //*
    auto makeButtonExperiment = [&]() {
        auto buttonGrid = std::make_shared<GridLayout>();
        buttonGrid->place(
            {0, 1},
            {0, 1},
            WidgetTree {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Right),
                {{
                    std::make_shared<MarginWidget>(0, 3, 0, 0),
                    {{
                        std::make_shared<ImageWidget>(iV_GetImage("image_commanddroid_down.png"))
                    }}
                }}
            }
        );

        buttonGrid->place(
            {1, 1},
            {0, 1},
            WidgetTree {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Left),
                {{
                    makeLabel("Button test")
                }}
            }
        );

        return WidgetTree {
            std::make_shared<BackgroundWidget>(PIELIGHT{0, 192, 255, 255}),
            {{
                std::make_shared<MarginWidget>(2),
                {{
                    std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 128, 255}),
                    {{
                        std::make_shared<MarginWidget>(10),
                        {{
                            buttonGrid
                        }}
                    }}
                }}
            }}
        };
    };

    grid->place(
        {0, 3},
        {2, 1, false},
        makeContainer(
            PIELIGHT{128, 0, 128, 128},
            makeButtonExperiment()
        )
    );

    grid->place(
        {3, 3},
        {2, 1, false},
        makeContainer(
            PIELIGHT{0, 128, 128, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Bottom, HorizontalAlignment::Right),
                {{
                    makeButtonExperiment()
                }}
            }
        )
    );
    //*/

    return grid;
}

std::shared_ptr<WIDGET> previousTest()
{
    auto grid = std::make_shared<GridLayout>();
    const auto COLS = 6;
    const auto ROWS = 8;

    std::function<void(int, int, int, int)> allocate;
    allocate = [&](int colStart, int rowStart, int colEnd, int rowEnd) {
        auto cols = colEnd - colStart;
        auto rows = rowEnd - rowStart;

        if ((rand() % (cols * rows * cols * rows)) <= 0)
        {
            const char *texts[] = {
                "A",
                "Small",
                "Medium lenght label",
                "A\nlabel\nwith\na\nfew\nlines",
            };
            VerticalAlignment verticals[] = {
                VerticalAlignment::Top,
                VerticalAlignment::Center,
                VerticalAlignment::Bottom,
            };
            HorizontalAlignment horizontals[] = {
                HorizontalAlignment::Left,
                HorizontalAlignment::Center,
                HorizontalAlignment::Right,
            };
            auto text = texts[rand() % (sizeof(texts) / sizeof(texts[0]))];

            auto label = std::make_shared<W_LABEL>();
            label->setFormattedString(text, 2000, font_regular);
            label->setCanTruncate(true);
            label->setFontColour({255, 255, 255, 255});

            auto vertical = verticals[rand() % (sizeof(verticals) / sizeof(verticals[0]))];
            auto horizontal = horizontals[rand() % (sizeof(horizontals) / sizeof(horizontals[0]))];

            grid->place(
                {(uint32_t)colStart, (uint32_t)cols}, 
                {(uint32_t)rowStart, (uint32_t)rows}, 
                WidgetTree
                {
                    std::make_shared<MarginWidget>(3),
                    {{
                        std::make_shared<BackgroundWidget>(
                            PIELIGHT{
                                (unsigned char)((rand() % 3) * 127),
                                (unsigned char)((rand() % 3) * 127),
                                (unsigned char)((rand() % 3) * 127),
                                127
                            }
                        ),
                        {{
                            std::make_shared<MarginWidget>(5),
                            {{
                                std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 0, 32}),
                                {{
                                    std::make_shared<AlignmentWidget>(vertical, horizontal),
                                    {{
                                        label
                                    }}
                                }}
                            }}
                        }}
                    }}
                }
            );
            return;
        }

        if (cols > 1 && (rows <= 1 || (rand() % (COLS + ROWS) <= COLS)))
        {
            auto split = 1 + rand() % (cols - 1);
            allocate(colStart, rowStart, colStart + split, rowEnd);
            allocate(colStart + split, rowStart, colEnd, rowEnd);
        }
        else
        {
            auto split = 1 + rand() % (rows - 1);
            allocate(colStart, rowStart, colEnd, rowStart + split);
            allocate(colStart, rowStart + split, colEnd, rowEnd);
        }
    };

    allocate(0, 0, COLS, ROWS);

    grid->setGeometry(0, 0, grid->idealWidth(), grid->idealHeight());
    auto outer = ScrollableListWidget::make();
    outer->addItem(grid);

    return outer;
}

bool gridtest()
{
    if (!GRIDTEST)
    {
        return false;
    }

    auto w1 = newTest();
    auto w2 = previousTest();

    auto split = std::make_shared<GridLayout>();
    split->place({0}, {0}, newTest());
    split->place({1}, {0}, previousTest());
    split->setCalcLayout([](WIDGET *w, uint32_t, uint32_t, uint32_t screenWidth, uint32_t screenHeight) {
        w->setGeometry(0, 0, screenWidth, screenHeight);
    });
    psWScreen->psForm->attach(split);

    return true;
}
