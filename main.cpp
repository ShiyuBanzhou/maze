#include "mainwindow.h"
#include "playwindow.h"
#include "registerloginwindow.h"
#include "mapselectwindow.h"

#include <QApplication>
#include <QThread>

/*
 * TODO:
 * 1. 重写QMessageBox类 以优化生硬弹窗
 * 2. 重写QLineEdit类 以实现密码保护*机制（可切换）
 */

int main(int argc, char *argv[]) {
    QApplication a(argc, argv);

    // 注册登录界面
    RegisterLoginWindow rlw;
    // 主界面
    MainWindow mw;
    // 选关界面
    MapSelectWindow msw;
    // 游戏界面
    PlayWindow *plw;

    // 界面呈现
    rlw.show();

    // 连接登录窗口的登录成功信号，显示主窗口并关闭登录窗口
    QObject::connect(&rlw, &RegisterLoginWindow::loginSuccess, [&]() {
        rlw.close(); // 关闭注册登录窗口
        mw.show();    // 显示主窗口
    });

    // 连接主窗口的点击开始游戏信号，显示选关界面并关闭主窗口
    QObject::connect(&mw, &MainWindow::beginGame, [&]() {
        mw.close();
        msw.show();    // 显示主窗口
    });

    // 连接选关界面的选关信号，显示游戏窗口并关闭选关界面
    QObject::connect(&msw, &MapSelectWindow::levelSelected, [&](int level) {
        plw = new PlayWindow(QString("C:/Users/29228/Desktop/maze/mapdata/").append(QString::number(level)).append(".mapdata"));
        msw.close();
        plw->show();
    });

    // 连接选关界面的选关信号，返回主界面并关闭选关界面
    QObject::connect(&msw, &MapSelectWindow::backBtnClicked, [&]() {
        msw.close();
        mw.show();
    });

    return a.exec();
}
