#ifndef CIRCLEROTATEDEMOPAGE_H
#define CIRCLEROTATEDEMOPAGE_H

#include <QWidget>
#include <QPropertyAnimation>
#include <QList>
#include "circledisplaywidget.h"

class QPushButton;
class QLabel;
class QGroupBox;

class CircleRotateDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit CircleRotateDemoPage(QWidget *parent = nullptr);

private slots:
    void onRotateForward();
    void onRotateBackward();
    void onReset();

private:
    struct RotateState {
        CircleDisplayWidget *widget = nullptr;
        QPropertyAnimation  *anim   = nullptr;
        QGroupBox           *group  = nullptr;
        QLabel              *label  = nullptr;
        int                  step   = 0;
        bool                 animating = false;
    };

    QList<RotateState> m_circles;  // 4个实例

    void updateInfoLabel(int idx);
};

#endif // CIRCLEROTATEDEMOPAGE_H
