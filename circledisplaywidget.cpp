#include "circledisplaywidget.h"

#include <QPainter>
#include <QFontMetrics>
#include <QtMath>
#include <QPainterPath>
#include <QMouseEvent>

CircleDisplayWidget::CircleDisplayWidget(QWidget *parent)
    : QWidget(parent)
{
    setMinimumSize(300, 300);
    m_items.resize(12);
    for (int i = 0; i < 12; ++i) {
        m_items[i].text = "名称";
    }
}

// --- 公开 API ---

void CircleDisplayWidget::setItemText(int index, const QString &text)
{
    if (index < 0 || index >= 12) return;
    m_items[index].text = text;
    update();
}

void CircleDisplayWidget::setAllTexts(const QStringList &texts)
{
    for (int i = 0; i < qMin(texts.size(), 12); ++i) {
        m_items[i].text = texts[i];
    }
    update();
}

void CircleDisplayWidget::setItemFillColor(int index, const QColor &color)
{
    if (index < 0 || index >= 12) return;
    m_items[index].fillColor = color;
    update();
}

void CircleDisplayWidget::setOuterBorderColor(const QColor &color)
{
    m_outerBorderColor = color;
    update();
}

void CircleDisplayWidget::setInnerBorderColor(const QColor &color)
{
    m_innerBorderColor = color;
    update();
}

void CircleDisplayWidget::setTextColor(const QColor &color)
{
    m_textColor = color;
    update();
}

void CircleDisplayWidget::setHandleVisible(bool visible)
{
    m_handleVisible = visible;
    update();
}

bool CircleDisplayWidget::handleVisible() const
{
    return m_handleVisible;
}

void CircleDisplayWidget::setSelectEnabled(bool enabled)
{
    m_selectEnabled = enabled;
}

bool CircleDisplayWidget::selectEnabled() const
{
    return m_selectEnabled;
}

void CircleDisplayWidget::setRotationOffset(double degrees)
{
    if (qFuzzyCompare(m_rotationOffset, degrees)) return;
    m_rotationOffset = degrees;
    update();
    emit rotationOffsetChanged(degrees);
}

double CircleDisplayWidget::rotationOffset() const
{
    return m_rotationOffset;
}

// --- 绘制 ---

void CircleDisplayWidget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing, true);

    QPointF center(width() / 2.0, height() / 2.0);
    double minSide = qMin(width(), height());
    double outerR  = minSide / 2.0 * 0.90;   // 大圆半径（留边距）
    double penW    = minSide * 0.004;        // 大圆线宽

    // 统一计算布局（只算一次，确保所有绘制用同一套参数）
    Layout layout = computeLayout(outerR);

    // 统一笔：弧线和小圆边框用完全相同的 pen
    QPen sharedPen(m_innerBorderColor, layout.penW);

    // 画大圆边框（可能更粗，与内部无关）
    QPen outerPen(m_outerBorderColor, qMax(penW, 1.5));
    painter.setPen(outerPen);
    painter.setBrush(Qt::NoBrush);
    painter.drawEllipse(center, outerR, outerR);

    // 画顶部把手
    if (m_handleVisible)
        drawHandle(painter, center, outerR, layout);

    // 画小圆之间的外侧连接弧线
    drawConnectingArcs(painter, center, layout);

    // 画12个小圆及编号
    drawItems(painter, center, layout);
}

void CircleDisplayWidget::resizeEvent(QResizeEvent *)
{
    update();
}

// --- 命中测试 ---

int CircleDisplayWidget::hitTest(const QPointF &pos) const
{
    QPointF center(width() / 2.0, height() / 2.0);
    double minSide = qMin(width(), height());
    double outerR  = minSide / 2.0 * 0.90;
    Layout layout  = computeLayout(outerR);

    for (int i = 0; i < 12; ++i) {
        double angleDeg = 270.0 + i * 30.0 + m_rotationOffset;
        double angleRad = qDegreesToRadians(angleDeg);
        double innerR = (i % 2 == 0) ? layout.farR : layout.nearR;

        double cx = center.x() + innerR * qCos(angleRad);
        double cy = center.y() + innerR * qSin(angleRad);
        QPointF itemCenter(cx, cy);

        double dx = pos.x() - itemCenter.x();
        double dy = pos.y() - itemCenter.y();
        if (dx * dx + dy * dy <= layout.smallR * layout.smallR) {
            return i;
        }
    }
    return -1;
}

