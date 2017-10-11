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
    void handleMouseMove(int x, int y);
    void handleLeftButtonPress(int x, int y);
    void handleRightButtonRelease(int x, int y);
    void updateScreenshot();
    void popupColorMenu();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    int width;
    int height;
    
    int windowWidth;
    int windowHeight;
    
    int screenshotWidth;
    int screenshotHeight;
    
    QPixmap screenPixmap;
    QPixmap screenshotPixmap;
    
    int cursorX;
    int cursorY;
    
    int blockWidth;
    int blockHeight;
    QTimer *updateScreenshotTimer;
    
    Animation *animation;
    bool displayCursorDot;
    ColorMenu *menu;
};	

#endif
