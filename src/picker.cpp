#include "picker.h"
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

        // Get image under cursor.
        screenshotPixmap = QApplication::primaryScreen()->grabWindow(
            0,
            cursorX - screenshotWidth / 2,
            cursorY - screenshotHeight / 2,
            screenshotWidth,
            screenshotHeight).scaled(width, height);

        // Clip cursor image with circle.
        QBitmap mask(screenshotPixmap.size());
        QPainter maskPainter(&mask);
        maskPainter.setRenderHint(QPainter::Antialiasing, true);
        mask.fill(Qt::white);
        maskPainter.setBrush(Qt::black);
        maskPainter.drawEllipse(1, 1, mask.width() - 2, mask.height() - 2);
        screenshotPixmap.setMask(mask);

        // Draw on screenshot.
        QPainter painter(&screenshotPixmap);
        painter.setRenderHint(QPainter::Antialiasing, true);

        // Draw circle bound.
        int penWidth = 2;
        QPen outsidePen("#000000");
        outsidePen.setWidth(penWidth);
        painter.setPen(outsidePen);
        painter.drawEllipse(penWidth / 2, penWidth / 2, mask.width() - penWidth, mask.height() - penWidth);

        QPen insidePen("#ffffff");
        insidePen.setWidth(penWidth);
        painter.setPen(insidePen);
        painter.drawEllipse(penWidth * 3 / 2, penWidth * 3 / 2, mask.width() - penWidth * 3, mask.height() - penWidth * 3);

        // Draw focus block.
        painter.setRenderHint(QPainter::Antialiasing, false);
        painter.setPen("#000000");
        painter.drawRect(QRect(width / 2 - blockWidth / 2, height / 2 - blockHeight / 2, blockWidth, blockHeight));
        painter.setPen("#ffffff");
        painter.drawRect(QRect(width / 2 - blockWidth / 2 + 1, height / 2 - blockHeight / 2 + 1, blockWidth - 2, blockHeight - 2));

        // Set screenshot as cursor.
        QApplication::setOverrideCursor(QCursor(screenshotPixmap));
    }
}

void Picker::handleLeftButtonPress(int x, int y)
{
    if (!displayCursorDot) {
        QApplication::setOverrideCursor(Qt::ArrowCursor);
        hide();
        
        QColor color = getColorAtCursor(x, y);
        QString hex = Utils::colorToHex(color);
        
        copyColor(color, hex);
    }
}

void Picker::handleRightButtonPress(int x, int y)
{
    if (!displayCursorDot) {
        displayCursorDot = true;

        QApplication::setOverrideCursor(Qt::ArrowCursor);
        hide();
        
        ColorMenu *menu = new ColorMenu(x - blockWidth / 2, y - blockHeight / 2, blockWidth, getColorAtCursor(x, y));
        connect(menu, &ColorMenu::copyColor, this, &Picker::copyColor, Qt::QueuedConnection);
        connect(menu, &ColorMenu::exit, this, &Picker::exit, Qt::QueuedConnection);
        menu->show();
        
        QTimer::singleShot(10, menu, &ColorMenu::showMenu);
    }
}

QColor Picker::getColorAtCursor(int x, int y)
{
    QImage img = screenPixmap.copy(x, y, 1, 1).toImage();
    return QColor(img.pixel(0, 0));
}