QPointF CircleDisplayWidget::itemCenter(int index) const
{
    if (index < 0 || index >= 12) return QPointF();

    QPointF centerPt(width() / 2.0, height() / 2.0);
    double minSide = qMin(width(), height());
    double outerR  = minSide / 2.0 * 0.90;
    Layout layout  = computeLayout(outerR);

    double angleDeg = 270.0 + index * 30.0 + m_rotationOffset;
    double angleRad = qDegreesToRadians(angleDeg);
    double innerR = (index % 2 == 0) ? layout.farR : layout.nearR;

    return QPointF(centerPt.x() + innerR * qCos(angleRad),
                   centerPt.y() + innerR * qSin(angleRad));
}

QString CircleDisplayWidget::itemText(int index) const
{
    if (index < 0 || index >= 12) return QString();
    return m_items[index].text;
}

void CircleDisplayWidget::mousePressEvent(QMouseEvent *event)
{
    if (event->button() == Qt::LeftButton && m_selectEnabled) {
        int idx = hitTest(event->position());
        emit itemClicked(idx);
    }
    QWidget::mousePressEvent(event);
}

// --- 顶部把手 ---

void CircleDisplayWidget::drawHandle(QPainter &painter, const QPointF &center, double outerR, const Layout &)
{
    // 倒三角形：顶边宽，底边尖（朝下指向大圆）
    double triW = outerR * 0.28;   // 底边（顶部）宽度
    double triH = outerR * 0.15;   // 三角形高度
    double topY = center.y() - outerR - triH * 0.1;  // 顶部紧贴大圆上方

    QPainterPath path;
    path.moveTo(center.x() - triW / 2.0, topY - triH);    // 左上角
    path.lineTo(center.x() + triW / 2.0, topY - triH);    // 右上角
    path.lineTo(center.x(), topY);                          // 底部尖角（朝下）
    path.closeSubpath();

    painter.setPen(QPen(m_outerBorderColor, 1.5));
    painter.setBrush(QColor("#424242"));
    painter.drawPath(path);
}

// --- 布局计算 ---

CircleDisplayWidget::Layout CircleDisplayWidget::computeLayout(double outerR) const
{
    // ============================================================
    // 可调参数：控制小圆距大圆圆心的距离（相对 outerR 的比例）
    // ============================================================
    double farRatio  = 0.791;   // 远圆圆心距 / outerR
    double nearRatio = 0.55;    // 近圆圆心距 / outerR
    bool tangentToOuter = true; // true=远圆与大圆内切(farR+smallR=outerR)，false=smallR单独算

    double farR  = outerR * farRatio;
    double nearR = outerR * nearRatio;

    double smallR;
    if (tangentToOuter) {
        // 远圆内切大圆：smallR = outerR - farR
        smallR = outerR - farR;
    } else {
        // 仅保证相邻小圆相切：smallR = 相邻圆心距 / 2
        double dist2 = farR * farR + nearR * nearR
                       - 2.0 * farR * nearR * qCos(qDegreesToRadians(30.0));
        smallR = qSqrt(dist2) / 2.0;
    }

    return {farR, nearR, smallR, qMax(outerR * 0.008, 1.5)};
}

// --- 小圆之间的外侧连接弧线 ---

void CircleDisplayWidget::drawConnectingArcs(QPainter &painter, const QPointF &center, const Layout &layout)
{
    double farR   = layout.farR;
    double nearR  = layout.nearR;
    double smallR = layout.smallR;
    double penW   = layout.penW;

    // 计算所有小圆圆心
    QVector<QPointF> centers(12);
    for (int i = 0; i < 12; ++i) {
        double angleDeg = 270.0 + i * 30.0 + m_rotationOffset;
        double angleRad = qDegreesToRadians(angleDeg);
        double innerR = (i % 2 == 0) ? farR : nearR;
        centers[i] = QPointF(center.x() + innerR * qCos(angleRad),
                              center.y() + innerR * qSin(angleRad));
    }

    QPen arcPen(m_innerBorderColor, penW);
    painter.setPen(arcPen);
    painter.setBrush(Qt::NoBrush);

    // 弧线端点向内缩，确保被小圆填充完全覆盖
    double edgeInset = penW / 2.0;

    for (int i = 0; i < 12; ++i) {
        int j = (i + 1) % 12;
        QPointF c1 = centers[i];
        QPointF c2 = centers[j];

        // c1→c2 方向
        QPointF d = c2 - c1;
        double dist = qSqrt(d.x() * d.x() + d.y() * d.y());
        if (dist < 1e-6) continue;
        d = QPointF(d.x() / dist, d.y() / dist);

        // 外法线方向（远离大圆圆心）
        QPointF n(-d.y(), d.x());
        QPointF mid = (c1 + c2) / 2.0;
        if (n.x() * (mid.x() - center.x()) + n.y() * (mid.y() - center.y()) < 0) {
            n = QPointF(-n.x(), -n.y());
        }

        // 起止点：小圆外侧边缘，向内缩 edgeInset
        QPointF startPt = c1 + QPointF((smallR - edgeInset) * n.x(), (smallR - edgeInset) * n.y());
        QPointF endPt   = c2 + QPointF((smallR - edgeInset) * n.x(), (smallR - edgeInset) * n.y());

        // 使用二次贝塞尔曲线（弧线在外侧凸出），再转为三次贝塞尔以获得更好的平滑度
        // Q 是二次贝塞尔控制点：在中点外侧
        QPointF Q = mid + QPointF(smallR * 0.55 * n.x(), smallR * 0.55 * n.y());

        // 二次贝塞尔 → 三次贝塞尔精确转换：
        // cp1 = P0 + (2/3)(Q - P0) = P0/3 + 2Q/3
        // cp2 = P3 + (2/3)(Q - P3) = P3/3 + 2Q/3
        QPointF cp1 = (startPt + 2.0 * Q) / 3.0;
        QPointF cp2 = (endPt   + 2.0 * Q) / 3.0;

        QPainterPath path;
        path.moveTo(startPt);
        path.cubicTo(cp1, cp2, endPt);
        painter.drawPath(path);
    }
}

