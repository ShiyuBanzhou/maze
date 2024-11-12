#include "mainwindow.h"
#include "imgbutton.h"
#include "ui_mainwindow.h"
#include "mazegenerator.h"
#include "tiletexture.h"

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

void MainWindow::on_pushButton_clicked()
{
    MazeGenerator *mg = new MazeGenerator(15);
    QVector<QVector<int>> mazeMap;
    mazeMap = mg->getMazeMap();
    for (int i = 0; i < 15; ++i) {
        for (int j = 0; j < 15; ++j) {
            TileStatus status;
            if (mazeMap[i][j] == 0) {
                status = WALL; // 0表示墙
            } else if (mazeMap[i][j] == 1) {
                status = PATH; // 1表示通路
            } else if (mazeMap[i][j] == 2) {
                status = STARTING; // 2表示起始点
            } else {
                status = ENDING; // 其他表示目的地
            }

            // 创建TileTexture对象
            TileTexture *tile = new TileTexture(status);
            tile->setParent(this); // 设置父对象
            tile->setSize(600 / 15);
            tile->move(j * (600 / 15), i * (600 / 15)); // 设置位置
            tile->lower();
            tile->show(); // 显示瓷砖
        }
    }
}

