#include "playwindow.h"
#include "imgbutton.h"
#include "mapdata.h"
#include "qdir.h"
#include "qstandardpaths.h"
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
    initializeMapFile();

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

PlayWindow::PlayWindow(MapData *md, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWindow)
{
    ui->setupUi(this);
    this->mapData = md;
    ImgButton *autoFoundWay = new ImgButton(":/res/autoBt.png", ":/res/ButtonSound.wav");
    autoFoundWay->setParent(this);
    autoFoundWay->move(640, this->height()*0.56);
    autoFoundWay->show();

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

// 延时
void PlayWindow::sleep(int sec)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<sec);
}

// 静态函数：初始化地图信息文件
bool PlayWindow::initializeMapFile()
{
    // 获取本地可写目录
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString mapdataDirPath = writablePath + "/mapdata/";
    QDir mapdataDir(mapdataDirPath);
    if (!mapdataDir.exists()) {
        if (!mapdataDir.mkpath(".")) {
            QMessageBox::critical(nullptr, "错误", "无法创建 mapdata 目录");
            return false;
        }
    }
    for(int i = 0; i < 16; i++){
        QString mapdataFilePath = mapdataDirPath + QString::number(i+1) + ".mapdata";
        qDebug() << mapdataFilePath;
        // 如果文件不存在，从资源文件中复制初始数据
        QFile file(mapdataFilePath);
        if (!file.exists()) {
            QString resourceFilePath = ":/mapdata/" + QString::number(i+1) + ".mapdata";
            QFile resourceFile(resourceFilePath);  // 从资源文件读取
            qDebug() << resourceFilePath;
            if (resourceFile.open(QIODevice::ReadOnly)) {
                if (file.open(QIODevice::WriteOnly)) {
                    QTextStream in(&resourceFile);
                    QTextStream out(&file);
                    out << in.readAll();  // 将资源文件的内容复制到本地文件
                    file.close();
                } else {
                    resourceFile.close();
                    QMessageBox::critical(nullptr, "错误", "无法创建本地地图信息文件");
                    return false; // 无法创建本地文件
                }
                resourceFile.close();
            } else {
                QMessageBox::critical(nullptr, "错误", "无法从资源文件读取地图文件");
                return false; // 无法从资源文件读取
            }
        }
    }
    return true; // 成功初始化地图信息文件
}

// 非递归深度优先搜索实现寻路
bool PlayWindow::findWay(bool drawPath)
{
    //是否走过的标记
    bool gone_mark[31][31] = {{false}};

    QStack<QPair<int, int>> possible_path; // 储存路线的栈
    QStack<QPair<int, int>> pos_stack; // 储存待遍历位置的栈，位置的最后一个总是最先遍历
    pos_stack.push(QPair<int, int>(startX, startY)); // 加入起点

    while(!pos_stack.empty())
    {
        int pos_x = pos_stack.top().first;
        int pos_y = pos_stack.top().second;
        possible_path.push(pos_stack.top());

        // 终点出口
        if(pos_x == endX && pos_y == endY)
        {
            // 处理路径显示
            if (drawPath) {
                int lastx = startX, lasty = startY;
                QVector<QPair<int, int>> final_path;
                while(!possible_path.isEmpty()){
                    auto p = possible_path.pop();
                    final_path.append(p);
                }
                for(int i = final_path.size()-1; i >= 0; i--){
                    int tx = final_path[i].first, ty = final_path[i].second;
                    // 自动寻路后逻辑
                    tiles[tx][ty]->changeStatus(STARTING);
                    tiles[lastx][lasty]->changeStatus(PATH);
                    lastx = tx;
                    lasty = ty;
                    repaint();
                    sleep(50);
                }
                tiles[startX][startY]->changeStatus(STARTING);
                tiles[endX][endY]->changeStatus(ENDING);
            }
            return true; // 找到路径
        }

        //如果不是出口则标记走过
        gone_mark[pos_x][pos_y] = true;

        int wall_count=0 , gone_count=0;

        // 探索顺序与MOVE_STRATEGY相反，因为最后入栈的最先遍历
        for(int i=0; i<4; i++)
        {
            int next_pos_x = pos_x + MOVE_STRATEGY[i][0];
            int next_pos_y = pos_y + MOVE_STRATEGY[i][1];
            // 是否撞墙或已走过
            if(next_pos_x < 0 || next_pos_x >= mapData->getMapSize()
            || next_pos_y < 0 || next_pos_y >= mapData->getMapSize()
            || mapData->mazeMap[next_pos_x][next_pos_y] == WALL)
            {
                wall_count++;
                continue;
            }
            if(gone_mark[next_pos_x][next_pos_y])
            {
                gone_count++;
                continue;
            }
            pos_stack.push(QPair<int, int>(next_pos_x, next_pos_y));
        }
        if(wall_count + gone_count == 4)
        {
            pos_stack.pop();
            possible_path.pop();
            if(gone_count != 1)
                possible_path.pop();
        }
    }
    if(drawPath)
        QMessageBox::information(this,"提示","该迷宫没有通路");
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
