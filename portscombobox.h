#ifndef PORTSCOMOBBOX_H
#define PORTSCOMOBBOX_H

#include <QObject>
#include <QWidget>
#include <QComboBox>

class PortsComboBox: public QComboBox
{
    Q_OBJECT

signals:
    void popupShowed();

public:
    PortsComboBox(){}
    PortsComboBox(QWidget* parent): QComboBox(parent){}

    virtual ~PortsComboBox(){}
    void showPopup(){
        emit popupShowed();
        QComboBox::showPopup();
    }
};

#endif // PORTSCOMBOBOX_H
