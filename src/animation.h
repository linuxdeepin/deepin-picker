#ifndef ANIMATION_H
#define ANIMATION_H

#include <QWidget>
#include <QPixmap>
#include <QTimer>
#include <QPaintEvent>

class Animation : public QWidget
{
    Q_OBJECT
    
public:
    Animation(int x, int y, QPixmap pixmap, QColor color, QWidget *parent=0);
	~Animation(); 
    
public slots:
    void renderAnimation();
    
signals:
    void finish(int x, int y, QColor color);
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    QPixmap screenshotPixmap;
    QColor cursorColor;
    int cursorX;
    int cursorY;
    
    QTimer *renderTimer;
    int animationDuration;
    int animationFrames;
    
    int blockWidth;
    int blockHeight;
    
    int width;
    int height;
    
    int renderTicker;
};	

#endif
