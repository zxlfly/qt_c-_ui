#include "flowwidget.h"

#include <QPainter>
#include <QPainterPath>
#include <QFontMetrics>
#include <QtMath>
#include <algorithm>

FlowWidget::FlowWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(400, 200);
}

// --- 公开 API ---

void FlowWidget::setFlowData(const FlowData &data)
{
    m_data = data;
    update();
}

void FlowWidget::updateNodeState(const QString &id, NodeState state)
{
    FlowNode *node = findNode(id);
    if (node) {
        node->state = state;
        update();
    }
}

void FlowWidget::updateEdgeState(const QString &fromId, const QString &toId, EdgeState state)
{
    FlowEdge *edge = findEdge(fromId, toId);
    if (edge) {
        edge->state = state;
        update();
    }
}

void FlowWidget::updateNodeText(const QString &id, const QString &text)
{
    FlowNode *node = findNode(id);
    if (node) {
        node->text = text;
        update();
    }
}

void FlowWidget::updateBranchState(const QString &id, EdgeState state)
{
    for (FlowBranch &branch : m_data.branches) {
        if (branch.id == id) {
            branch.state = state;
            update();
            break;
        }
    }
}

void FlowWidget::setArrowVisible(bool visible)
{
    m_arrowVisible = visible;
    update();
}

bool FlowWidget::arrowVisible() const
{
    return m_arrowVisible;
}

const FlowData &FlowWidget::flowData() const
{
    return m_data;
}

// --- 绘制 ---

void FlowWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    drawBranches(painter);
    drawEdges(painter);
    drawNodes(painter);
    drawLabels(painter);
}

// --- 分支连线（带拐角的树形连接） ---

void FlowWidget::drawBranches(QPainter &painter)
{
    painter.setBrush(Qt::NoBrush);

    for (const FlowBranch &branch : m_data.branches) {
        const FlowNode *stemNode = findNodeConst(branch.stemId);
        if (!stemNode) continue;

        QPen pen(edgeColor(branch.state), 2.0);
        if (branch.state == EdgeState::Active) {
            pen.setStyle(Qt::DashLine);
        }
        painter.setPen(pen);

        // 收集分支节点的像素坐标和半径
        struct BranchTarget {
            const FlowNode *node;
            double pixelY;
            double pixelR;
        };
        QVector<BranchTarget> targets;
        for (const QString &bid : branch.branchIds) {
            const FlowNode *n = findNodeConst(bid);
            if (!n) continue;
            double r = toPixelSize(effectiveRadius(*n));
            targets.append({n, toPixel(n->x, n->y).y(), r});
        }
        if (targets.isEmpty()) continue;

        // 按 y 坐标排序，方便画竖线
        std::sort(targets.begin(), targets.end(),
                  [](const BranchTarget &a, const BranchTarget &b) {
                      return a.pixelY < b.pixelY;
                  });

        QPointF stemPx = toPixel(stemNode->x, stemNode->y);
        double stemR    = toPixelSize(effectiveRadius(*stemNode));

        // 计算拐角竖线的 x 像素位置：取 stem 和 branch 节点的中间
        double branchMinX = 1.0, branchMaxX = 0.0;
        for (const auto &t : targets) {
            if (t.node->x < branchMinX) branchMinX = t.node->x;
            if (t.node->x > branchMaxX) branchMaxX = t.node->x;
        }
        double midRelX = (stemNode->x + branchMinX) / 2.0;
        double colX = toPixel(midRelX, 0).x();

        if (!branch.isMerge) {
            // Split: stem → branches
            // 1) 水平线：stem 右边缘 → colX
            QPointF hStart(stemPx.x() + stemR, stemPx.y());
            painter.drawLine(hStart, QPointF(colX, stemPx.y()));

            // 2) 竖线：从 stem y → 最远 branch y
            double topY = qMin(stemPx.y(), targets.first().pixelY);
            double botY = qMax(stemPx.y(), targets.last().pixelY);
            painter.drawLine(QPointF(colX, topY), QPointF(colX, botY));

            // 3) 水平线：colX → 每个 branch 左边缘 + 箭头
            for (const auto &t : targets) {
                QPointF branchCenter = toPixel(t.node->x, t.node->y);
                QPointF hEnd(branchCenter.x() - t.pixelR, t.pixelY);
                painter.drawLine(QPointF(colX, t.pixelY), hEnd);

                // 箭头
                if (m_arrowVisible)
                    drawArrow(painter, hEnd, QPointF(colX, t.pixelY));
            }
        } else {
            // Merge: branches → stem
            // 1) 水平线：每个 branch 右边缘 → colX
            for (const auto &t : targets) {
                QPointF branchCenter = toPixel(t.node->x, t.node->y);
                QPointF hStart(branchCenter.x() + t.pixelR, t.pixelY);
                painter.drawLine(hStart, QPointF(colX, t.pixelY));
            }

            // 2) 竖线：从最远 branch y → stem y
            double topY = qMin(stemPx.y(), targets.first().pixelY);
            double botY = qMax(stemPx.y(), targets.last().pixelY);
            painter.drawLine(QPointF(colX, topY), QPointF(colX, botY));

            // 3) 水平线：colX → stem 左边缘 + 箭头
            QPointF hEnd(stemPx.x() - stemR, stemPx.y());
            painter.drawLine(QPointF(colX, stemPx.y()), hEnd);

            // 箭头
            if (m_arrowVisible)
                drawArrow(painter, hEnd, QPointF(colX, stemPx.y()));
        }
    }
}

