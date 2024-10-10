#include "playwindow.h"
#include "mapdata.h"
#include "ui_playwindow.h"
#include "tiletexture.h"
#include "mazegenerator.h"

#include <QPainter>
#include <QVector>

PlayWindow::PlayWindow(QString mapPath, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWindow)
    , mapData(new MapData()) // 确保初始化
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
    this->setMapPath(mapPath);
    if (!mapData) return;
    int tempSize = this->mapData->getMapSize();
    if(tempSize > 0){
        // 创建TileTexture对象并添加到窗口
        for (int i = 0; i < tempSize; ++i) {
            for (int j = 0; j < tempSize; ++j) {
                TileStatus status;
                if (this->mapData->mazeMap[i][j] == 0) {
                    status = WALL; // 0表示墙
                } else if (this->mapData->mazeMap[i][j] == 1) {
                    status = PATH; // 1表示通路
                } else if (this->mapData->mazeMap[i][j] == 2) {
                    status = STARTING; // 2表示起始点
                } else {
                    status = ENDING; // 其他表示目的地
                }

                // 创建TileTexture对象
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this); // 设置父对象
                tile->setSize(600 / tempSize);
                tile->move(j * (600 / tempSize), i * (600 / tempSize)); // 设置位置
                tile->show(); // 显示瓷砖
            }
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
    QPainter painter(this);
    painter.drawPixmap(0, 0, 600, 600, QPixmap(":/res/road.png"));
    painter.drawPixmap(600, 0, 200, 600, QPixmap(":/res/statusBackground.png"));
}

// 设置地图地址
void PlayWindow::setMapPath(QString mapPath)
{
    this->mapData->setMapPath(mapPath);
    this->mapData->loadMap();
}
