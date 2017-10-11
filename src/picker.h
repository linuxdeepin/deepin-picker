#ifndef PICKER_H
#define PICKER_H

#include <QWidget>
#include <QMenu>
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
    void copyColor(QColor color, QString colorString);
    void exit();
                 
public slots:
    void handleMouseMove(int x, int y);
    void handleLeftButtonPress(int x, int y);
    void handleRightButtonRelease(int x, int y);
    void updateScreenshot();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    int width;
    int height;
    
    int screenshotWidth;
    int screenshotHeight;
    
    QPixmap screenPixmap;
    QPixmap screenshotPixmap;
    
    int cursorX;
    int cursorY;
    
    int blockWidth;
    int blockHeight;

    QTimer *updateScreenshotTimer;
    
    bool displayCursorDot;
};	

#endif
