#ifndef REGISTERLOGINWINDOW_H
#define REGISTERLOGINWINDOW_H

#include <QWidget>
#include <QPainter>


namespace Ui {
class RegisterLoginWindow;
}

class RegisterLoginWindow : public QWidget
{
    Q_OBJECT

public:
    explicit RegisterLoginWindow(QWidget *parent = nullptr);
    ~RegisterLoginWindow();
    void paintEvent(QPaintEvent *event); // 重载绘制注册登录界面

private:
    Ui::RegisterLoginWindow *ui;
    bool isFirstLogin = true;

signals:
    void registerSuccess(); // 注册成功信号
    void loginSuccess(); // 登录成功信号

private slots:
    void onRegisterButtonClicked(); // 注册按钮事件处理
    void onLoginButtonClicked(); // 登录按钮事件处理
};

#endif // REGISTERLOGINWINDOW_H
