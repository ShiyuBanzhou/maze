#include "mainwindow.h"
#include "imgbutton.h"
#include "ui_mainwindow.h"

#include <QPainter>
#include <QTimer>
MainWindow::MainWindow(QWidget *parent) : QWidget(parent), ui(new Ui::MainWindow) {
    ui->setupUi(this);
    // 设置固定大小的主窗口
    this->setFixedSize(800, 600);

    // 开始按钮
    ImgButton *startButton = new ImgButton(":/res/startBtn.png", ":/res/ButtonSound.wav");
    startButton->setParent(this);
    startButton->move(this->width() * 0.41, this->height() * 0.6);
    startButton->show();

    // 开始按钮动画实现
    connect(startButton, &ImgButton::clicked, [=]() {
        // 按钮弹跳
        startButton->buttonDown();
        startButton->buttonUp();

        // 0.3秒后返回开始游戏信号
        QTimer::singleShot(300, this, [=]() {
            emit beginGame();
        });
    });
}

MainWindow::~MainWindow() {
    delete ui;
}

// 重载绘制主背景
void MainWindow::paintEvent(QPaintEvent *) {
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/res/bg.png"));
}
