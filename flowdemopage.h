#ifndef FLOWDEMOPAGE_H
#define FLOWDEMOPAGE_H

#include <QWidget>
#include <QStackedWidget>
#include "flowwidget.h"

class FlowDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit FlowDemoPage(QWidget *parent = nullptr);

private slots:
    void onStepForward();

private:
    // 每个流程控件的状态
    struct FlowState {
        FlowWidget *widget  = nullptr;
        int templateId      = 0;   // 0=A, 1=B, 2=C, 3=D
        int stepIndex       = 0;
    };

    // 每个堆叠页面的状态
    struct PageState {
        QWidget *page = nullptr;
        QList<FlowState> flows;
    };

    QStackedWidget *m_stack;
    QList<PageState> m_pages;
    int m_currentPage = 0;

    // 创建一个堆叠页面（内含4个流程控件，2x2 网格）
    QWidget *createPage(const QStringList &names,
                        const QList<int> &templateIds);

    // 对单个流程执行一步推进
    void stepFlow(FlowState &fs);

    // 重置某个流程到初始状态
    void resetFlow(FlowState &fs);

    // 根据模板ID获取初始 FlowData
    static FlowData templateData(int id);

    // 根据模板ID获取总步数
    static int templateStepCount(int id);
};

#endif // FLOWDEMOPAGE_H
