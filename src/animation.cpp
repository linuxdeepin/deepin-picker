#include "animation.h"
#include "utils.h"
#include <QColor>
#include <QPainter>
#include <QDebug>

Animation::Animation(int x, int y, QPixmap pixmap, QColor color, QWidget *parent) : QWidget(parent)
{
    cursorX = x;
    cursorY = y;
    screenshotPixmap = pixmap;
    cursorColor = color;

    blockWidth = 20;
    blockHeight = 20;

    setWindowFlags(Qt::X11BypassWindowManagerHint | Qt::WindowStaysOnTopHint | Qt::FramelessWindowHint | Qt::Tool);
    setAttribute(Qt::WA_TranslucentBackground, true);
    setMouseTracking(true);
    installEventFilter(this);

    width = 220;
    height = 220;

    move(x - width / 2, y - height / 2);
    resize(width, height);
    
    renderTicker = 0;
    animationFrames = 12;
    animationDuration = 25;
    
    renderTimer = new QTimer();
    connect(renderTimer, SIGNAL(timeout()), this, SLOT(renderAnimation()));
    renderTimer->start(animationDuration);
}

Animation::~Animation()
{
}

void Animation::paintEvent(QPaintEvent *)
{
    
    int radius = screenshotPixmap.width() / 2 * (1 - Utils::easeInOut(renderTicker * 1.0 / animationFrames));
    qDebug() << renderTicker << animationFrames << radius << Utils::easeInOut(renderTicker * 1.0 / animationFrames);
    
    int width = rect().width();
    int height = rect().height();

    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);
    
    painter.setOpacity(Utils::easeInOut(renderTicker * 1.0 / animationFrames));

    QPainterPath circlePath;
    circlePath.addEllipse(width / 2 - radius, height / 2 - radius, radius * 2, radius * 2);
    painter.setClipPath(circlePath);
    painter.drawPixmap(width / 2 - radius, height / 2 - radius, screenshotPixmap);
}

void Animation::renderAnimation()
{
    if (renderTicker < animationFrames) {
        renderTicker++;

        repaint();
    } else {
        renderTimer->stop();
        hide();
        
        emit finish(cursorX, cursorY, cursorColor);
    }
}

