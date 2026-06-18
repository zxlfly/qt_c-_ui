#include "circleselectdemopage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
#include <QGroupBox>
#include <QCheckBox>
#include <QMouseEvent>
#include <QApplication>
#include <QScrollBar>

const QStringList CircleSelectDemoPage::s_options = {
    // 原料类
    "原料A", "原料B", "原料C", "原料D",
    "催化剂", "溶剂", "添加剂", "缓冲剂",
    "稳定剂", "助剂", "混合物", "产物",
    // 介质类
    "冷却水", "蒸汽", "氮气", "空气",
    "压缩空气", "循环水", "脱盐水", "导热油",
    // 工艺类
    "进料", "出料", "回流", "旁路",
    "排放", "采样", "吹扫", "置换",
    // 状态类
    "运行", "停止", "备用", "检修"
};

CircleSelectDemoPage::CircleSelectDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("环形管点击选择（4实例）", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 提示 + 开关
    auto *hint = new QLabel("点击小管子弹出选择列表，选择后更新管内文字", this);
    hint->setStyleSheet("color: #757575; font-size: 12px; padding-bottom: 4px;");

    auto *topBar = new QHBoxLayout();
    auto *selectCheck = new QCheckBox("开启点击选择", this);
    selectCheck->setChecked(true);
    selectCheck->setStyleSheet("font-size: 13px; color: #424242;");
    topBar->addWidget(selectCheck);
    topBar->addWidget(hint);
    topBar->addStretch();
    mainLayout->addLayout(topBar);

    // 2x2 网格放4个 CircleDisplayWidget
    auto *grid = new QGridLayout();
    grid->setSpacing(6);
    mainLayout->addLayout(grid, 1);

    const QStringList titles = {"管组 A", "管组 B", "管组 C", "管组 D"};
    for (int i = 0; i < 4; ++i) {
        auto *gb = new QGroupBox(titles[i], this);
        auto *vlay = new QVBoxLayout(gb);
        vlay->setContentsMargins(4, 4, 4, 4);

        auto *cw = new CircleDisplayWidget(gb);
        cw->setSelectEnabled(true);

        // 初始文字
        for (int j = 0; j < 12; ++j) {
            cw->setItemText(j, s_options[j % s_options.size()]);
        }
        vlay->addWidget(cw);

        CircleState cs;
        cs.widget = cw;
        cs.group  = gb;
        m_circles.append(cs);

        // 连接点击信号：用 lambda 捕获控件索引 i
        connect(cw, &CircleDisplayWidget::itemClicked, this,
                [this, i](int index) { onCircleItemClicked(index); });

        grid->addWidget(gb, i / 2, i % 2);
    }

    // 弹出列表（QFrame + QListWidget），初始隐藏
    m_popupFrame = new QFrame(this);
    m_popupFrame->setFrameShape(QFrame::StyledPanel);
    m_popupFrame->setStyleSheet(R"(
        QFrame {
            background: white;
            border: 1px solid #BDBDBD;
            border-radius: 4px;
        }
    )");
    m_popupFrame->raise();
    m_popupFrame->hide();

    auto *popupLayout = new QVBoxLayout(m_popupFrame);
    popupLayout->setContentsMargins(2, 2, 2, 2);

    m_listWidget = new QListWidget(m_popupFrame);
    m_listWidget->setFixedWidth(140);
    m_listWidget->setMinimumHeight(200);
    m_listWidget->setVerticalScrollMode(QAbstractItemView::ScrollPerPixel);
    m_listWidget->setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    m_listWidget->setStyleSheet(R"(
        QListWidget {
            border: none;
            background: transparent;
            outline: none;
            font-size: 13px;
        }
        QListWidget::item {
            padding: 6px 16px;
            color: #424242;
        }
        QListWidget::item:hover {
            background: #E3F2FD;
            color: #1565C0;
        }
        QListWidget::item:selected {
            background: #BBDEFB;
            color: #1565C0;
        }
    )");
    for (const QString &opt : s_options) {
        m_listWidget->addItem(opt);
    }
    popupLayout->addWidget(m_listWidget);

    // 使用 lambda 连接，确保直接拿到点击的 item
    connect(m_listWidget, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *item) {
        if (m_currentIndex < 0 || m_currentCircle < 0 || !item) return;
        int itemIdx = m_currentIndex;
        int circleIdx = m_currentCircle;
        QString text = item->text();
        hidePopup();                        // 先关弹窗
        m_circles[circleIdx].widget->setItemText(itemIdx, text);
        m_circles[circleIdx].widget->repaint();  // 强制重绘
    });

    // 全局事件过滤器：仅用于检测弹出框外点击关闭，绝不消费事件
    qApp->installEventFilter(this);

    // 开关联动
    connect(selectCheck, &QCheckBox::toggled, this, [this](bool checked) {
        for (const CircleState &cs : m_circles) {
            cs.widget->setSelectEnabled(checked);
        }
        if (!checked) hidePopup();
    });
}

bool CircleSelectDemoPage::eventFilter(QObject *watched, QEvent *event)
{
    if (m_popupFrame->isVisible() && event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        QPoint globalPos = mouseEvent->globalPosition().toPoint();
        QPoint popupLocal = m_popupFrame->mapFromGlobal(globalPos);

        if (!m_popupFrame->rect().contains(popupLocal)) {
            hidePopup();
        }
    }
    return false;
}

void CircleSelectDemoPage::onCircleItemClicked(int index)
{
    // 找到是哪个 CircleDisplayWidget 发出的信号
    CircleDisplayWidget *sender = qobject_cast<CircleDisplayWidget *>(this->sender());
    int circleIdx = -1;
    for (int i = 0; i < m_circles.size(); ++i) {
        if (m_circles[i].widget == sender) {
            circleIdx = i;
            break;
        }
    }

    if (index < 0 || circleIdx < 0) {
        hidePopup();
        return;
    }
    showPopup(circleIdx, index);
}

void CircleSelectDemoPage::showPopup(int circleIdx, int itemIdx)
{
    m_currentCircle = circleIdx;
    m_currentIndex = itemIdx;

    CircleDisplayWidget *cw = m_circles[circleIdx].widget;

    // 定位到小圆旁边
    QPointF center = cw->itemCenter(itemIdx);
    QPoint popupPos = cw->mapToParent(center.toPoint());

    // 先调整弹出框大小
    m_popupFrame->adjustSize();

    // 偏移到小圆右侧，避免遮挡
    popupPos += QPoint(30, -m_popupFrame->height() / 2);

    // 转换到本页面坐标（因为小管可能在 GroupBox 内多层嵌套）
    QPoint pagePos = cw->parentWidget()->mapToParent(popupPos);

    // 确保不超出父控件
    int maxX = width() - m_popupFrame->width() - 4;
    int maxY = height() - m_popupFrame->height() - 4;
    pagePos.setX(qBound(4, pagePos.x(), maxX));
    pagePos.setY(qBound(4, pagePos.y(), maxY));

    m_popupFrame->move(pagePos);
    m_popupFrame->show();
    m_popupFrame->raise();

    m_listWidget->clearSelection();
    m_listWidget->setFocus();
}

void CircleSelectDemoPage::hidePopup()
{
    m_popupFrame->hide();
    m_currentIndex = -1;
    m_currentCircle = -1;
}
