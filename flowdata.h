#ifndef FLOWDATA_H
#define FLOWDATA_H

#include <QString>
#include <QVector>
#include <QStringList>

// 节点状态
enum class NodeState {
    Idle,
    Running,
    Success,
    Failed
};

// 连线状态
enum class EdgeState {
    Idle,
    Active,
    Done,
    Error
};

// 文字标签位置
enum class LabelPosition {
    Top,     // 上方（默认）
    Bottom,  // 下方
    Left,    // 左侧
    Right    // 右侧
};

// 流程节点
struct FlowNode {
    QString id;
    QString text;
    double x = 0.0;          // 相对坐标 0~1
    double y = 0.0;          // 相对坐标 0~1
    double radius = 0.0;     // 0=使用 FlowData::defaultRadius，否则用此值
    NodeState state = NodeState::Idle;
    LabelPosition labelPos = LabelPosition::Top;
};

// 流程连线
struct FlowEdge {
    QString fromId;
    QString toId;
    EdgeState state = EdgeState::Idle;
};

// 分支连接（带拐角的树形连线）
// Split: stemId → branchIds (一个出发，多个到达)
// Merge: branchIds → stemId (多个出发，一个到达)
struct FlowBranch {
    QString id;
    QString stemId;          // 单侧节点
    QStringList branchIds;   // 多侧节点
    double columnX = 0.0;    // 拐角竖线的 x 坐标（相对值）
    bool isMerge = false;    // false=split(一→多), true=merge(多→一)
    EdgeState state = EdgeState::Idle;  // 分支连线整体状态
};

// 整体流程数据
struct FlowData {
    double defaultRadius = 0.035;  // 默认节点半径（相对值）
    QVector<FlowNode> nodes;
    QVector<FlowEdge> edges;
    QVector<FlowBranch> branches;
};

#endif // FLOWDATA_H
