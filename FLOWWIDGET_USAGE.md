# FlowWidget 流程状态展示组件

## 概述

FlowWidget 是一个基于 QWidget + paintEvent 自绘的流程状态展示组件，用于可视化展示固定模板的流程节点及其运行状态。不支持编辑，仅做展示用途。

## 文件结构

| 文件 | 说明 |
|------|------|
| `flowdata.h` | 数据结构定义（枚举、节点、连线、分支、流程） |
| `flowwidget.h/cpp` | FlowWidget 绘制组件 |
| `flowtemplates.h/cpp` | 流程模板定义 |
| `flowdemopage.h/cpp` | 演示页面（含交互按钮） |

---

## 快速开始

### 1. 引入头文件

```cpp
#include "flowwidget.h"
#include "flowdata.h"
```

### 2. 创建实例并添加到布局

```cpp
FlowWidget *flowWidget = new FlowWidget(this);
someLayout->addWidget(flowWidget);
```

### 3. 定义流程数据并加载

```cpp
FlowData data;
data.defaultRadius = 0.035;

data.nodes = {
    {"start", "开始", 0.08, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    {"check", "校验", 0.25, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    {"end",   "结束", 0.80, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
};

data.edges = {
    {"start", "check"},
    {"check", "end"},
};

flowWidget->setFlowData(data);
```

### 4. 运行时更新状态

```cpp
flowWidget->updateNodeState("check", NodeState::Running);
flowWidget->updateEdgeState("start", "check", EdgeState::Done);
flowWidget->updateBranchState("split1", EdgeState::Done);
flowWidget->updateNodeText("check", "校验中...");
```

---

## 数据结构

### NodeState — 节点状态

| 值 | 说明 | 填充色 | 边框色 |
|----|------|--------|--------|
| `Idle` | 空闲 | #E0E0E0 浅灰 | #BDBDBD 灰 |
| `Running` | 运行中 | #E3F2FD 浅蓝 | #2196F3 蓝 |
| `Success` | 成功 | #E8F5E9 浅绿 | #4CAF50 绿 |
| `Failed` | 失败 | #FFEBEE 浅红 | #F44336 红 |

### EdgeState — 连线状态

| 值 | 说明 | 颜色 | 线型 |
|----|------|------|------|
| `Idle` | 空闲 | #BDBDBD 灰 | 实线 |
| `Active` | 活跃 | #2196F3 蓝 | 虚线 |
| `Done` | 完成 | #4CAF50 绿 | 实线 |
| `Error` | 错误 | #F44336 红 | 实线 |

### LabelPosition — 文字标签位置

| 值 | 说明 |
|----|------|
| `Top` | 圆的上方（默认） |
| `Bottom` | 圆的下方 |
| `Left` | 圆的左侧 |
| `Right` | 圆的右侧 |

### FlowNode — 节点

```cpp
struct FlowNode {
    QString id;                            // 唯一标识
    QString text;                          // 显示文字
    double x = 0.0;                        // 相对 x 坐标 (0~1)
    double y = 0.0;                        // 相对 y 坐标 (0~1)
    double radius = 0.0;                  // 节点半径，0=使用默认值
    NodeState state = NodeState::Idle;     // 状态
    LabelPosition labelPos = LabelPosition::Top;  // 文字位置
};
```

- `radius = 0` 时使用 `FlowData::defaultRadius`，`> 0` 则单独覆盖

### FlowEdge — 普通连线

```cpp
struct FlowEdge {
    QString fromId;                        // 起始节点 id
    QString toId;                          // 目标节点 id
    EdgeState state = EdgeState::Idle;    // 状态
};
```

- 连线从起始节点圆边缘到目标节点圆边缘，末端带箭头

### FlowBranch — 分支连线（带拐角的树形连线）

```cpp
struct FlowBranch {
    QString id;                            // 唯一标识
    QString stemId;                         // 单侧节点 id
    QStringList branchIds;                 // 多侧节点 id 列表
    double columnX = 0.0;                  // 保留字段，当前自动计算
    bool isMerge = false;                  // false=Split, true=Merge
    EdgeState state = EdgeState::Idle;     // 状态
};
```