// --- 小圆与编号 ---

void CircleDisplayWidget::drawItems(QPainter &painter, const QPointF &center, const Layout &layout)
{
    double farR   = layout.farR;
    double nearR  = layout.nearR;
    double smallR = layout.smallR;
    double penW   = layout.penW;
    double outerR = farR + smallR;  // 远圆内切大圆时 outerR = farR + smallR

    QPen circlePen(m_innerBorderColor, penW);
    painter.setPen(circlePen);

    // 字体设置
    QFont textFont;
    textFont.setPointSizeF(qMax(8.0, smallR * 0.35));

    QFont labelFont;
    labelFont.setPointSizeF(qMax(7.0, smallR * 0.30));

    for (int i = 0; i < 12; ++i) {
        // 角度：从12点方向(270°)开始顺时针，每30°一个，加上旋转偏移
        double angleDeg = 270.0 + i * 30.0 + m_rotationOffset;
        double angleRad = qDegreesToRadians(angleDeg);

        // 交替远近：偶数索引远，奇数索引近
        double innerR = (i % 2 == 0) ? farR : nearR;

        double cx = center.x() + innerR * qCos(angleRad);
        double cy = center.y() + innerR * qSin(angleRad);
        QPointF itemCenter(cx, cy);

        // 画小圆（确保用 circlePen）
        painter.setPen(circlePen);
        painter.setBrush(m_items[i].fillColor);
        painter.drawEllipse(itemCenter, smallR, smallR);

        // 画内部文字（每次都要重设字体，因为上一轮循环末尾切换成了 labelFont）
        painter.setFont(textFont);
        painter.setPen(m_textColor);
        QFontMetrics fm(textFont);
        QRect textRect = fm.boundingRect(m_items[i].text);
        QRectF drawRect(itemCenter.x() - textRect.width() / 2.0,
                        itemCenter.y() - textRect.height() / 2.0,
                        textRect.width(), textRect.height());
        painter.drawText(drawRect, Qt::AlignCenter, m_items[i].text);

        // 编号标签位置
        // 远圆(偶数)：标签在内侧（靠近大圆圆心方向）
        // 近圆(奇数)：标签在外侧（远离大圆圆心方向）
        bool isFar = (i % 2 == 0);

        QString label = QString::number(i + 1) + "号";
        painter.setFont(labelFont);
        QFontMetrics fmLabel(labelFont);
        QRect labelRect = fmLabel.boundingRect(label);

        // 标签距离：圆边缘 + 较大间距，确保不重叠
        double labelOffset = smallR + penW * 4 + fmLabel.height() * 1.5;
        double labelR = isFar ? (innerR - labelOffset) : (innerR + labelOffset);

        // 近圆外侧标签：确保不超出大圆（留足文字高度边距）
        if (!isFar) {
            double maxR = outerR - qMax(labelRect.width(), labelRect.height()) * 0.6;
            if (labelR > maxR) {
                labelR = maxR;
            }
        }

        // 标签中心位置
        double labelCx = center.x() + labelR * qCos(angleRad);
        double labelCy = center.y() + labelR * qSin(angleRad);

        // 用 QRectF 居中渲染，精确控制
        QRectF renderRect(labelCx - labelRect.width() / 2.0,
                          labelCy - labelRect.height() / 2.0,
                          labelRect.width(), labelRect.height());

        painter.setPen(m_labelColor);
        painter.drawText(renderRect, Qt::AlignCenter, label);
    }
}
