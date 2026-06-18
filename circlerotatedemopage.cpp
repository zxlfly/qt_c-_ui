#include "circlerotatedemopage.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QPushButton>
#include <QGroupBox>

CircleRotateDemoPage::CircleRotateDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("环形管旋转切换（4实例）", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 提示
    auto *hint = new QLabel("点击按钮同时旋转全部4个环形管，倒三角指向下一个小管", this);
    hint->setStyleSheet("color: #757575; font-size: 12px; padding-bottom: 4px;");
    mainLayout->addWidget(hint);

    // 2x2 网格放4个 CircleDisplayWidget
    auto *grid = new QGridLayout();
    grid->setSpacing(6);
    mainLayout->addLayout(grid, 1);

    const QStringList titles = {"管组 A", "管组 B", "管组 C", "管组 D"};
    const QList<QStringList> dataSets = {
        {"原料A", "原料B", "原料C", "原料D", "催化剂", "溶剂", "添加剂", "缓冲剂", "稳定剂", "助剂", "混合物", "产物"},
        {"进料", "出料", "回流", "旁路", "排放", "采样", "吹扫", "置换", "冷却水", "蒸汽", "氮气", "空气"},
        {"运行", "停止", "备用", "检修", "原料A", "原料B", "原料C", "原料D", "催化剂", "溶剂", "添加剂", "缓冲剂"},
        {"管1", "管2", "管3", "管4", "管5", "管6", "管7", "管8", "管9", "管10", "管11", "管12"},
    };

    for (int i = 0; i < 4; ++i) {
        auto *gb = new QGroupBox(titles[i], this);
        auto *vlay = new QVBoxLayout(gb);
        vlay->setContentsMargins(4, 4, 4, 4);

        auto *cw = new CircleDisplayWidget(gb);
        cw->setAllTexts(dataSets[i]);
        vlay->addWidget(cw, 1);

        auto *infoLbl = new QLabel(gb);
        infoLbl->setStyleSheet("font-size: 12px; color: #1565C0; padding: 2px 0;");
        vlay->addWidget(infoLbl);

        // 动画
        auto *anim = new QPropertyAnimation(cw, "rotationOffset");
        anim->setDuration(400);
        anim->setEasingCurve(QEasingCurve::InOutCubic);

        RotateState rs;
        rs.widget = cw;
        rs.anim   = anim;
        rs.group  = gb;
        rs.label  = infoLbl;
        rs.step   = 0;
        rs.animating = false;
        m_circles.append(rs);

        grid->addWidget(gb, i / 2, i % 2);

        // 动画结束时更新标签（只连接一次）
        connect(anim, &QPropertyAnimation::finished, this, [this, i]() {
            if (i < m_circles.size()) {
                m_circles[i].animating = false;
                updateInfoLabel(i);
            }
        });

        updateInfoLabel(i);
    }

    // 按钮区
    auto *btnLayout = new QHBoxLayout();

    auto *btnBackward = new QPushButton("◀ 全部上一个", this);
    btnBackward->setStyleSheet(R"(
        QPushButton {
            background: #E3F2FD; border: 1px solid #90CAF9;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #1565C0;
        }
        QPushButton:hover { background: #BBDEFB; }
        QPushButton:pressed { background: #90CAF9; }
    )");

    auto *btnReset = new QPushButton("全部复位", this);
    btnReset->setStyleSheet(R"(
        QPushButton {
            background: #FFF3E0; border: 1px solid #FFCC80;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #E65100;
        }
        QPushButton:hover { background: #FFE0B2; }
        QPushButton:pressed { background: #FFCC80; }
    )");

    auto *btnForward = new QPushButton("全部下一个 ▶", this);
    btnForward->setStyleSheet(R"(
        QPushButton {
            background: #E3F2FD; border: 1px solid #90CAF9;
            border-radius: 4px; padding: 8px 24px;
            font-size: 14px; color: #1565C0;
        }
        QPushButton:hover { background: #BBDEFB; }
        QPushButton:pressed { background: #90CAF9; }
    )");

    btnLayout->addWidget(btnBackward);
    btnLayout->addWidget(btnReset);
    btnLayout->addWidget(btnForward);
    mainLayout->addLayout(btnLayout);

    // 连接按钮
    connect(btnForward,  &QPushButton::clicked, this, &CircleRotateDemoPage::onRotateForward);
    connect(btnBackward, &QPushButton::clicked, this, &CircleRotateDemoPage::onRotateBackward);
    connect(btnReset,    &QPushButton::clicked, this, &CircleRotateDemoPage::onReset);
}

void CircleRotateDemoPage::onRotateForward()
{
    for (RotateState &rs : m_circles) {
        if (rs.animating) continue;
        rs.animating = true;

        double start = rs.widget->rotationOffset();
        double end = start - 30.0;

        rs.anim->setStartValue(start);
        rs.anim->setEndValue(end);
        rs.anim->start();

        rs.step = (rs.step + 1) % 12;
    }
}

void CircleRotateDemoPage::onRotateBackward()
{
    for (RotateState &rs : m_circles) {
        if (rs.animating) continue;
        rs.animating = true;

        double start = rs.widget->rotationOffset();
        double end = start + 30.0;

        rs.anim->setStartValue(start);
        rs.anim->setEndValue(end);
        rs.anim->start();

        rs.step = (rs.step + 11) % 12;
    }
}

void CircleRotateDemoPage::onReset()
{
    for (RotateState &rs : m_circles) {
        if (rs.animating) {
            rs.anim->stop();
            rs.animating = false;
        }

        rs.anim->setStartValue(rs.widget->rotationOffset());
        rs.anim->setEndValue(0.0);
        rs.anim->start();
        rs.animating = true;
        rs.step = 0;
    }
}

void CircleRotateDemoPage::updateInfoLabel(int idx)
{
    if (idx < 0 || idx >= m_circles.size()) return;
    const RotateState &rs = m_circles[idx];
    rs.label->setText(QString("当前指向：%1号管 — %2")
                          .arg(rs.step + 1)
                          .arg(rs.widget->itemText(rs.step)));
}
