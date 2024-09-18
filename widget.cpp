#include "widget.h"
#include "ui_widget.h"
#include "imgbutton.h"

#include <QTimer>
Widget::Widget(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::Widget)
{
    ui->setupUi(this);
    // 设置固定大小的主窗口
    this->setFixedSize(800, 600);

    //开始按钮
    ImgButton *startButton = new ImgButton(":/res/startBtn.png", ":/res/ButtonSound.wav");
    startButton->setParent(this);
    startButton->move(this->width()*0.41, this->height()*0.6);
    startButton->show();
    connect(startButton, &ImgButton::clicked, [=](){
        //按钮弹跳
        startButton->buttonDown();
        startButton->buttonUp();

        //0.3秒后打开选择关卡页面
        QTimer::singleShot(300, this, [=](){
            // this->hide();
        });
    });

}

Widget::~Widget()
{
    delete ui;
}

// 重载绘制主背景
void Widget::paintEvent(QPaintEvent *)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/res/bg.png"));
}

