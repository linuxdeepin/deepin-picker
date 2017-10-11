#ifndef COLORMENU_H
#define COLORMENU_H

#include <QWidget>
#include <QMenu>
#include <QAction>
#include <QColor>
#include <QPaintEvent>

class ColorMenu : public QWidget
{
    Q_OBJECT
    
public:
    ColorMenu(int x, int y, int size, QColor color, QWidget *parent=0);
	~ColorMenu(); 
    
signals:
    void copyColor(QColor color, QString colorType);
    void exit();
                                                     
public slots:
    void showMenu();
    void copyRGBColor();
    void copyFloatRGBColor();
    void copyRGBAColor();
    void copyFloatRGBAColor();
    void copyHexColor();
    
protected:
    void paintEvent(QPaintEvent *);
    
private:
    int windowX;
    int windowY;
    int windowSize;
    
    QColor windowColor;

    QMenu *colorMenu;
    QAction *rgbAction;
    QAction *rgbFloatAction;
    QAction *rgbaAction;
    QAction *rgbaFloatAction;
    QAction *hexAction;
    
    int menuOffsetY;
    int menuOffsetX;
    int shadowXMargin;
    int shadowBottomMargin;
    bool clickMenuItem;
};	

#endif
