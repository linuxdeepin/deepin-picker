#include "picker.h"
#include "animation.h"
#include "settings.h"
#include "utils.h"
#include "colormenu.h"
#include <QPainter>
#include <QBitmap>
#include <QPixmap>
#include <QMenu>
#include <QAction>
#include <QClipboard>
#include <QStyleFactory>
#include <QScreen>
#include <QApplication>
#include <QDesktopWidget>
#include <QDebug>

Picker::Picker(QWidget *parent) : QWidget(parent)
{
    // Init window flags.
    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    installEventFilter(this);

    // Init attributes.
    width = 220;
    height = 220;

    windowWidth = 236;
    windowHeight = 236;

    screenshotWidth = 11;
    screenshotHeight = 11;

    blockWidth = 20;
    blockHeight = 20;

    displayCursorDot = false;

    // Init update screenshot timer.
    updateScreenshotTimer = new QTimer(this);
    updateScreenshotTimer->setSingleShot(true);
    connect(updateScreenshotTimer, SIGNAL(timeout()), this, SLOT(updateScreenshot()));

    // Init window size and position.
    screenPixmap = QApplication::primaryScreen()->grabWindow(0);
    resize(screenPixmap.size());
    move(0, 0);

    // Show.
    show();

    // Update screenshot.
    updateScreenshot();
}

Picker::~Picker()
{
}

void Picker::paintEvent(QPaintEvent *)
{
}

void Picker::handleMouseMove(int, int)
{
    // Update screenshot.
    if (updateScreenshotTimer->isActive()) {
        updateScreenshotTimer->stop();
    }
    updateScreenshotTimer->start(5);
}

void Picker::updateScreenshot()
{
    if (!displayCursorDot) {
        // Get cursor coordinate.
        cursorX = QCursor::pos().x();
        cursorY = QCursor::pos().y();

        int offsetX = (windowWidth - width) / 2;
        int offsetY = (windowHeight - height) / 2;

        QPixmap cursorPixmap(Utils::getQrcPath("shadow.png"));

        // Get image under cursor.
        screenshotPixmap = QApplication::primaryScreen()->grabWindow(
            0,
            cursorX - screenshotWidth / 2,
            cursorY - screenshotHeight / 2,
            screenshotWidth,
            screenshotHeight).scaled(width, height);

        // Draw on screenshot.
        QPainter painter(&cursorPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        painter.save();
        QPainterPath circlePath;
        circlePath.addEllipse(2 + offsetX, 2 + offsetY, width - 4, height - 4);
        painter.setClipPath(circlePath);
        painter.drawPixmap(1 + offsetX, 1 + offsetY, screenshotPixmap);
        painter.restore();

        // Draw circle bound.
        int outsidePenWidth = 1;
        QPen outsidePen("#000000");
        outsidePen.setWidth(outsidePenWidth);
        painter.setOpacity(0.05);
        painter.setPen(outsidePen);
        painter.drawEllipse(1 + offsetX, 1 + offsetY, width - 2, height - 2);

        int insidePenWidth = 4;
        QPen insidePen("#ffffff");
        insidePen.setWidth(insidePenWidth);
        painter.setOpacity(0.5);
        painter.setPen(insidePen);
        painter.drawEllipse(3 + offsetX, 3 + offsetY, width - 6, height - 6);

        // Draw focus block.
        painter.setOpacity(1);
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setOpacity(0.2);
        painter.setPen("#000000");
        painter.drawRect(QRect(width / 2 - blockWidth / 2 + offsetX, height / 2 - blockHeight / 2 + offsetY, blockWidth, blockHeight));
        painter.setOpacity(1);
        painter.setPen("#ffffff");
        painter.drawRect(QRect(width / 2 - blockWidth / 2 + 1 + offsetX, height / 2 - blockHeight / 2 + 1 + offsetY, blockWidth - 2, blockHeight - 2));

        // Set screenshot as cursor.
        QApplication::setOverrideCursor(QCursor(cursorPixmap));
    }
}

void Picker::handleLeftButtonPress(int x, int y)
{
    if (!displayCursorDot) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        hide();

        Settings *settings = new Settings();

        if (!Utils::fileExists(settings->configPath())) {
            settings->setOption("color_type", "HEX");
        }
        QString colorType = settings->getOption("color_type").toString();

        QColor color = getColorAtCursor(x, y);

        copyColor(color, colorType);
    }
}

void Picker::handleRightButtonRelease(int x, int y)
{
    if (!displayCursorDot) {
        displayCursorDot = true;

        menu = new ColorMenu(x - blockWidth / 2, y - blockHeight / 2, blockWidth, getColorAtCursor(x, y));
        connect(menu, &ColorMenu::copyColor, this, &Picker::copyColor, Qt::QueuedConnection);
        connect(menu, &ColorMenu::exit, this, &Picker::exit, Qt::QueuedConnection);
        menu->show();
        menu->setFocus();
    
        animation = new Animation(x, y, screenshotPixmap, getColorAtCursor(x, y));
        connect(animation, &Animation::finish, this, &Picker::popupColorMenu, Qt::QueuedConnection);

        QApplication::setOverrideCursor(Qt::ArrowCursor);

        QTimer::singleShot(10, animation, &Animation::show);
    }
}

QColor Picker::getColorAtCursor(int x, int y)
{
    QImage img = screenPixmap.copy(x, y, 1, 1).toImage();
    return QColor(img.pixel(0, 0));
}

void Picker::popupColorMenu(int x, int y, QColor color)
{
    hide();

    QTimer::singleShot(10, menu, &ColorMenu::showMenu);
}
