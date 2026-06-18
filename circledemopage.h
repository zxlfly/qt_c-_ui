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
    QList<CircleDisplayWidget *> m_widgets;  // 4个实例
};

#endif // CIRCLEDEMOPAGE_H
