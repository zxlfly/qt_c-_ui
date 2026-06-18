#include "circledemopage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>
#include <QRandomGenerator>

static const QStringList kNames = {
    "原料A", "原料B", "原料C", "原料D",
    "催化剂", "溶剂", "添加剂", "缓冲剂",
    "稳定剂", "助剂", "混合物", "产物"
};

CircleDemoPage::CircleDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("环形管展示（4实例）", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 控制栏
    auto *btnRandom = new QPushButton("全部随机更新", this);
    auto *btnReset  = new QPushButton("全部重置", this);
    auto *lblHint   = new QLabel("点击按钮同时更新4个环形管控件", this);
    lblHint->setStyleSheet("color: #757575;");

    auto *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(btnRandom);
    ctrlLayout->addWidget(btnReset);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(lblHint);
    mainLayout->addLayout(ctrlLayout);

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
        // 初始随机文字
        for (int j = 0; j < 12; ++j) {
            cw->setItemText(j, kNames[QRandomGenerator::global()->bounded(kNames.size())]);
        }
        vlay->addWidget(cw);
        m_widgets.append(cw);

        grid->addWidget(gb, i / 2, i % 2);
    }

    // 连接信号
    connect(btnRandom, &QPushButton::clicked, this, &CircleDemoPage::onRandomUpdate);
    connect(btnReset,  &QPushButton::clicked, this, &CircleDemoPage::onReset);
}

void CircleDemoPage::onRandomUpdate()
{
    for (CircleDisplayWidget *cw : m_widgets) {
        for (int i = 0; i < 12; ++i) {
            int idx = QRandomGenerator::global()->bounded(kNames.size());
            cw->setItemText(i, kNames[idx]);
        }
    }
}

void CircleDemoPage::onReset()
{
    for (CircleDisplayWidget *cw : m_widgets) {
        for (int i = 0; i < 12; ++i) {
            cw->setItemText(i, "名称");
        }
    }
}
