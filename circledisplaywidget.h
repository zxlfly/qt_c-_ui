#ifndef CIRCLEDISPLAYWIDGET_H
#define CIRCLEDISPLAYWIDGET_H

#include <QWidget>
#include <QColor>

class CircleDisplayWidget : public QWidget
{
    Q_OBJECT

public:
    explicit CircleDisplayWidget(QWidget *parent = nullptr);

    // 设置/更新某一个小管子的文字（index: 0~11）
    void setItemText(int index, const QString &text);

    // 一次性设置所有文字（列表长度应为 12）
    void setAllTexts(const QStringList &texts);

    // 设置某一小管子的背景填充色（可选）
    void setItemFillColor(int index, const QColor &color);

    // 设置大圆边框颜色
    void setOuterBorderColor(const QColor &color);

    // 设置小圆边框颜色
    void setInnerBorderColor(const QColor &color);

    // 设置文字颜色
    void setTextColor(const QColor &color);

    // 控制是否绘制顶部倒三角形
    void setHandleVisible(bool visible);
    bool handleVisible() const;

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;

private:
    struct ItemData {
        QString text = "名称";
        QColor fillColor = QColor("#F5F5F5");
    };

    // 布局计算结果
    struct Layout {
        double farR;
        double nearR;
        double smallR;
        double penW;  // 统一笔宽
    };

    QVector<ItemData> m_items;     // 12个
    bool m_handleVisible = true;   // 是否绘制顶部倒三角形
    QColor m_outerBorderColor = QColor("#424242");
    QColor m_innerBorderColor = QColor("#424242");
    QColor m_textColor = QColor("#424242");
    QColor m_labelColor = QColor("#424242");

    // 绘制辅助
    void drawHandle(QPainter &painter, const QPointF &center, double outerR, const Layout &layout);
    void drawConnectingArcs(QPainter &painter, const QPointF &center, const Layout &layout);
    void drawItems(QPainter &painter, const QPointF &center, const Layout &layout);

    // 布局计算：从 outerR 和 nearR 推算 farR、smallR
    // 约束1：远圆与大圆内切 → farR + smallR = outerR
    // 约束2：相邻小圆外切 → 圆心距 = 2 * smallR
    Layout computeLayout(double outerR) const;
};

#endif // CIRCLEDISPLAYWIDGET_H
