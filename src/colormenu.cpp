#include "colormenu.h"
#include "utils.h"
#include <QPoint>
#include <QTimer>
#include <QClipboard>
#include <QPainter>
#include <QApplication>
#include <QDebug>

ColorMenu::ColorMenu(int x, int y, int size, QColor color, QWidget *parent) : QWidget(parent)
{
    windowX = x;
    windowY = y;
    windowSize = size;
    windowColor = color;

    menuOffsetY = 50;
    
    clickMenuItem = false;

    setWindowFlags(Qt::FramelessWindowHint | Qt::Tool);
    setMouseTracking(true);
    installEventFilter(this);

    colorMenu = new QMenu();
    connect(colorMenu, &QMenu::aboutToHide, this, [&] () {
            QTimer::singleShot(200, this, [&] () {
                    if (!clickMenuItem) {
                        exit();
                    }
                });
        });
    
    rgbAction = new QAction("RGB", this);
    connect(rgbAction, &QAction::triggered, this, &ColorMenu::copyRGBColor);
    rgbFloatAction = new QAction("RGB (1.0)", this);
    connect(rgbFloatAction, &QAction::triggered, this, &ColorMenu::copyFloatRGBColor);
    rgbaAction = new QAction("RGBA", this);
    connect(rgbaAction, &QAction::triggered, this, &ColorMenu::copyRGBAColor);
    rgbaFloatAction = new QAction("RGBA (1.0)", this);
    connect(rgbaFloatAction, &QAction::triggered, this, &ColorMenu::copyFloatRGBAColor);
    hexAction = new QAction("HEX",this);
    connect(hexAction, &QAction::triggered, this, &ColorMenu::copyHexColor);

    colorMenu->addAction(rgbAction);
    colorMenu->addAction(rgbFloatAction);
    colorMenu->addAction(rgbaAction);
    colorMenu->addAction(rgbaFloatAction);
    colorMenu->addAction(hexAction);

    setFocus();

    move(x, y);
    resize(windowSize, windowSize);
}

ColorMenu::~ColorMenu()
{
}

void ColorMenu::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setOpacity(1);
    painter.setRenderHint(QPainter::Antialiasing, true);
    painter.setBrush(windowColor);
    painter.drawRect(QRect(0, 0, windowSize, windowSize));
}

void ColorMenu::showMenu()
{
    colorMenu->exec(QPoint(windowX - windowSize / 2, windowY - windowSize / 2 + menuOffsetY));
}

void ColorMenu::copyRGBColor()
{
    clickMenuItem = true;
    copyColor(windowColor, Utils::colorToRGB(windowColor));
}

void ColorMenu::copyFloatRGBColor()
{
    clickMenuItem = true;
    copyColor(windowColor, Utils::colorToFloatRGB(windowColor));
}

void ColorMenu::copyRGBAColor()
{
    clickMenuItem = true;
    copyColor(windowColor, Utils::colorToRGBA(windowColor));
}

void ColorMenu::copyFloatRGBAColor()
{
    clickMenuItem = true;
    copyColor(windowColor, Utils::colorToFloatRGBA(windowColor));
}

void ColorMenu::copyHexColor()
{
    clickMenuItem = true;
    copyColor(windowColor, Utils::colorToHex(windowColor));
}
