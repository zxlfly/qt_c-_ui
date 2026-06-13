#include "circleselectdemopage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>
#include <QListWidget>
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
    auto *title = new QLabel("环形管展示 - 点击选择", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 提示
    auto *hint = new QLabel("点击小管子弹出选择列表，选择后更新管内文字", this);
    hint->setStyleSheet("color: #757575; font-size: 12px; padding-bottom: 4px;");

    // 开关 + 提示 水平排列
    auto *topBar = new QHBoxLayout();
    auto *selectCheck = new QCheckBox("开启点击选择", this);
    selectCheck->setChecked(true);
    selectCheck->setStyleSheet("font-size: 13px; color: #424242;");
    topBar->addWidget(selectCheck);
    topBar->addWidget(hint);
    topBar->addStretch();
    mainLayout->addLayout(topBar);

    // CircleDisplayWidget
    m_circleWidget = new CircleDisplayWidget(this);
    m_circleWidget->setSelectEnabled(true);   // 默认开启
    mainLayout->addWidget(m_circleWidget, 1);

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

    // 连接信号
    connect(m_circleWidget, &CircleDisplayWidget::itemClicked,
            this, &CircleSelectDemoPage::onCircleItemClicked);

    // 使用 lambda 连接，确保直接拿到点击的 item
    connect(m_listWidget, &QListWidget::itemClicked,
            this, [this](QListWidgetItem *item) {
        if (m_currentIndex < 0 || !item) return;
        int idx = m_currentIndex;          // 先保存，hidePopup 会重置
        QString text = item->text();
        hidePopup();                        // 先关弹窗，避免遮挡重绘
        m_circleWidget->setItemText(idx, text);
        m_circleWidget->repaint();         // 强制立即重绘
    });

    // 全局事件过滤器：仅用于检测弹出框外点击关闭，绝不消费事件
    qApp->installEventFilter(this);

    // 开关联动
    connect(selectCheck, &QCheckBox::toggled, this, [this](bool checked) {
        m_circleWidget->setSelectEnabled(checked);
        if (!checked) hidePopup();
    });
}

bool CircleSelectDemoPage::eventFilter(QObject *watched, QEvent *event)
{
    if (m_popupFrame->isVisible() && event->type() == QEvent::MouseButtonPress) {
        auto *mouseEvent = static_cast<QMouseEvent *>(event);
        // 用全局坐标判断点击是否在弹出框内部
        QPoint globalPos = mouseEvent->globalPosition().toPoint();
        QPoint popupLocal = m_popupFrame->mapFromGlobal(globalPos);

        if (!m_popupFrame->rect().contains(popupLocal)) {
            // 点击在弹出框外部 → 关闭弹窗
            hidePopup();
        }
        // 关键：始终返回 false，不消费事件，让事件正常传递到目标控件
    }
    return false;
}

void CircleSelectDemoPage::onCircleItemClicked(int index)
{
    if (index < 0) {
        hidePopup();
        return;
    }
    showPopup(index);
}

void CircleSelectDemoPage::showPopup(int index)
{
    m_currentIndex = index;

    // 定位到小圆旁边
    QPointF center = m_circleWidget->itemCenter(index);
    QPoint popupPos = m_circleWidget->mapToParent(center.toPoint());

    // 先调整弹出框大小
    m_popupFrame->adjustSize();

    // 偏移到小圆右侧，避免遮挡
    popupPos += QPoint(30, -m_popupFrame->height() / 2);

    // 确保不超出父控件
    int maxX = width() - m_popupFrame->width() - 4;
    int maxY = height() - m_popupFrame->height() - 4;
    popupPos.setX(qBound(4, popupPos.x(), maxX));
    popupPos.setY(qBound(4, popupPos.y(), maxY));

    m_popupFrame->move(popupPos);
    m_popupFrame->show();
    m_popupFrame->raise();

    // 清空之前的选择
    m_listWidget->clearSelection();
    m_listWidget->setFocus();
}

void CircleSelectDemoPage::hidePopup()
{
    m_popupFrame->hide();
    m_currentIndex = -1;
}
