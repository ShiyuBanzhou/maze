#include "playwindow.h"
#include "ui_playwindow.h"
#include "mazegenerator.h"
#include "tiletexture.h"

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

    // 创建TileTexture对象并添加到窗口
    for (int i = 0; i < mazeMap.size(); ++i) {
        for (int j = 0; j < mazeMap[i].size(); ++j) {
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
            tile->move(j * (600 / mazeMap.size()), i * (600 / mazeMap.size())); // 设置位置
            tile->show(); // 显示瓷砖
        }
    }

}

PlayWindow::~PlayWindow()
{
    delete ui;
}

// 绘制迷宫
void PlayWindow::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event); // 忽略事件参数

}
