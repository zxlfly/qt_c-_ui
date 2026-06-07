#ifndef FLOWDEMOPAGE_H
#define FLOWDEMOPAGE_H

#include <QWidget>
#include "flowwidget.h"

class FlowDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlowDemoPage(QWidget *parent = nullptr);

private slots:
    void onSwitchTemplate();
    void onStepForward();

private:
    FlowWidget *m_flowWidget;
    int m_currentTemplate = 0;
    int m_stepIndex = 0;
};

#endif // FLOWDEMOPAGE_H
