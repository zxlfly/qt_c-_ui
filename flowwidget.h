#ifndef FLOWWIDGET_H
#define FLOWWIDGET_H

#include <QWidget>
#include "flowdata.h"

class FlowWidget : public QWidget
{
    Q_OBJECT

public:
    explicit FlowWidget(QWidget *parent = nullptr);

    // 设置流程数据
    void setFlowData(const FlowData &data);

    // 运行时更新
    void updateNodeState(const QString &id, NodeState state);
    void updateEdgeState(const QString &fromId, const QString &toId, EdgeState state);
    void updateBranchState(const QString &id, EdgeState state);
    void updateNodeText(const QString &id, const QString &text);

    // 获取当前数据
    const FlowData &flowData() const;

    // 箭头控制
    void setArrowVisible(bool visible);
    bool arrowVisible() const;

protected:
    void paintEvent(QPaintEvent *event) override;
    QSize sizeHint() const override;

private:
    FlowData m_data;
    bool m_arrowVisible = true;  // 是否绘制箭头

    // 绘制各层
    void drawBranches(QPainter &painter);
    void drawEdges(QPainter &painter);
    void drawNodes(QPainter &painter);
    void drawLabels(QPainter &painter);
    void drawArrow(QPainter &painter, const QPointF &tip, const QPointF &from);

    // 工具方法
    QPointF toPixel(double rx, double ry) const;
    double toPixelSize(double relativeSize) const;
    double effectiveRadius(const FlowNode &node) const;
    QColor nodeFillColor(NodeState state) const;
    QColor nodeBorderColor(NodeState state) const;
    QColor edgeColor(EdgeState state) const;
    QFont labelFont() const;

    // 查找
    const FlowNode *findNodeConst(const QString &id) const;
    FlowNode *findNode(const QString &id);
    FlowEdge *findEdge(const QString &fromId, const QString &toId);

    // 判断某条边是否属于某个分支（分支连线单独绘制）
    bool isBranchEdge(const QString &fromId, const QString &toId) const;
};
#endif // FLOWWIDGET_H
