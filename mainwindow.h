#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include "registerloginwindow.h"
#include "ui_registerloginwindow.h"
#include <QWidget>
#include <QPainter>

QT_BEGIN_NAMESPACE
namespace Ui {
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    // 注册登录窗口
    RegisterLoginWindow *registerLoginWindow = NULL;

    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();
    void paintEvent(QPaintEvent *event); // 重载绘制主背景

private:
    Ui::MainWindow *ui;
};
#endif // MAINWINDOW_H
