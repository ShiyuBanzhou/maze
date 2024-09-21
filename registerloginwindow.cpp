#include "registerloginwindow.h"
#include "ui_registerloginwindow.h"
#include "imgbutton.h"
#include "user.h"
#include "qdebug.h"

RegisterLoginWindow::RegisterLoginWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::RegisterLoginWindow)
{
    ui->setupUi(this);
    // 注册|登录按钮实例化
    ImgButton *registerButton = new ImgButton(":/res/register.png", ":/res/ButtonSound.wav");
    ImgButton *loginButton = new ImgButton(":/res/login.png", ":/res/ButtonSound.wav");

    // 注册按钮
    registerButton->setParent(this);
    registerButton->move(this->width()*0.35, this->height()*0.6);
    registerButton->show();

    // 登录按钮
    loginButton->setParent(this);
    loginButton->move(this->width()*0.58, this->height()*0.6);
    loginButton->show();

    // 根据资源文件中索引的用户信息文件创建可写入副本
    if (!User::initializeUserFile()) {
        QMessageBox::critical(nullptr, "错误", "用户文件初始化失败");
        exit(0);
    }

    // 注册按钮动画实现
    connect(registerButton, &ImgButton::clicked, [=](){
        //按钮弹跳
        registerButton->buttonDown();
        registerButton->buttonUp();
        onRegisterButtonClicked();  // 调用注册函数
    });

    // 登录按钮动画实现
    connect(loginButton, &ImgButton::clicked, [=](){
        //按钮弹跳
        loginButton->buttonDown();
        loginButton->buttonUp();
        onLoginButtonClicked();  // 调用登录函数
    });
}

RegisterLoginWindow::~RegisterLoginWindow()
{
    delete ui;
}

// 绘制背景
void RegisterLoginWindow::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/res/rlbp.jpg"));
}

void RegisterLoginWindow::onRegisterButtonClicked()
{
    QString username = ui->userName->text(); // 获取用户名
    QString password = ui->userPassword->text(); // 获取密码

    User newUser(username, password); // 创建用户对象
    // qDebug() << username << "," << password;
    // 注册用户
    if (newUser.registerUser()) {
        // 用户注册成功 或 用户信息已存在
    }
}

void RegisterLoginWindow::onLoginButtonClicked()
{
    QString username = ui->userName->text(); // 获取用户名
    QString password = ui->userPassword->text(); // 获取密码
    // 只在第一次调用时初始化对象
    static User newUser(username, password); // 创建用户对象

    // 确保用户输入错误检测正常运行
    if(!isFirstLogin){
        newUser.setUsername(username);
        newUser.setPassword(password);
    }else{
        isFirstLogin = false;
    }
    // 用户登录
    if(newUser.login()){
        // 用户登录成功
        QMessageBox::information(nullptr, "成功", "恭喜您，登录成功！");
        // 抛出用户登录成功信息
        emit loginSuccess();
    }

}