**Split（一→多）**：从 stem 画水平线→竖线→水平线+箭头到每个 branch
**Merge（多→一）**：从每个 branch 画水平线→竖线→水平线+箭头到 stem

拐角竖线的 x 位置自动取 stem 和 branch 节点的中间。

> **注意**：属于分支的 from→to 关系不要重复写进 `edges`，否则会画两次。

### FlowData — 整体流程数据

```cpp
struct FlowData {
    double defaultRadius = 0.035;         // 默认节点半径（相对值）
    QVector<FlowNode> nodes;
    QVector<FlowEdge> edges;
    QVector<FlowBranch> branches;
};
```

---

## API 接口

### 设置数据

```cpp
void setFlowData(const FlowData &data);
```

替换整个流程数据，触发重绘。

### 更新节点状态

```cpp
void updateNodeState(const QString &id, NodeState state);
```

### 更新普通连线状态

```cpp
void updateEdgeState(const QString &fromId, const QString &toId, EdgeState state);
```

### 更新分支连线状态

```cpp
void updateBranchState(const QString &id, EdgeState state);
```

### 更新节点文字

```cpp
void updateNodeText(const QString &id, const QString &text);
```

### 获取当前数据

```cpp
const FlowData &flowData() const;
```

### 箭头控制

```cpp
void setArrowVisible(bool visible);  // true=绘制箭头（默认），false=不绘制
bool arrowVisible() const;           // 查询当前状态
```

---

## 坐标系统

- 所有 x/y 坐标范围为 **0~1**，相对于控件宽高
- `defaultRadius` 和节点 `radius` 为相对值，绘制时乘以 `min(width, height)`
- 控件缩放时自动响应，节点和连线等比例变化

### 坐标示例

```
(0,0)─────────────────(1,0)
  │                       │
  │   (0.08,0.50)         │
  │   (0.25,0.50)         │
  │   (0.50,0.25)         │
  │   (0.50,0.75)         │
  │   (0.80,0.50)         │
  │                       │
(0,1)─────────────────(1,1)
```

---

## 模板定义

每种流程定义一个函数，返回配置好的 `FlowData`：

```cpp
FlowData createTemplateA()
{
    FlowData data;
    data.defaultRadius = 0.035;

    data.nodes = {
        {"start",   "开始", 0.08, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"check",   "校验", 0.22, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"review",  "审核", 0.48, 0.22, 0.0, NodeState::Idle, LabelPosition::Top},
        {"execute", "执行", 0.48, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"archive", "归档", 0.48, 0.78, 0.0, NodeState::Idle, LabelPosition::Top},
        {"complete","完成", 0.74, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
        {"end",     "结束", 0.90, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    };

    data.edges = {
        {"start",   "check"},
        {"complete","end"},
    };

    data.branches = {
        {"split1", "check",   {"review", "execute", "archive"}, 0.0, false},
        {"merge1", "complete",{"review", "execute", "archive"}, 0.0, true},
    };

    return data;
}
```

---

## 绘制顺序

`paintEvent` 按以下顺序绘制，后绘制的覆盖先绘制的：

1. **分支连线**（FlowBranch）— 带拐角的树形线
2. **普通连线**（FlowEdge）— 直线 + 箭头
3. **节点圆形**（FlowNode）— 填充 + 边框
4. **文字标签** — 节点文字

---

## 完整示例

```cpp
#include "flowwidget.h"
#include "flowdata.h"

// 创建
FlowWidget *fw = new FlowWidget(this);
layout->addWidget(fw);

// 定义流程
FlowData data;
data.defaultRadius = 0.035;
data.nodes = {
    {"s1", "开始",  0.10, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    {"s2", "步骤1", 0.35, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    {"s3", "步骤2", 0.65, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
    {"s4", "结束",  0.90, 0.50, 0.0, NodeState::Idle, LabelPosition::Top},
};
data.edges = {
    {"s1", "s2"},
    {"s2", "s3"},
    {"s3", "s4"},
};

fw->setFlowData(data);

// 运行时更新
fw->updateNodeState("s1", NodeState::Success);
fw->updateEdgeState("s1", "s2", EdgeState::Done);
fw->updateNodeState("s2", NodeState::Running);
```
