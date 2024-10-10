#include "mapselectwindow.h"
#include "ui_mapselectwindow.h"
#include "imgbutton.h"

#include <QTimer>
#include <QPainter>
#include <QLabel>
#include <QVector>

MapSelectWindow::MapSelectWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::MapSelectWindow)
{
    ui->setupUi(this);

    // 返回按钮
    ImgButton *backBtn = new ImgButton(":/res/backBtn_1.png",
                                       ":/res/backBtn_2.png",
                                       ":/res/ButtonSound.wav");
    backBtn->setParent(this);
    backBtn->move(550, this->height() * 0.8 + 70);
    backBtn->show();

    connect(backBtn, &ImgButton::clicked, this, [=](){
        emit backBtnClicked();
    });

    QVector<ImgButton*> tempBtn; // 使用 QVector 管理按钮
    // 选关按钮
    for(int i = 0; i < 16; i++){
        ImgButton *button = new ImgButton(":/res/chooseBtn_1.png",
                                          ":/res/chooseBtn_2.png",
                                          ":/res/ButtonSound.wav");
        button->setParent(this);
        button->move(140 + (i % 4) * 150, 100 + (i / 4) * 100);
        button->show();

        tempBtn.append(button); // 添加按钮到 QVector

        // 绑定按钮点击事件
        connect(button, &ImgButton::clicked, this, [=]() {
            button->buttonDown();
            button->buttonUp();
            // 发出选择关卡的信号
            emit levelSelected(i + 1); // 发出信号，传递关卡编号
        });

        // 设置按钮上的数字标签
        QLabel *label = new QLabel(this);
        label->setFixedSize(button->width(), button->height());
        label->setText(QString::number(i + 1));
        label->setAlignment(Qt::AlignHCenter | Qt::AlignVCenter); // 设置居中
        label->move(140 + (i % 4) * 150, 100 + (i / 4) * 100);
        label->setAttribute(Qt::WA_TransparentForMouseEvents, true); // 鼠标事件穿透
        label->setFont(QFont("微软雅黑", 13));
    }
}

MapSelectWindow::~MapSelectWindow()
{
    delete ui; // 确保 ui 被删除
    // 动态分配的按钮会在 QVector 的析构时被自动删除
}

// 绘制背景
void MapSelectWindow::paintEvent(QPaintEvent *event)
{
    QPainter painter(this);
    painter.drawPixmap(0, 0, width(), height(), QPixmap(":/res/mapSelctionBp.png"));
}
