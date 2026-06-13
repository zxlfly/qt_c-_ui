#ifndef CIRCLESELECTDEMOPAGE_H
#define CIRCLESELECTDEMOPAGE_H

#include <QWidget>
#include "circledisplaywidget.h"

class QFrame;
class QListWidget;
class QListWidgetItem;

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
    CircleDisplayWidget *m_circleWidget;
    QFrame              *m_popupFrame = nullptr;
    QListWidget         *m_listWidget = nullptr;
    int                  m_currentIndex = -1;

    void showPopup(int index);
    void hidePopup();

    static const QStringList s_options;
};

#endif // CIRCLESELECTDEMOPAGE_H
