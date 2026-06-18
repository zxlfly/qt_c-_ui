#include "flowdemopage.h"
#include "flowtemplates.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QPushButton>
#include <QLabel>
#include <QGroupBox>

// ── 构造 ──────────────────────────────────────────

FlowDemoPage::FlowDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("流程状态展示（堆叠多控件）", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // 控制栏
    auto *btnSwitch = new QPushButton("切换页面", this);
    auto *btnStep   = new QPushButton("全部推进", this);
    auto *lblHint   = new QLabel("第 1 页 / 共 2 页", this);
    lblHint->setStyleSheet("color: #757575;");

    auto *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(btnSwitch);
    ctrlLayout->addWidget(btnStep);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(lblHint);
    mainLayout->addLayout(ctrlLayout);

    // 堆叠控件
    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack, 1);

    // 页面1：并行分支 + 线性
    auto *page1 = createPage(
        {"并行分支流程", "线性流程", "双分支流程", "长线性流程"},
        {0, 1, 2, 3});
    PageState ps1;
    ps1.page = page1;
    for (int i = 0; i < 4; ++i) {
        FlowState fs;
        fs.templateId = i;
        // 从页面中找到对应的 FlowWidget（在 GroupBox → QVBoxLayout → FlowWidget）
        auto *gb = qobject_cast<QGroupBox *>(page1->layout()->itemAt(i)->widget());
        if (gb) {
            auto *vlay = qobject_cast<QVBoxLayout *>(gb->layout());
            if (vlay) {
                fs.widget = qobject_cast<FlowWidget *>(vlay->itemAt(0)->widget());
            }
        }
        ps1.flows.append(fs);
    }
    m_pages.append(ps1);
    m_stack->addWidget(page1);

    // 页面2：同样的4种模板，独立实例
    auto *page2 = createPage(
        {"并行分支流程", "线性流程", "双分支流程", "长线性流程"},
        {0, 1, 2, 3});
    PageState ps2;
    ps2.page = page2;
    for (int i = 0; i < 4; ++i) {
        FlowState fs;
        fs.templateId = i;
        auto *gb = qobject_cast<QGroupBox *>(page2->layout()->itemAt(i)->widget());
        if (gb) {
            auto *vlay = qobject_cast<QVBoxLayout *>(gb->layout());
            if (vlay) {
                fs.widget = qobject_cast<FlowWidget *>(vlay->itemAt(0)->widget());
            }
        }
        ps2.flows.append(fs);
    }
    m_pages.append(ps2);
    m_stack->addWidget(page2);

    // 连接
    connect(btnSwitch, &QPushButton::clicked, this, [this, lblHint]() {
        m_currentPage = (m_currentPage + 1) % m_pages.size();
        m_stack->setCurrentIndex(m_currentPage);
        lblHint->setText(QString("第 %1 页 / 共 %2 页").arg(m_currentPage + 1).arg(m_pages.size()));
    });

    connect(btnStep, &QPushButton::clicked, this, &FlowDemoPage::onStepForward);
}

// ── 创建堆叠页面 ──────────────────────────────────

QWidget *FlowDemoPage::createPage(const QStringList &names,
                                  const QList<int> &templateIds)
{
    auto *page = new QWidget();
    auto *grid = new QGridLayout(page);
    grid->setSpacing(6);

    for (int i = 0; i < 4; ++i) {
        auto *gb = new QGroupBox(names.value(i, QStringLiteral("流程")), page);
        auto *vlay = new QVBoxLayout(gb);
        vlay->setContentsMargins(4, 4, 4, 4);

        auto *fw = new FlowWidget(gb);
        fw->setFlowData(templateData(templateIds.value(i, 0)));
        vlay->addWidget(fw);

        // 2x2 网格
        grid->addWidget(gb, i / 2, i % 2);
    }

    return page;
}

// ── 模板数据 ──────────────────────────────────────

FlowData FlowDemoPage::templateData(int id)
{
    switch (id) {
    case 0: return createTemplateA();
    case 1: return createTemplateB();
    case 2: return createTemplateC();
    case 3: return createTemplateD();
    }
    return createTemplateA();
}

int FlowDemoPage::templateStepCount(int id)
{
    switch (id) {
    case 0: return 6;  // TemplateA: 6步
    case 1: return 6;  // TemplateB: 6步
    case 2: return 6;  // TemplateC: 6步
    case 3: return 8;  // TemplateD: 8步
    }
    return 6;
}

// ── 推进逻辑 ──────────────────────────────────────

void FlowDemoPage::onStepForward()
{
    PageState &ps = m_pages[m_currentPage];
    for (FlowState &fs : ps.flows) {
        stepFlow(fs);
    }
}

void FlowDemoPage::resetFlow(FlowState &fs)
{
    fs.stepIndex = 0;
    fs.widget->setFlowData(templateData(fs.templateId));
}

