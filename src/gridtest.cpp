#define GRIDTEST true

#include "lib/ivis_opengl/pieblitfunc.h"
#include "lib/widget/gridlayout.h"
#include "lib/widget/form.h"
#include "lib/widget/label.h"
#include "lib/widget/paragraph.h"
#include "lib/widget/scrollablelist.h"
#include "hci.h"
#include "init.h"
#include "frend.h"

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

class PaddingWidget: public WIDGET
{
public:
	PaddingWidget(uint32_t value): padding({value, value, value, value}) {}

	PaddingWidget(uint32_t top, uint32_t right, uint32_t bottom, uint32_t left)
		: padding({top, right, bottom, left})
	{
	}

protected:
    void geometryChanged() override
    {
        if (!children().empty())
        {
            auto &child = children().front();
            child->setGeometry(
                padding.left,
                padding.top,
                std::max(0, width() - (int)padding.left - (int)padding.right),
                std::max(0, height() - (int)padding.top - (int)padding.bottom)
            );
        }
    }

    int32_t idealWidth() override
    {
        auto innerWidth = children().empty() ? 0: children().front()->idealWidth();
        return innerWidth + padding.left + padding.right;
    }

    int32_t idealHeight() override
    {
        auto innerHeight = children().empty() ? 0: children().front()->idealHeight();
        return innerHeight + padding.top + padding.bottom;
    }

private:
    Padding padding;
};

enum class VerticalAlignment
{
    Top,
    Center,
    Bottom
};

enum class HorizontalAlignment
{
    Left,
    Center,
    Right
};

class AlignmentWidget: public WIDGET
{
public:
    AlignmentWidget(VerticalAlignment verticalAlignment, HorizontalAlignment horizontalAlignment)
        : verticalAlignment(verticalAlignment)
        , horizontalAlignment(horizontalAlignment)
    {
    }

protected:
    void geometryChanged() override
    {
        if (!children().empty())
        {
            auto &child = children().front();
            auto childLeft = 0;
            if (horizontalAlignment == HorizontalAlignment::Center)
            {
                childLeft = std::max(0, width() - child->idealWidth()) / 2;
            }
            else if (horizontalAlignment == HorizontalAlignment::Right)
            {
                childLeft = std::max(0, width() - child->idealWidth());
            }
            auto childTop = 0;
            if (verticalAlignment == VerticalAlignment::Center)
            {
                childTop = std::max(0, height() - child->idealHeight()) / 2;
            }
            else if (verticalAlignment == VerticalAlignment::Bottom)
            {
                childTop = std::max(0, height() - child->idealHeight());
            }
            child->setGeometry(
                childLeft,
                childTop,
                std::min(width(), child->idealWidth()),
                std::min(height(), child->idealHeight())
            );
        }
    }

    int32_t idealWidth() override
    {
        return children().empty() ? WIDGET::idealWidth(): children().front()->idealWidth();
    }

    int32_t idealHeight() override
    {
        return children().empty() ? WIDGET::idealHeight(): children().front()->idealHeight();
    }

private:
    VerticalAlignment verticalAlignment;
    HorizontalAlignment horizontalAlignment;
};

class ImageWidget: public WIDGET
{
public:
    ImageWidget(Image image): image(image)
    {
    }

    int32_t idealWidth()
    {
        return image.width();
    }

    int32_t idealHeight()
    {
        return image.height();
    }

protected:
    void display(int xOffset, int yOffset) override
    {
        auto x0 = xOffset + x();
        auto y0 = yOffset + y();
        iV_DrawImage2("image_research_up.png", x0, y0, width(), height());
    }

private:
    Image image;
};

std::shared_ptr<WIDGET> widgetChain(std::vector<std::shared_ptr<WIDGET>> chain)
{
    for (size_t i = 1; i < chain.size(); i++)
    {
        chain[i - 1]->attach(chain[i]);
    }

    return chain.front();
}


struct WidgetTree
{
public:
    WidgetTree(std::shared_ptr<WIDGET> root, std::shared_ptr<WIDGET> leaf): root(root)
    {
        root->attach(leaf);
    }

    WidgetTree(std::shared_ptr<WIDGET> root, WidgetTree leaf): root(root)
    {
        root->attach(leaf);
    }

    operator std::shared_ptr<WIDGET>()
    {
        return root;
    }

private:
    std::shared_ptr<WIDGET> root;
};


std::shared_ptr<WIDGET> wrap(std::shared_ptr<WIDGET> wrapWidget, std::shared_ptr<WIDGET> widget)
{
    wrapWidget->attach(widget);
    return wrapWidget;
}

