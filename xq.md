业务场景
在 Qt C++ 页面中的某个固定区域内，展示 流程状态图。
流程特点
●  流程结构预先确定，不是动态生成。 
●  一个页面通常会展示 2 条或多条独立流程。 
●  流程可能有多个起点。 
●  节点数量、连接关系、分支结构均已知。 
●  节点大小不固定（有大有小）。 
●  每个节点都有文字标识。 
交互需求
●  仅展示，不支持编辑。 
●  不支持拖拽、缩放、连线修改。 
●  不需要流程设计器能力。 
动态变化内容
运行过程中可能变化：
●  节点颜色 
●  节点图标 
●  节点状态（未开始、运行中、成功、失败等） 
●  连线颜色 
●  节点文字 
流程结构本身基本不变。
页面特点
●  流程图只是页面中的一个组件，不是整个页面。 
●  外层页面已经支持响应式布局。 
●  流程区域容器尺寸已知。 
●  最坏情况下接受固定尺寸实现。 
技术目标
希望找到一种：
●  实现简单 
●  易维护 
●  后续修改状态方便 
●  不引入过重框架 
●  能适应容器尺寸变化（可选） 
的实现方案。

按照这个总结来看，本质上就是：
在一个 QWidget 中绘制固定模板的流程状态图，根据业务状态动态刷新节点和连线样式。
而不是流程编辑器、拓扑图引擎或组织架构图系统。


推荐方案
采用：
FlowWidget（继承 QWidget）
    ↓
维护流程数据
    ↓
paintEvent() 自绘
不使用：
QGraphicsView
QGraphicsScene
大量 QLabel
大量 QFrame
复杂 Layout

整体架构
页面
├── 其它业务区域
├── FlowWidget（流程A）
├── FlowWidget（流程B）
└── 其它业务区域

数据结构
节点：
struct FlowNode
{
    QString id;
    QString text;

    double x;      // 相对坐标(0~1)
    double y;      // 相对坐标(0~1)

    double radius;

    NodeState state;
};
连线：
struct FlowEdge
{
    QString fromId;
    QString toId;

    EdgeState state;
};
流程：
struct FlowData
{
    QVector<FlowNode> nodes;
    QVector<FlowEdge> edges;
};

流程模板
每种流程定义一份模板：
FlowTemplateA
FlowTemplateB
FlowTemplateC
例如：
开始
 │
校验
 ├──审核
 ├──执行
 └──归档
 │
结束
节点坐标提前配置好：
{
    "start",
    "开始",
    0.2,
    0.1,
    20
}

绘制流程
第一步：画线
for(edge : edges)
{
    painter.drawLine(...);
}
根据状态决定颜色：
灰色
蓝色
绿色
红色

第二步：画节点
for(node : nodes)
{
    painter.drawEllipse(...);
}
根据状态决定：
背景色
边框色
图标
大小

第三步：画文字
painter.drawText(...);
显示：
设备A
数据校验
任务执行
...

响应式实现
节点保存相对坐标：
0~1
例如：
x = 0.3;
y = 0.4;
绘制时：
realX = width() * x;
realY = height() * y;
这样：
容器变大
→ 自动放大

容器变小
→ 自动缩小
无需额外处理。

状态刷新
业务层：
ui->flowWidget->updateNodeState(
    "node3",
    Running
);
内部：
node.state = Running;
update();
触发重绘。

最终效果
FlowWidget
├── 保存节点数据
├── 保存连线数据
├── 保存节点状态
├── paintEvent()
└── updateNodeState()
业务代码只负责：
更新状态
↓
FlowWidget刷新
↓
自动重绘
整个实现大概只有一个 FlowWidget 类 + 几个数据结构，复杂度低，比较符合你这种固定模板流程展示组件的场景。