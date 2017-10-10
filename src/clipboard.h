#ifndef CLIPBOARD_H
#define CLIPBOARD_H

#include <QObject>
#include <QColor>

class Clipboard : public QObject
{
    Q_OBJECT
    
public:
    Clipboard(QObject *parent=0);
	~Clipboard(); 
    
public slots:
    void copyToClipboard(QColor color, QString colorString);
};	

#endif