void FlowWidget::drawArrow(QPainter &painter, const QPointF &tip, const QPointF &from)
{
    QLineF arrowLine(tip, from);
    arrowLine.setLength(10);
    double arrowAngle = 25.0;
    QLineF left  = arrowLine;
    QLineF right = arrowLine;
    left.setAngle(arrowLine.angle() + arrowAngle);
    right.setAngle(arrowLine.angle() - arrowAngle);

    painter.drawLine(tip, left.p2());
    painter.drawLine(tip, right.p2());
}

// --- 普通连线 ---

void FlowWidget::drawEdges(QPainter &painter)
{
    for (const FlowEdge &edge : m_data.edges) {
        // 跳过属于分支的边
        if (isBranchEdge(edge.fromId, edge.toId))
            continue;

        const FlowNode *fromNode = findNodeConst(edge.fromId);
        const FlowNode *toNode   = findNodeConst(edge.toId);
        if (!fromNode || !toNode) continue;

        QPointF from = toPixel(fromNode->x, fromNode->y);
        QPointF to   = toPixel(toNode->x, toNode->y);

        double fromR = toPixelSize(effectiveRadius(*fromNode));
        double toR   = toPixelSize(effectiveRadius(*toNode));

        // 计算方向，从圆边缘开始/结束
        QLineF line(from, to);
        double angle = qRadiansToDegrees(qAtan2(line.dy(), line.dx()));

        QPointF fromEdge = from + QPointF(fromR * qCos(qDegreesToRadians(angle)),
                                           fromR * qSin(qDegreesToRadians(angle)));
        QPointF toEdge   = to   - QPointF(toR   * qCos(qDegreesToRadians(angle)),
                                           toR   * qSin(qDegreesToRadians(angle)));

        // 画线
        QPen pen(edgeColor(edge.state), 2.0);
        if (edge.state == EdgeState::Active) {
            pen.setStyle(Qt::DashLine);
        }
        painter.setPen(pen);
        painter.drawLine(fromEdge, toEdge);

        // 画箭头
        if (m_arrowVisible)
            drawArrow(painter, toEdge, fromEdge);
    }
}

// --- 节点圆形 ---

void FlowWidget::drawNodes(QPainter &painter)
{
    for (const FlowNode &node : m_data.nodes) {
        QPointF center = toPixel(node.x, node.y);
        double r = toPixelSize(effectiveRadius(node));

        // 填充
        painter.setPen(Qt::NoPen);
        painter.setBrush(nodeFillColor(node.state));
        painter.drawEllipse(center, r, r);

        // 边框
        QPen pen(nodeBorderColor(node.state), 2.0);
        painter.setPen(pen);
        painter.setBrush(Qt::NoBrush);
        painter.drawEllipse(center, r, r);
    }
}

// --- 文字标签 ---

