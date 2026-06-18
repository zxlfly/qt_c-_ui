#include "flowtemplates.h"

FlowData createTemplateA()
{
    FlowData data;
    data.defaultRadius = 0.035;

    // 节点定义（从左到右，分支区域上下排列）
    // radius=0 表示使用 defaultRadius
    data.nodes = {
        {"start",    "开始",  0.08, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"check",    "校验",  0.22, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        // 并行分支（三行）
        {"review",   "审核",  0.48, 0.22, 0.0, NodeState::Idle, LabelPosition::Top},
        {"execute",  "执行",  0.48, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"archive",  "归档",  0.48, 0.78, 0.0, NodeState::Idle, LabelPosition::Top},
        // 汇合
        {"complete", "完成",  0.74, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        // 结束
        {"end",      "结束",  0.90, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    };

    // 连线（只包含非分支的直线连接）
    data.edges = {
        {"start",   "check"},
        {"complete","end"},
    };

    // 分支连线
    data.branches = {
        // Split: 校验 → 审核/执行/归档
        {"split1",  "check",  {"review", "execute", "archive"}, 0.0, false},
        // Merge: 审核/执行/归档 → 完成
        {"merge1",  "complete", {"review", "execute", "archive"}, 0.0, true},
    };

    return data;
}

FlowData createTemplateB()
{
    FlowData data;
    data.defaultRadius = 0.035;

    // 简单线性流程
    data.nodes = {
        {"start",  "开始",  0.10, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step1",  "步骤1", 0.30, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step2",  "步骤2", 0.50, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step3",  "步骤3", 0.70, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"end",    "结束",  0.90, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    };

    data.edges = {
        {"start", "step1"},
        {"step1", "step2"},
        {"step2", "step3"},
        {"step3", "end"},
    };

    // 无分支
    data.branches = {};

    return data;
}

FlowData createTemplateC()
{
    FlowData data;
    data.defaultRadius = 0.04;

    // 双分支并行流程
    data.nodes = {
        {"start",   "启动",  0.10, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"check",   "检测",  0.25, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"pass",    "合格",  0.50, 0.28, 0.0, NodeState::Idle, LabelPosition::Top},
        {"fail",    "不合格", 0.50, 0.72, 0.0, NodeState::Idle, LabelPosition::Top},
        {"report",  "报告",  0.75, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"end",     "完成",  0.90, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    };

    data.edges = {
        {"start",  "check"},
        {"report", "end"},
    };

    data.branches = {
        {"split1", "check",  {"pass", "fail"}, 0.0, false},
        {"merge1", "report", {"pass", "fail"}, 0.0, true},
    };

    return data;
}

FlowData createTemplateD()
{
    FlowData data;
    data.defaultRadius = 0.032;

    // 7节点长线性流程
    data.nodes = {
        {"start",  "发起",  0.07, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step1",  "填表",  0.22, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step2",  "初审",  0.37, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step3",  "复审",  0.52, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step4",  "审批",  0.67, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"step5",  "归档",  0.82, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"end",    "结案",  0.95, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    };

    data.edges = {
        {"start", "step1"},
        {"step1", "step2"},
        {"step2", "step3"},
        {"step3", "step4"},
        {"step4", "step5"},
        {"step5", "end"},
    };

    data.branches = {};

    return data;
}
