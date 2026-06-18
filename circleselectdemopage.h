#ifndef CIRCLESELECTDEMOPAGE_H
#define CIRCLESELECTDEMOPAGE_H

#include <QWidget>
#include <QList>
#include "circledisplaywidget.h"

class QFrame;
class QListWidget;
class QListWidgetItem;
class QGroupBox;

class CircleSelectDemoPage : public QWidget
{
    Q_OBJECT

public:
    explicit CircleSelectDemoPage(QWidget *parent = nullptr);

protected:
    bool eventFilter(QObject *watched, QEvent *event) override;

private slots:
    void onCircleItemClicked(int index);

private:
    // 每个环形管控件的状态
    struct CircleState {
        CircleDisplayWidget *widget = nullptr;
        QGroupBox           *group  = nullptr;
    };

    QList<CircleState> m_circles;     // 4个实例

    // 弹出列表（所有实例共用一个）
    QFrame      *m_popupFrame  = nullptr;
    QListWidget *m_listWidget  = nullptr;
    int          m_currentIndex = -1;          // 当前点击的小管索引 (0~11)
    int          m_currentCircle = -1;          // 当前操作的环形管控件索引 (0~3)

    void showPopup(int circleIdx, int itemIdx);
    void hidePopup();

    static const QStringList s_options;
};

#endif // CIRCLESELECTDEMOPAGE_H