void FlowWidget::drawLabels(QPainter &painter)
{
    QFont font = labelFont();
    painter.setFont(font);
    QFontMetrics fm(font);

    for (const FlowNode &node : m_data.nodes) {
        QPointF center = toPixel(node.x, node.y);
        double r = toPixelSize(effectiveRadius(node));

        QRect textRect = fm.boundingRect(node.text);
        int textW = textRect.width();
        int textH = textRect.height();

        double gap = 4;  // 文字与圆边缘间距（像素）
        QPointF textCenter;

        switch (node.labelPos) {
        case LabelPosition::Top:
            textCenter = QPointF(center.x(), center.y() - r - gap - textH / 2.0);
            break;
        case LabelPosition::Bottom:
            textCenter = QPointF(center.x(), center.y() + r + gap + textH / 2.0);
            break;
        case LabelPosition::Left:
            textCenter = QPointF(center.x() - r - gap - textW / 2.0, center.y());
            break;
        case LabelPosition::Right:
            textCenter = QPointF(center.x() + r + gap + textW / 2.0, center.y());
            break;
        }

        QRectF drawRect(textCenter.x() - textW / 2.0,
                        textCenter.y() - textH / 2.0,
                        textW, textH);

        painter.setPen(QColor("#424242"));
        painter.drawText(drawRect, Qt::AlignCenter, node.text);
    }
}

// --- 工具方法 ---

QPointF FlowWidget::toPixel(double rx, double ry) const
{
    return QPointF(rx * width(), ry * height());
}

double FlowWidget::toPixelSize(double relativeSize) const
{
    return relativeSize * qMin(width(), height());
}

double FlowWidget::effectiveRadius(const FlowNode &node) const
{
    return (node.radius > 0.0) ? node.radius : m_data.defaultRadius;
}

QColor FlowWidget::nodeFillColor(NodeState state) const
{
    switch (state) {
    case NodeState::Idle:    return QColor("#E0E0E0");
    case NodeState::Running: return QColor("#E3F2FD");
    case NodeState::Success: return QColor("#E8F5E9");
    case NodeState::Failed:  return QColor("#FFEBEE");
    }
    return QColor("#E0E0E0");
}

QColor FlowWidget::nodeBorderColor(NodeState state) const
{
    switch (state) {
    case NodeState::Idle:    return QColor("#BDBDBD");
    case NodeState::Running: return QColor("#2196F3");
    case NodeState::Success: return QColor("#4CAF50");
    case NodeState::Failed:  return QColor("#F44336");
    }
    return QColor("#BDBDBD");
}

QColor FlowWidget::edgeColor(EdgeState state) const
{
    switch (state) {
    case EdgeState::Idle:   return QColor("#BDBDBD");
    case EdgeState::Active: return QColor("#2196F3");
    case EdgeState::Done:   return QColor("#4CAF50");
    case EdgeState::Error:  return QColor("#F44336");
    }
    return QColor("#BDBDBD");
}

QFont FlowWidget::labelFont() const
{
    QFont font;
    font.setPointSize(9);
    return font;
}

const FlowNode *FlowWidget::findNodeConst(const QString &id) const
{
    for (const FlowNode &node : m_data.nodes) {
        if (node.id == id)
            return &node;
    }
    return nullptr;
}

FlowNode *FlowWidget::findNode(const QString &id)
{
    for (FlowNode &node : m_data.nodes) {
        if (node.id == id)
            return &node;
    }
    return nullptr;
}

FlowEdge *FlowWidget::findEdge(const QString &fromId, const QString &toId)
{
    for (FlowEdge &edge : m_data.edges) {
        if (edge.fromId == fromId && edge.toId == toId)
            return &edge;
    }
    return nullptr;
}

bool FlowWidget::isBranchEdge(const QString &fromId, const QString &toId) const
{
    for (const FlowBranch &branch : m_data.branches) {
        if (!branch.isMerge) {
            // Split: stemId → branchIds
            if (fromId == branch.stemId && branch.branchIds.contains(toId))
                return true;
        } else {
            // Merge: branchIds → stemId
            if (branch.branchIds.contains(fromId) && toId == branch.stemId)
                return true;
        }
    }
    return false;
}
