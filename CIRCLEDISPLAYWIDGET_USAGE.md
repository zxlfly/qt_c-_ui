# CircleDisplayWidget 使用说明

## 概述

`CircleDisplayWidget` 是一个自绘的 Qt 展示控件，呈现一个大圆形容器内 12 个小圆管环形排列的效果。小圆之间远近交替，外侧由弧线相连，顶部可绘制倒三角形把手。

适用场景：管道监控、阀门状态、环形指标展示等。

---

## 快速开始

```cpp
#include "circledisplaywidget.h"

// 1. 创建并添加到布局
CircleDisplayWidget *widget = new CircleDisplayWidget(this);
layout->addWidget(widget);

// 2. 设置小圆文字
widget->setAllTexts({"A1", "B2", "C3", "D4", "E5", "F6",
                     "G7", "H8", "I9", "J10", "K11", "L12"});

// 3. 设置某个小圆背景色
widget->setItemFillColor(0, QColor("#E8F5E9"));
```

---

## API 接口

### 数据更新

| 方法 | 说明 |
|------|------|
| `setItemText(int index, const QString &text)` | 更新第 index 个小圆的内部文字（index: 0~11） |
| `setAllTexts(const QStringList &texts)` | 一次性设置全部 12 个小圆的文字 |
| `setItemFillColor(int index, const QColor &color)` | 设置第 index 个小圆的背景填充色 |

### 样式设置

| 方法 | 说明 |
|------|------|
| `setOuterBorderColor(const QColor &color)` | 大圆边框颜色（默认 `#424242`） |
| `setInnerBorderColor(const QColor &color)` | 小圆边框 + 弧线颜色（默认 `#424242`） |
| `setTextColor(const QColor &color)` | 小圆内部文字颜色（默认 `#424242`） |
| `setHandleVisible(bool visible)` | 是否绘制顶部倒三角形（默认 `true`） |
| `handleVisible() const` | 查询倒三角形是否可见 |
| `setSelectEnabled(bool enabled)` | 是否开启点击选择功能（默认 `false`，开启后点击小圆发射 `itemClicked` 信号） |
| `selectEnabled() const` | 查询点击选择是否开启 |

---

## 布局结构

```
        ▽  ← 倒三角形把手（可选）
    ┌─────────┐
    │  1号    │  ← 远圆（外侧）
    │  ──╮──  │
    │    2号  │  ← 近圆（内侧）
    │  ──╯──  │
    │  3号    │  ← 远圆
    │  ──╮──  │
    │    4号  │  ← 近圆
    │  ...... │
    └─────────┘
```

- 12 个小圆从 **12 点方向（1号）** 开始 **顺时针** 排列，每 30° 一个
- 偶数索引（1、3、5...号）为**远圆**，离大圆圆心更远，与大圆**内切**
- 奇数索引（2、4、6...号）为**近圆**，离大圆圆心更近
- 相邻小圆**外切**（圆心距 = 2 × smallR）
- 相邻小圆**外侧**由弧线连接

---

## 编号标签位置

- **远圆**（偶数索引）：编号标签在**内侧**（靠近大圆圆心方向）
- **近圆**（奇数索引）：编号标签在**外侧**（远离大圆圆心方向）

---

## 布局参数调整

在 `circledisplaywidget.cpp` 的 `computeLayout()` 方法顶部，有 3 个可调参数：

```cpp
double farRatio       = 0.791;   // 远圆圆心距 / outerR
double nearRatio      = 0.55;    // 近圆圆心距 / outerR
bool   tangentToOuter = true;    // true=远圆内切大圆，false=仅相邻小圆相切
```

| 想改什么 | 调整方式 |
|---------|---------|
| 远圆离大圆更近/更远 | 调 `farRatio`（0~1，越大越远） |
| 近圆离大圆中心更近/更远 | 调 `nearRatio`（0~1，越大越远） |
| 远圆不内切大圆 | `tangentToOuter = false`，`farRatio` 可自由设置 |

**注意**：当 `tangentToOuter = true` 时，`smallR = outerR - farR`（保证内切），改 `farRatio` 会影响小圆大小。当 `tangentToOuter = false` 时，`smallR` 由相邻小圆相切约束独立算出。

---

## 默认颜色

| 元素 | 默认颜色 | 色值 |
|------|---------|------|
| 大圆边框 | 深灰 | `#424242` |
| 小圆边框 + 弧线 | 深灰 | `#424242` |
| 小圆填充 | 极浅灰 | `#F5F5F5` |
| 小圆内部文字 | 深灰 | `#424242` |
| 编号标签 | 深灰 | `#424242` |
| 倒三角形填充 | 深灰 | `#424242` |

