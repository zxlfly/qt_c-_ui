#ifndef CIRCLEROTATEDEMOPAGE_H
#define CIRCLEROTATEDEMOPAGE_H

#include <QWidget>
#include <QPropertyAnimation>
#include "circledisplaywidget.h"

class QPushButton;
class QLabel;

class CircleRotateDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit CircleRotateDemoPage(QWidget *parent = nullptr);

private slots:
    void onRotateForward();
    void onRotateBackward();
    void onReset();
    void onRotationFinished();

private:
    CircleDisplayWidget *m_circleWidget;
    QPropertyAnimation   *m_animation = nullptr;
    QLabel               *m_infoLabel = nullptr;
    int                   m_currentStep = 0;  // 当前步数（0~11，对应倒三角指向的小管编号）
    bool                  m_animating = false;

    void updateInfoLabel();
};

#endif // CIRCLEROTATEDEMOPAGE_H
