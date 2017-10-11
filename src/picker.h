#ifndef PICKER_H
#define PICKER_H

#include <QWidget>
#include <QMenu>
#include "colormenu.h"
#include "animation.h"
#include <QAction>
#include <QTimer>

class Picker : public QWidget
{
    Q_OBJECT
    
public:
    Picker(QWidget *parent=0);
	~Picker(); 
             
    QColor getColorAtCursor(int x, int y);
                                         
signals:
    void copyColor(QColor color, QString colorType);
    void exit();
                 
public slots:
    void handleLeftButtonPress(int x, int y);
    void handleMouseMove(int x, int y);
    void handleRightButtonRelease(int x, int y);
    void popupColorMenu();
    void updateScreenshot();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    Animation *animation;
    ColorMenu *menu;
    QPixmap *cursorPixmap;
    QPixmap screenPixmap;
    QPixmap screenshotPixmap;
    QTimer *updateScreenshotTimer;
    bool displayCursorDot;
    int blockHeight;
    int blockWidth;
    int cursorX;
    int cursorY;
    int height;
    int screenshotHeight;
    int screenshotWidth;
    int width;
    int windowHeight;
    int windowWidth;
};	

#endif
