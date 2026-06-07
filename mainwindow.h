#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QListWidget>

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = nullptr);
    ~MainWindow() override;

private slots:
    void onPageChanged(int row);

private:
    Q_DISABLE_COPY(MainWindow)
    QListWidget *m_navList = nullptr;
    QStackedWidget *m_stack = nullptr;

    void addPage(const QString &name, QWidget *page);
};

#endif // MAINWINDOW_H