void FlowDemoPage::stepFlow(FlowState &fs)
{
    FlowWidget *w = fs.widget;
    int &step = fs.stepIndex;

    switch (fs.templateId) {

    // ── TemplateA：并行分支（3分支），6步 ──
    case 0: {
        switch (step) {
        case 0:
            w->updateNodeState("start", NodeState::Running);
            break;
        case 1:
            w->updateNodeState("start", NodeState::Success);
            w->updateEdgeState("start", "check", EdgeState::Done);
            w->updateNodeState("check", NodeState::Running);
            break;
        case 2:
            w->updateNodeState("check", NodeState::Success);
            w->updateBranchState("split1", EdgeState::Done);
            w->updateNodeState("review", NodeState::Running);
            w->updateNodeState("execute", NodeState::Running);
            w->updateNodeState("archive", NodeState::Running);
            break;
        case 3:
            w->updateNodeState("review", NodeState::Success);
            w->updateNodeState("execute", NodeState::Success);
            w->updateNodeState("archive", NodeState::Success);
            w->updateBranchState("merge1", EdgeState::Done);
            w->updateNodeState("complete", NodeState::Running);
            break;
        case 4:
            w->updateNodeState("complete", NodeState::Success);
            w->updateEdgeState("complete", "end", EdgeState::Done);
            w->updateNodeState("end", NodeState::Running);
            break;
        case 5:
            w->updateNodeState("end", NodeState::Success);
            step = -1;  // 下次从0开始
            break;
        }
        break;
    }

    // ── TemplateB：线性流程，6步 ──
    case 1: {
        switch (step) {
        case 0:
            w->updateNodeState("start", NodeState::Running);
            break;
        case 1:
            w->updateNodeState("start", NodeState::Success);
            w->updateEdgeState("start", "step1", EdgeState::Done);
            w->updateNodeState("step1", NodeState::Running);
            break;
        case 2:
            w->updateNodeState("step1", NodeState::Success);
            w->updateEdgeState("step1", "step2", EdgeState::Done);
            w->updateNodeState("step2", NodeState::Running);
            break;
        case 3:
            w->updateNodeState("step2", NodeState::Success);
            w->updateEdgeState("step2", "step3", EdgeState::Done);
            w->updateNodeState("step3", NodeState::Running);
            break;
        case 4:
            w->updateNodeState("step3", NodeState::Success);
            w->updateEdgeState("step3", "end", EdgeState::Done);
            w->updateNodeState("end", NodeState::Running);
            break;
        case 5:
            w->updateNodeState("end", NodeState::Success);
            step = -1;
            break;
        }
        break;
    }

    // ── TemplateC：双分支，6步 ──
    case 2: {
        switch (step) {
        case 0:
            w->updateNodeState("start", NodeState::Running);
            break;
        case 1:
            w->updateNodeState("start", NodeState::Success);
            w->updateEdgeState("start", "check", EdgeState::Done);
            w->updateNodeState("check", NodeState::Running);
            break;
        case 2:
            w->updateNodeState("check", NodeState::Success);
            w->updateBranchState("split1", EdgeState::Done);
            w->updateNodeState("pass", NodeState::Running);
            w->updateNodeState("fail", NodeState::Running);
            break;
        case 3:
            w->updateNodeState("pass", NodeState::Success);
            w->updateNodeState("fail", NodeState::Failed);
            w->updateBranchState("merge1", EdgeState::Done);
            w->updateNodeState("report", NodeState::Running);
            break;
        case 4:
            w->updateNodeState("report", NodeState::Success);
            w->updateEdgeState("report", "end", EdgeState::Done);
            w->updateNodeState("end", NodeState::Running);
            break;
        case 5:
            w->updateNodeState("end", NodeState::Success);
            step = -1;
            break;
        }
        break;
    }

    // ── TemplateD：长线性（7节点），8步 ──
    case 3: {
        // 节点顺序: start → step1 → step2 → step3 → step4 → step5 → end
        const QStringList nodeIds = {"start", "step1", "step2", "step3", "step4", "step5", "end"};
        const QStringList edgePairs = {
            "start|step1", "step1|step2", "step2|step3",
            "step3|step4", "step4|step5", "step5|end"
        };

        if (step == 0) {
            // 第一步：启动第一个节点
            w->updateNodeState(nodeIds[0], NodeState::Running);
        } else if (step <= 6) {
            // 中间步：标记前一个成功 + 连线完成 + 启动下一个
            w->updateNodeState(nodeIds[step - 1], NodeState::Success);
            QString edge = edgePairs[step - 1];
            QStringList parts = edge.split('|');
            w->updateEdgeState(parts[0], parts[1], EdgeState::Done);
            w->updateNodeState(nodeIds[step], NodeState::Running);
        } else {
            // 最后一步：end → Success
            w->updateNodeState("end", NodeState::Success);
            step = -1;
        }
        break;
    }

    } // switch templateId

    step++;
}
