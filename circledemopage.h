#ifndef CIRCLEDEMOPAGE_H
#define CIRCLEDEMOPAGE_H

#include <QWidget>
#include "circledisplaywidget.h"

class CircleDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit CircleDemoPage(QWidget *parent = nullptr);

private slots:
    void onRandomUpdate();
    void onReset();

private:
    CircleDisplayWidget *m_circleWidget;
};

#endif // CIRCLEDEMOPAGE_H