bool gridtest()
{
    if (!GRIDTEST)
    {
        return false;
    }

    auto grid = std::make_shared<GridLayout>();

    auto makeLabel = [](const char *text) {
        auto label = std::make_shared<W_LABEL>();
        label->setFormattedString(text, 2000, font_regular);
        label->setCanTruncate(true);
        label->setFontColour({255, 255, 255, 255});
        return label;
    };

    auto makeButtonExperiment = [&]() {
        auto buttonGrid = std::make_shared<GridLayout>();
        buttonGrid->place({
            GridLocation{0, 0, 1, 1},
            WidgetTree
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Right),
                {
                    std::make_shared<PaddingWidget>(0, 3, 0, 0),
                    std::make_shared<ImageWidget>(Image(FrontImages, IMAGE_SCAVENGERS_ON))
                }
            }
        });

        buttonGrid->place({
            GridLocation{1, 0, 1, 1},
            WidgetTree
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Left),
                makeLabel("Button test")
            }
        });

        return WidgetTree
        {
            std::make_shared<BackgroundWidget>(PIELIGHT{0, 192, 255, 255}),
            {
                std::make_shared<PaddingWidget>(2),
                {
                    std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 128, 255}),
                    {
                        std::make_shared<PaddingWidget>(10),
                        buttonGrid
                    }
                }
            }
        };
    };

    auto makeContainer = [](PIELIGHT color, WidgetTree inner)
    {
        return WidgetTree
        {
            std::make_shared<PaddingWidget>(5),
            {
                std::make_shared<BackgroundWidget>(color),
                {
                    std::make_shared<PaddingWidget>(10),
                    {
                        std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 0, 32}),
                        inner
                    }
                }
            }
        };
    };

    grid->place({
        GridLocation{0, 0, 6, 1},
        makeContainer(
            PIELIGHT{128, 0, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Top, HorizontalAlignment::Center),
                makeLabel("Top")
            }
        )
    });

    grid->place({
        GridLocation{2, 1, 2, 1},
        makeContainer(
            PIELIGHT{128, 128, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Center),
                makeLabel("Hello World!\nThis is a test string.\nWith a little more\ncontent, to make it\na bit taller than the\nother ones.")
            }
        )
    });

    grid->place({
        GridLocation{0, 1, 2, 1},
        makeContainer(
            PIELIGHT{0, 0, 128, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Left),
                makeLabel("Left")
            }
        )
    });

    grid->place({
        GridLocation{4, 1, 2, 1},
        makeContainer(
            PIELIGHT{0, 128, 0, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Center, HorizontalAlignment::Right),
                makeLabel("Right")
            }
        )
    });

    grid->place({
        GridLocation{0, 2, 3, 1},
        makeContainer(
            PIELIGHT{128, 0, 128, 128},
            makeButtonExperiment()
        )
    });

    grid->place({
        GridLocation{3, 2, 3, 1},
        makeContainer(
            PIELIGHT{0, 128, 128, 128},
            {
                std::make_shared<AlignmentWidget>(VerticalAlignment::Bottom, HorizontalAlignment::Right),
                makeButtonExperiment()
            }
        )
    });

    auto outer = std::make_shared<PaddingWidget>(100, 200, 100, 200);
    outer->attach(grid);
    outer->setCalcLayout([](WIDGET *w, uint32_t, uint32_t, uint32_t screenWidth, uint32_t screenHeight) {
        w->setGeometry(0, 0, screenWidth, screenHeight);
    });
    psWScreen->psForm->attach(outer);
    return true;
}

bool previousTest()
{
    auto grid = std::make_shared<GridLayout>();
    grid->setCalcLayout([](WIDGET *w, uint32_t, uint32_t, uint32_t screenWidth, uint32_t screenHeight) {
        w->setGeometry(15, 15, screenWidth - 30, screenHeight - 30);
    });

    std::function<void(int, int, int, int)> allocate;
    allocate = [&](int colStart, int rowStart, int colEnd, int rowEnd) {
        auto cols = colEnd - colStart;
        auto rows = rowEnd - rowStart;

        if ((rand() % (cols * rows)) <= 1)
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

            ///*
            auto label = std::make_shared<W_LABEL>();
            label->setFormattedString(text, 2000, font_regular);
            label->setCanTruncate(true);
            label->setFontColour({255, 255, 255, 255});
            /*/
            auto label = std::make_shared<Paragraph>();
            label->addText(text);
            label->setGeometry(0, 0, 60, 40);
            auto scrollable = ScrollableListWidget::make();
            scrollable->setSnapOffset(false);
            scrollable->setGeometry(0, 0, 60, 40);
            scrollable->addItem(label);
            //*/

            auto vertical = verticals[rand() % (sizeof(verticals) / sizeof(verticals[0]))];
            auto horizontal = horizontals[rand() % (sizeof(horizontals) / sizeof(horizontals[0]))];

            GridPlacement placement;
            placement.widget = widgetChain({
                std::make_shared<PaddingWidget>(3),
                std::make_shared<BackgroundWidget>(
                    PIELIGHT{
                        (unsigned char)((rand() % 3) * 127),
                        (unsigned char)((rand() % 3) * 127),
                        (unsigned char)((rand() % 3) * 127),
                        127
                    }
                ),
                std::make_shared<PaddingWidget>(5),
                std::make_shared<BackgroundWidget>(PIELIGHT{0, 0, 0, 32}),
                std::make_shared<AlignmentWidget>(vertical, horizontal),
                label
            });

            placement.location.colStart = colStart;
            placement.location.rowStart = rowStart;
            placement.location.colSpan = cols;
            placement.location.rowSpan = rows;
            grid->place(placement);
            return;
        }

        if (cols > 1 && (rows <= 1 || (rand() % 2)))
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

    allocate(0, 0, 10, 10);
    psWScreen->psForm->attach(grid);

    return true;
}