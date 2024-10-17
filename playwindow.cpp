#include "playwindow.h"
#include "imgbutton.h"
#include "mapdata.h"
#include "ui_playwindow.h"
#include "tiletexture.h"

#include <QPainter>
#include <QVector>
#include <QKeyEvent>
#include <QStack>
#include <QMessageBox>
#include <QTimer>

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

    ImgButton *autoFoundWay = new ImgButton(":/res/autoBt.png", ":/res/ButtonSound.wav");
    autoFoundWay->setParent(this);
    autoFoundWay->move(640, this->height()*0.56);
    autoFoundWay->show();

    // 读取地图
    this->setMapPath(mapPath);
    if (!mapData) return;
    int tempSize = this->mapData->getMapSize();

    //设置StrongFocus才能监听键盘事件
    setFocusPolicy(Qt::StrongFocus);

    // 地图贴图绘制
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
                    this->startX = i;
                    this->startY = j;
                    this->posX = i;
                    this->posY = j;
                } else {
                    status = ENDING; // 其他表示目的地
                    this->endX = i;
                    this->endY = j;
                }

                // 创建TileTexture对象
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this); // 设置父对象
                tile->setSize(600 / tempSize);
                tile->move(j * (600 / tempSize), i * (600 / tempSize)); // 设置位置
                tiles[i][j] = tile;
                tile->lower();
                tile->show(); // 显示瓷砖
            }
        }
    }

    // 自动寻路按钮动画及逻辑实现
    connect(autoFoundWay, &ImgButton::clicked, [=](){
        //按钮弹跳
        autoFoundWay->buttonDown();
        autoFoundWay->buttonUp();
        findWay(true);
    });
}

PlayWindow::~PlayWindow()
{
    delete ui;
}

// 自动寻路
bool PlayWindow::findWay(bool drawPath){
    // 是否走过的标记
    bool gone_mark[31][31] = {{false}}; // 初始化为 false
    QStack<QPair<int, int>> possible_path; // 储存路线的栈
    QStack<QPair<int, int>> pos_stack;     // 储存待遍历位置的栈
    pos_stack.push(QPair<int, int>(startX, startY)); // 加入起点

    while (!pos_stack.empty()) {
        auto current_pos = pos_stack.top();
        int pos_x = current_pos.first;
        int pos_y = current_pos.second;
        possible_path.push(current_pos);

        // 如果到达终点
        int lastx = startX;
        int lasty = startY;
        QTimer *timer = new QTimer(this);
        if (pos_x == endX && pos_y == endY) {
            if (drawPath) {
                timer->start(100);
                for (const auto& p : possible_path) {
                    int tx = p.first, ty = p.second;
                    if (tx != startX || ty != startY) {
                        // 自动寻路后逻辑
                        tiles[tx][ty]->changeStatus(STARTING);
                        tiles[lastx][lasty]->changeStatus(PATH);
                        lastx = tx;
                        lasty = ty;
                    }
                }
            }
            return true;
        }

        // 标记为已走过
        gone_mark[pos_x][pos_y] = true;

        bool moved = false;
        for (int i = 0; i < 4; i++) {
            int next_pos_x = pos_x + MOVE_STRATEGY[i][0];
            int next_pos_y = pos_y + MOVE_STRATEGY[i][1];

            // 判断是否撞墙或已走过
            if (next_pos_x >= 0 && next_pos_x < mapData->getMapSize() &&
                next_pos_y >= 0 && next_pos_y < mapData->getMapSize() &&
                mapData->mazeMap[next_pos_x][next_pos_y] != WALL &&
                !gone_mark[next_pos_x][next_pos_y]) {
                pos_stack.push(QPair<int, int>(next_pos_x, next_pos_y));
                moved = true;
                break; // 找到一个可行的方向后，立即移动
            }
        }

        // 如果四个方向都无法前进，则回退
        if (!moved) {
            pos_stack.pop();
            possible_path.pop();
        }
    }

    // 没有找到路径
    if (drawPath)
        QMessageBox::information(this, "迷宫无解", "该迷宫没有通路");

    return false;
}

//监听键盘事件，实现移动
void PlayWindow::keyPressEvent(QKeyEvent *e)
{
    // 按 W，向上移动
    if(e->key() == Qt::Key_W && posX > 0 && mapData->mazeMap[posX-1][posY] > 0)
    {
        tiles[posX][posY]->changeStatus(PATH);
        posX--; // 减少 X 坐标
        tiles[posX][posY]->changeStatus(STARTING);
        if(mapData->mazeMap[posX][posY] == ENDING){
            // 游戏结束
            // qDebug() << "end";
        }
    }

    // 按 S，向下移动
    if(e->key() == Qt::Key_S && posX < mapData->getMapSize() - 1 && mapData->mazeMap[posX+1][posY] > 0)
    {
        tiles[posX][posY]->changeStatus(PATH);
        posX++; // 增加 X 坐标
        tiles[posX][posY]->changeStatus(STARTING);
        if(mapData->mazeMap[posX][posY] == ENDING){
            // 游戏结束
            // qDebug() << "end";
        }
    }

    // 按 A，向左移动
    if(e->key() == Qt::Key_A && posY > 0 && mapData->mazeMap[posX][posY-1] > 0)
    {
        tiles[posX][posY]->changeStatus(PATH);
        posY--; // 减少 Y 坐标
        tiles[posX][posY]->changeStatus(STARTING);
        if(mapData->mazeMap[posX][posY] == ENDING){
            // 游戏结束
            // qDebug() << "end";
        }
    }

    // 按 D，向右移动
    if(e->key() == Qt::Key_D && posY < mapData->getMapSize() - 1 && mapData->mazeMap[posX][posY+1] > 0)
    {
        tiles[posX][posY]->changeStatus(PATH);
        posY++; // 增加 Y 坐标
        tiles[posX][posY]->changeStatus(STARTING);
        if(mapData->mazeMap[posX][posY] == ENDING){
            // 游戏结束
            // qDebug() << "end";
        }
    }
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
