#include "mainwindow.h"
#include "registerloginwindow.h"

#include <QApplication>

/*
 * TODO:
 * 1. 重写QMessageBox类 以优化生硬弹窗
 * 2. 重写QLineEdit类 以实现密码保护*机制（可切换）
*/

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    // 注册登录界面
    RegisterLoginWindow rlw;
    // 主界面
    MainWindow w;
    // 画布界面呈现
    rlw.show();
    // 连接登录窗口的登录成功信号，显示主窗口并关闭登录窗口
    QObject::connect(&rlw, &RegisterLoginWindow::loginSuccess, [&]() {
        rlw.close();  // 关闭注册登录窗口
        w.show();     // 显示主窗口
    });
    return a.exec();
}