---

## 绘制顺序

1. **大圆边框** — 空心圆形
2. **倒三角形把手** — 可选，默认绘制
3. **连接弧线** — 相邻小圆外侧的平滑外凸弧线
4. **小圆** — 填充 + 边框，覆盖弧线端点
5. **编号标签** — 远圆内侧，近圆外侧

### 信号

| 信号 | 说明 |
|------|------|
| `itemClicked(int index)` | 点击了第 index 个小圆（0~11），需 `setSelectEnabled(true)` 后才会发射；点空白返回 -1 |

### 辅助方法

| 方法 | 说明 |
|------|------|
| `itemCenter(int index) const` | 获取第 index 个小圆的中心像素坐标，可用于弹出菜单定位 |

---

## 点击选择功能

`CircleDisplayWidget` 内置了点击选择能力，通过 `setSelectEnabled()` 控制开关。

### 基本用法

```cpp
// 1. 创建控件
CircleDisplayWidget *widget = new CircleDisplayWidget(this);
layout->addWidget(widget);

// 2. 开启点击选择（默认关闭）
widget->setSelectEnabled(true);

// 3. 监听点击信号
connect(widget, &CircleDisplayWidget::itemClicked, this, [widget](int index) {
    if (index >= 0) {
        qDebug() << "点击了" << index << "号管，当前文字：" << ...;
    }
});
```

### 配合弹出列表选择

`CircleSelectDemoPage` 演示了点击小管 → 弹出选项列表 → 选择后更新管内文字的完整交互：

```cpp
#include "circledisplaywidget.h"
#include <QListWidget>
#include <QFrame>

// 1. 创建控件并开启选择
CircleDisplayWidget *circle = new CircleDisplayWidget(this);
circle->setSelectEnabled(true);

// 2. 创建弹出列表
QFrame *popup = new QFrame(this);
QListWidget *listWidget = new QListWidget(popup);
listWidget->addItems({"选项A", "选项B", "选项C"});
popup->hide();

// 3. 点击小管 → 定位弹出列表
connect(circle, &CircleDisplayWidget::itemClicked, this, [this, circle, popup](int index) {
    if (index < 0) { popup->hide(); return; }

    // 获取小圆中心坐标并定位弹出框
    QPointF center = circle->itemCenter(index);
    QPoint pos = circle->mapToParent(center.toPoint());
    pos += QPoint(30, -popup->height() / 2);
    popup->move(pos);
    popup->show();
    popup->raise();
});

// 4. 选择列表项 → 更新小管文字
connect(listWidget, &QListWidget::itemClicked, this, [circle, popup](QListWidgetItem *item) {
    int idx = /* 保存的当前索引 */;
    circle->setItemText(idx, item->text());
    popup->hide();
});
```

---

## 完整示例

```cpp
#include "circledisplaywidget.h"

// 创建
CircleDisplayWidget *widget = new CircleDisplayWidget(this);
someLayout->addWidget(widget);

// 隐藏顶部倒三角形
widget->setHandleVisible(false);

// 设置所有管子名称
widget->setAllTexts({"原料A", "原料B", "催化剂", "溶剂", "原料C",
                     "中间体1", "中间体2", "产品A", "产品B",
                     "废液", "回收", "排放"});

// 单独改某个管子
widget->setItemText(3, "溶剂(满)");
widget->setItemFillColor(3, QColor("#E3F2FD"));  // 浅蓝

// 标记异常
widget->setItemFillColor(9, QColor("#FFEBEE"));  // 浅红
widget->setItemText(9, "废液!");

// 改边框颜色
widget->setInnerBorderColor(QColor("#1976D2"));  // 蓝色系

// 开启点击选择
widget->setSelectEnabled(true);
connect(widget, &CircleDisplayWidget::itemClicked, this, [widget](int index) {
    if (index >= 0) {
        widget->setItemText(index, "已选中");
    }
});
```

---

## 响应式

控件大小变化时自动重绘，所有尺寸基于 `outerR` 的比例计算。最小尺寸 300×300。

---

## 文件清单

| 文件 | 说明 |
|------|------|
| `circledisplaywidget.h` | 类声明 |
| `circledisplaywidget.cpp` | 绘制实现 |
| `circledemopage.h/cpp` | 演示页面（随机更新、重置按钮） |
| `circleselectdemopage.h/cpp` | 点击选择演示页面（弹出列表选择后更新管内文字） |
