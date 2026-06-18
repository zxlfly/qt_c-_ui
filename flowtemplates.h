#ifndef FLOWTEMPLATES_H
#define FLOWTEMPLATES_H

#include "flowdata.h"

// 创建示例流程模板 A：带并行分支的流程（3分支）
FlowData createTemplateA();

// 创建示例流程模板 B：线性流程（5节点）
FlowData createTemplateB();

// 创建示例流程模板 C：双分支并行流程
FlowData createTemplateC();

// 创建示例流程模板 D：长线性流程（7节点）
FlowData createTemplateD();

#endif // FLOWTEMPLATES_H
