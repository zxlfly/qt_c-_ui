#ifndef CIRCLEDISPLAYWIDGET_H
#define CIRCLEDISPLAYWIDGET_H

#include <QWidget>
#include <QColor>

class CircleDisplayWidget : public QWidget
{
    Q_OBJECT
    Q_PROPERTY(double rotationOffset READ rotationOffset WRITE setRotationOffset NOTIFY rotationOffsetChanged)

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

    // 控制是否开启点击选择功能（关闭后点击小圆不发射 itemClicked 信号）
    void setSelectEnabled(bool enabled);
    bool selectEnabled() const;

    // 旋转偏移（度数），大管+小管整体旋转，倒三角位置不变
    void setRotationOffset(double degrees);
    double rotationOffset() const;

    // 获取某个小圆的中心像素坐标（用于弹出菜单定位）
    QPointF itemCenter(int index) const;

    // 获取某个小管的文字
    QString itemText(int index) const;

signals:
    // 点击了第 index 个小圆（0~11），-1 表示没点中任何小圆
    void itemClicked(int index);

    // 旋转偏移变化
    void rotationOffsetChanged(double degrees);

protected:
    void paintEvent(QPaintEvent *) override;
    void resizeEvent(QResizeEvent *) override;
    void mousePressEvent(QMouseEvent *event) override;
    QSize sizeHint() const override;

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
    bool m_selectEnabled = false;  // 是否开启点击选择（默认关闭）
    double m_rotationOffset = 0.0; // 旋转偏移角度（度数）
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

    // 命中测试：给定像素坐标，返回点中的小圆索引（-1=无）
    int hitTest(const QPointF &pos) const;
};

#endif // CIRCLEDISPLAYWIDGET_H
