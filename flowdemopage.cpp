#include "flowdemopage.h"
#include "flowtemplates.h"
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QPushButton>
#include <QLabel>

FlowDemoPage::FlowDemoPage(QWidget *parent)
    : QWidget(parent)
{
    auto *mainLayout = new QVBoxLayout(this);

    // 标题
    auto *title = new QLabel("流程状态展示", this);
    title->setStyleSheet("font-size: 16px; font-weight: bold; color: #424242; padding: 8px 0;");
    mainLayout->addWidget(title);

    // FlowWidget
    m_flowWidget = new FlowWidget(this);
    mainLayout->addWidget(m_flowWidget, 1);

    // 控制栏
    auto *btnSwitch = new QPushButton("切换模板", this);
    auto *btnStep   = new QPushButton("推进状态", this);
    auto *lblHint   = new QLabel("点击按钮查看效果", this);
    lblHint->setStyleSheet("color: #757575;");

    auto *ctrlLayout = new QHBoxLayout();
    ctrlLayout->addWidget(btnSwitch);
    ctrlLayout->addWidget(btnStep);
    ctrlLayout->addStretch();
    ctrlLayout->addWidget(lblHint);
    mainLayout->addLayout(ctrlLayout);

    // 加载第一个模板
    m_flowWidget->setFlowData(createTemplateA());

    // 连接信号
    connect(btnSwitch, &QPushButton::clicked, this, &FlowDemoPage::onSwitchTemplate);
    connect(btnStep,   &QPushButton::clicked, this, &FlowDemoPage::onStepForward);
}

void FlowDemoPage::onSwitchTemplate()
{
    m_stepIndex = 0;

    if (m_currentTemplate == 0) {
        m_currentTemplate = 1;
        m_flowWidget->setFlowData(createTemplateB());
    } else {
        m_currentTemplate = 0;
        m_flowWidget->setFlowData(createTemplateA());
    }
}

void FlowDemoPage::onStepForward()
{
    if (m_currentTemplate == 0) {
        switch (m_stepIndex) {
        case 0:
            m_flowWidget->updateNodeState("start", NodeState::Running);
            break;
        case 1:
            m_flowWidget->updateNodeState("start", NodeState::Success);
            m_flowWidget->updateEdgeState("start", "check", EdgeState::Done);
            m_flowWidget->updateNodeState("check", NodeState::Running);
            break;
        case 2:
            m_flowWidget->updateNodeState("check", NodeState::Success);
            m_flowWidget->updateBranchState("split1", EdgeState::Done);
            m_flowWidget->updateNodeState("review", NodeState::Running);
            m_flowWidget->updateNodeState("execute", NodeState::Running);
            m_flowWidget->updateNodeState("archive", NodeState::Running);
            break;
        case 3:
            m_flowWidget->updateNodeState("review", NodeState::Success);
            m_flowWidget->updateNodeState("execute", NodeState::Success);
            m_flowWidget->updateNodeState("archive", NodeState::Success);
            m_flowWidget->updateBranchState("merge1", EdgeState::Done);
            m_flowWidget->updateNodeState("complete", NodeState::Running);
            break;
        case 4:
            m_flowWidget->updateNodeState("complete", NodeState::Success);
            m_flowWidget->updateEdgeState("complete", "end", EdgeState::Done);
            m_flowWidget->updateNodeState("end", NodeState::Running);
            break;
        case 5:
            m_flowWidget->updateNodeState("end", NodeState::Success);
            m_stepIndex = -1;
            break;
        }
    } else {
        switch (m_stepIndex) {
        case 0:
            m_flowWidget->updateNodeState("start", NodeState::Running);
            break;
        case 1:
            m_flowWidget->updateNodeState("start", NodeState::Success);
            m_flowWidget->updateEdgeState("start", "step1", EdgeState::Done);
            m_flowWidget->updateNodeState("step1", NodeState::Running);
            break;
        case 2:
            m_flowWidget->updateNodeState("step1", NodeState::Success);
            m_flowWidget->updateEdgeState("step1", "step2", EdgeState::Done);
            m_flowWidget->updateNodeState("step2", NodeState::Running);
            break;
        case 3:
            m_flowWidget->updateNodeState("step2", NodeState::Success);
            m_flowWidget->updateEdgeState("step2", "step3", EdgeState::Done);
            m_flowWidget->updateNodeState("step3", NodeState::Running);
            break;
        case 4:
            m_flowWidget->updateNodeState("step3", NodeState::Success);
            m_flowWidget->updateEdgeState("step3", "end", EdgeState::Done);
            m_flowWidget->updateNodeState("end", NodeState::Running);
            break;
        case 5:
            m_flowWidget->updateNodeState("end", NodeState::Success);
            m_stepIndex = -1;
            break;
        }
    }

    m_stepIndex++;
}
