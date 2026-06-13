#include "circlerotatedemopage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>

CircleRotateDemoPage::CircleRotateDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("环形管展示 - 旋转切换", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 提示
    auto *hint = new QLabel("点击按钮旋转，倒三角指向下一个小管", this);
    hint->setStyleSheet("color: #757575; font-size: 12px; padding-bottom: 4px;");
    mainLayout->addWidget(hint);

    // CircleDisplayWidget
    m_circleWidget = new CircleDisplayWidget(this);
    m_circleWidget->setAllTexts({"原料A", "原料B", "原料C", "原料D",
                                  "催化剂", "溶剂", "添加剂", "缓冲剂",
                                  "稳定剂", "助剂", "混合物", "产物"});
    mainLayout->addWidget(m_circleWidget, 1);

    // 当前指向信息
    m_infoLabel = new QLabel(this);
    m_infoLabel->setStyleSheet("font-size: 13px; color: #1565C0; padding: 4px 0;");
    updateInfoLabel();
    mainLayout->addWidget(m_infoLabel);

    // 按钮区
    auto *btnLayout = new QHBoxLayout();

    auto *btnBackward = new QPushButton("◀ 上一个", this);
    btnBackward->setStyleSheet(R"(
        QPushButton {
            background: #E3F2FD; border: 1px solid #90CAF9;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #1565C0;
        }
        QPushButton:hover { background: #BBDEFB; }
        QPushButton:pressed { background: #90CAF9; }
        QPushButton:disabled { background: #F5F5F5; color: #BDBDBD; border-color: #E0E0E0; }
    )");

    auto *btnReset = new QPushButton("复位", this);
    btnReset->setStyleSheet(R"(
        QPushButton {
            background: #FFF3E0; border: 1px solid #FFCC80;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #E65100;
        }
        QPushButton:hover { background: #FFE0B2; }
        QPushButton:pressed { background: #FFCC80; }
    )");

    auto *btnForward = new QPushButton("下一个 ▶", this);
    btnForward->setStyleSheet(R"(
        QPushButton {
            background: #E3F2FD; border: 1px solid #90CAF9;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #1565C0;
        }
        QPushButton:hover { background: #BBDEFB; }
        QPushButton:pressed { background: #90CAF9; }
        QPushButton:disabled { background: #F5F5F5; color: #BDBDBD; border-color: #E0E0E0; }
    )");

    btnLayout->addWidget(btnBackward);
    btnLayout->addWidget(btnReset);
    btnLayout->addWidget(btnForward);
    mainLayout->addLayout(btnLayout);

    // 动画
    m_animation = new QPropertyAnimation(m_circleWidget, "rotationOffset");
    m_animation->setDuration(400);
    m_animation->setEasingCurve(QEasingCurve::InOutCubic);
    connect(m_animation, &QPropertyAnimation::finished,
            this, &CircleRotateDemoPage::onRotationFinished);

    // 连接按钮
    connect(btnForward, &QPushButton::clicked, this, &CircleRotateDemoPage::onRotateForward);
    connect(btnBackward, &QPushButton::clicked, this, &CircleRotateDemoPage::onRotateBackward);
    connect(btnReset, &QPushButton::clicked, this, &CircleRotateDemoPage::onReset);
}

void CircleRotateDemoPage::onRotateForward()
{
    if (m_animating) return;
    m_animating = true;

    // 下一个小管：旋转 -30°（让下一个小管转到顶部）
    double start = m_circleWidget->rotationOffset();
    double end = start - 30.0;

    m_animation->setStartValue(start);
    m_animation->setEndValue(end);
    m_animation->start();

    m_currentStep = (m_currentStep + 1) % 12;
}

void CircleRotateDemoPage::onRotateBackward()
{
    if (m_animating) return;
    m_animating = true;

    // 上一个小管：旋转 +30°
    double start = m_circleWidget->rotationOffset();
    double end = start + 30.0;

    m_animation->setStartValue(start);
    m_animation->setEndValue(end);
    m_animation->start();

    m_currentStep = (m_currentStep + 11) % 12;  // +11 等同于 -1 (mod 12)
}

void CircleRotateDemoPage::onReset()
{
    if (m_animating) {
        m_animation->stop();
        m_animating = false;
    }

    m_animation->setStartValue(m_circleWidget->rotationOffset());
    m_animation->setEndValue(0.0);
    m_animation->start();
    m_animating = true;

    m_currentStep = 0;
}

void CircleRotateDemoPage::onRotationFinished()
{
    m_animating = false;
    updateInfoLabel();
}

void CircleRotateDemoPage::updateInfoLabel()
{
    // 倒三角当前指向的小管
    int tubeIndex = m_currentStep;
    QString text = QString("当前指向：%1号管 — %2")
                       .arg(tubeIndex + 1)
                       .arg(m_circleWidget->itemText(tubeIndex));
    m_infoLabel->setText(text);
}
