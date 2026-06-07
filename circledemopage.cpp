#include "circledemopage.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>
#include <QRandomGenerator>

CircleDemoPage::CircleDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("环形管展示", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // CircleDisplayWidget
    m_circleWidget = new CircleDisplayWidget(this);
    mainLayout->addWidget(m_circleWidget, 1);

    // 控制栏
    auto *btnRandom = new QPushButton("随机更新文字", this);
    auto *btnReset  = new QPushButton("重置", this);
    auto *lblHint   = new QLabel("点击按钮更新小管子文字", this);
    lblHint->setStyleSheet("color: #757575;");

    auto *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(btnRandom);
    ctrlLayout->addWidget(btnReset);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(lblHint);
    mainLayout->addLayout(ctrlLayout);

    // 连接信号
    connect(btnRandom, &QPushButton::clicked, this, &CircleDemoPage::onRandomUpdate);
    connect(btnReset,  &QPushButton::clicked, this, &CircleDemoPage::onReset);
}

void CircleDemoPage::onRandomUpdate()
{
    static const QStringList names = {
        "原料A", "原料B", "原料C", "原料D",
        "催化剂", "溶剂", "添加剂", "缓冲剂",
        "稳定剂", "助剂", "混合物", "产物"
    };

    for (int i = 0; i < 12; ++i) {
        int idx = QRandomGenerator::global()->bounded(names.size());
        m_circleWidget->setItemText(i, names[idx]);
    }
}

void CircleDemoPage::onReset()
{
    for (int i = 0; i < 12; ++i) {
        m_circleWidget->setItemText(i, "名称");
    }
}
