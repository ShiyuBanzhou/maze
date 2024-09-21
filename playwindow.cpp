#include "playwindow.h"
#include "ui_playwindow.h"
#include "mazegenerator.h"

#include <QPainter>

PlayWindow::PlayWindow(QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWindow)
{
    ui->setupUi(this);
    /*
     * 测试迷宫生成可行性：
    MazeGenerator *map = new MazeGenerator(9);
    QVector<QVector<int>> test = map->getMazeMap();
    for(int i = 0; i < 9; i++){
        for(int j = 0; j < 9; j++){
            qDebug() << test[i][j];
        }
    }
    */
    MazeGenerator *map = new MazeGenerator(15);
    this->mazeMap = map->getMazeMap();
}

PlayWindow::~PlayWindow()
{
    delete ui;
}

// 绘制迷宫
void PlayWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // 忽略事件参数

    QPainter painter(this);
    int cellSize = 40; // 每个单元格的大小

    for (int i = 0; i < mazeMap.size(); i++) {
        for (int j = 0; j < mazeMap[i].size(); j++) {
            if (mazeMap[i][j] == 1) {
                painter.setBrush(Qt::white); // 路径为白色
            } else {
                painter.setBrush(Qt::black); // 墙为黑色
            }
            painter.drawRect(j * cellSize, i * cellSize, cellSize, cellSize); // 绘制矩形
        }
    }
}
