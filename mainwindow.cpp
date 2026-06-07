#include "mainwindow.h"
#include "flowdemopage.h"
#include <QHBoxLayout>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
{
    setWindowTitle("UI 组件示例");
    resize(1000, 600);

    // 中央容器
    auto *central = new QWidget(this);
    setCentralWidget(central);
    auto *mainLayout = new QHBoxLayout(central);
    mainLayout->setContentsMargins(0, 0, 0, 0);
    mainLayout->setSpacing(0);

    // 左侧导航
    m_navList = new QListWidget(this);
    m_navList->setFixedWidth(160);
    m_navList->setStyleSheet(R"(
        QListWidget {
            background: #FAFAFA;
            border: none;
            border-right: 1px solid #E0E0E0;
            outline: none;
            font-size: 14px;
        }
        QListWidget::item {
            padding: 14px 16px;
            border: none;
            color: #424242;
        }
        QListWidget::item:selected {
            background: #E3F2FD;
            color: #1565C0;
        }
        QListWidget::item:hover {
            background: #F5F5F5;
        }
    )");
    mainLayout->addWidget(m_navList);

    // 右侧页面区
    m_stack = new QStackedWidget(this);
    mainLayout->addWidget(m_stack, 1);

    // 注册页面
    addPage("流程状态展示", new FlowDemoPage());
    // 后续添加新页面：
    // addPage("新功能名称", new NewDemoPage());

    // 默认选中第一项
    m_navList->setCurrentRow(0);

    // 连接
    connect(m_navList, &QListWidget::currentRowChanged,
            this, &MainWindow::onPageChanged);
}

MainWindow::~MainWindow() = default;

void MainWindow::addPage(const QString &name, QWidget *page)
{
    m_navList->addItem(name);
    m_stack->addWidget(page);
}

void MainWindow::onPageChanged(int row)
{
    if (row >= 0 && row < m_stack->count()) {
        m_stack->setCurrentIndex(row);
    }
}
