#include "playwindow.h"
#include "imgbutton.h"
#include "mapdata.h"
#include "qdir.h"
#include "qpropertyanimation.h"
#include "qstandardpaths.h"
#include "ui_playwindow.h"
#include "tiletexture.h"
#include "mazegenerator.h"
#include "taskhandlerwindow.h"

#include <QPainter>
#include <QVector>
#include <QKeyEvent>
#include <QStack>
#include <QMessageBox>
#include <QTimer>
#include <QQueue>

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
    mazeVector.append(mapData);
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

PlayWindow::PlayWindow(MapData *md, QString username, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::PlayWindow)
{
    ui->setupUi(this);
    this->username = username;
    initializeLeaderboardFile();
    // qDebug() << username;
    winSound = new QSoundEffect;
    winSound->setSource(QUrl("qrc:/res/LevelWinSound.wav"));
    winSound->setLoopCount(1); // 循环次数
    winSound->setVolume(0.5f); // 音量 0~1之间
    this->mapData = md;
    mazeVector.append(md);
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
                } else if (this->mapData->mazeMap[i][j] == 3){
                    status = ENDING; // 3表示目的地
                    this->endX = i;
                    this->endY = j;
                } else {
                    status = TASK;
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

    // 初始化计时器
    gameTimer = new QTimer(this);
    connect(gameTimer, &QTimer::timeout, this, &PlayWindow::updateTimeDisplay);

    // 初始化时间显示标签
    timeLabel = new QLabel(this);
    timeLabel->setText("Time: 00:00");
    timeLabel->setGeometry(620, 30, 150, 30); // 设置时间显示的位置
    timeLabel->setStyleSheet("font-size: 18px; color: white;");

    // 设置游戏开始时间
    elapsedTime = QTime(0, 0);

    // 开始游戏计时
    gameTimer->start(1000); // 每秒更新一次

    levelLabel = new QLabel(this);
    levelLabel->setText("第" + QString::number(mapId + 1) +"关");
    levelLabel->setGeometry(620, 80, 150, 30);
    levelLabel->setStyleSheet("font-size: 18px; color: white;");

    scoreLabel = new QLabel(this);
    scoreLabel->setText("分数：" + QString::number(user_score));
    scoreLabel->setGeometry(620, 140, 150, 30);
    scoreLabel->setStyleSheet("font-size: 18px; color: white;");

    // 自动寻路按钮动画及逻辑实现
    connect(autoFoundWay, &ImgButton::clicked, [=](){
        //按钮弹跳
        autoFoundWay->buttonDown();
        autoFoundWay->buttonUp();
        if(!inTaskMaze){
            findWay(true);
        }else{
            multiTargetFindWay(true);
        }
    });
    initializeFogOfWar();
}

PlayWindow::~PlayWindow()
{
    saveLeaderboard();
    delete ui;
}

void PlayWindow::showLeaderboard() {
    // 确保排行榜已经加载并排序
    loadLeaderboard();

    QString leaderboardText;

    // 排行榜为空时显示提示
    if (leaderboard.isEmpty()) {
        leaderboardText = "暂无数据";
    } else {
        // 遍历排行榜并格式化显示
        for (const PlayerScore &score : leaderboard) {
            leaderboardText += score.name + ": " + QString::number(score.score) + "\n";
        }
    }

    qDebug() << "排行榜内容: " << leaderboardText;  // 调试输出显示的内容

    QMessageBox::information(this, "排行榜", leaderboardText);
}

void PlayWindow::loadLeaderboard() {
    QFile file(leaderboardFile);

    // 打开文件进行读取
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qDebug() << "无法打开排行榜文件";
        return;
    }

    QTextStream in(&file);
    leaderboard.clear();  // 清空现有数据

    // 读取每行数据并解析
    while (!in.atEnd()) {
        QString line = in.readLine();
        qDebug() << "读取行: " << line;  // 输出读取的每一行
        QStringList parts = line.split(",");

        if (parts.size() == 2) {
            QString name = parts[0].trimmed();  // 去除多余的空格
            int score = parts[1].trimmed().toInt();  // 转为整数类型
            leaderboard.append(PlayerScore{name, score});
        } else {
            qDebug() << "无效格式: " << line;
        }
    }

    file.close();

    // 排序，分数高的排前面
    std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore &a, const PlayerScore &b) {
        return a.score > b.score;  // 按分数从高到低排序
    });

    // 输出排序后的排行榜
    for (const PlayerScore &score : leaderboard) {
        qDebug() << score.name << ": " << score.score;
    }
}

void PlayWindow::saveLeaderboard() {
    QFile file(leaderboardFile);

    // 打开文件进行写入
    if (!file.open(QIODevice::WriteOnly | QIODevice::Text)) {
        qDebug() << "无法打开排行榜文件";
        return;
    }

    QTextStream out(&file);
    // 遍历排行榜并保存
    for (const PlayerScore &score : leaderboard) {
        out << score.name << "," << score.score << "\n";
    }

    file.close();
}

void PlayWindow::addScoreToLeaderboard(const QString &name, int score) {
    loadLeaderboard();  // 加载现有的排行榜数据

    // 查找是否已有该用户名的记录
    bool playerFound = false;
    for (PlayerScore &existingScore : leaderboard) {
        if (existingScore.name == name) {
            // 如果用户已经存在，则比较并保留较高的分数
            if (existingScore.score < score) {
                existingScore.score = score;  // 更新为更高的分数
            }
            playerFound = true;
            break;
        }
    }

    // 如果没有找到该玩家的记录，则添加新的玩家及分数
    if (!playerFound) {
        leaderboard.append(PlayerScore{name, score});
    }

    // 排序，分数高的排前面
    std::sort(leaderboard.begin(), leaderboard.end(), [](const PlayerScore &a, const PlayerScore &b) {
        return a.score > b.score;  // 按分数从高到低排序
    });

    // 保证最多保存前10名
    if (leaderboard.size() > 10) {
        leaderboard.removeLast();
    }

    saveLeaderboard();  // 保存更新后的排行榜
}

bool PlayWindow::isVisibleByRayCasting(int startX, int startY, int targetX, int targetY) {
    int dx = abs(targetX - startX);
    int dy = abs(targetY - startY);

    int sx = (startX < targetX) ? 1 : -1;
    int sy = (startY < targetY) ? 1 : -1;

    int err = dx - dy;

    int currentX = startX;
    int currentY = startY;

    while (currentX != targetX || currentY != targetY) {
        // 如果经过墙壁，目标格子不可见
        if (mapData->mazeMap[currentX][currentY] == WALL) {
            return false;
        }

        int e2 = err * 2;
        if (e2 > -dy) {
            err -= dy;
            currentX += sx;
        }
        if (e2 < dx) {
            err += dx;
            currentY += sy;
        }
    }

    return true; // 没有被墙壁阻挡
}

void PlayWindow::stopGame(bool isWon)
{
    //胜利播放胜利音效
    winSound->play();
    //标签落下动画
    ui->congraLabel->raise();
    QPropertyAnimation * anima =  new QPropertyAnimation(ui->congraLabel,"geometry");
    anima->setDuration(1000);
    anima->setStartValue(QRect(ui->congraLabel->x(),ui->congraLabel->y(),ui->congraLabel->width(),ui->congraLabel->height()));
    anima->setEndValue(QRect(ui->congraLabel->x(),ui->congraLabel->y()+150,ui->congraLabel->width(),ui->congraLabel->height()));
    anima->setEasingCurve(QEasingCurve::OutBounce);
    anima->start();
    // 使用QEventLoop等待动画完成
    QEventLoop loop;
    connect(anima, &QPropertyAnimation::finished, &loop, &QEventLoop::quit);  // 动画结束时退出循环
    anima->start();
    loop.exec();  // 等待动画完成
}

void PlayWindow::initializeFogOfWar() {
    for (int x = 0; x < this->mapData->getMapSize(); x++) {
        for (int y = 0; y < this->mapData->getMapSize(); y++) {
            tiles[x][y]->setFog(true); // 设置所有格子为迷雾
        }
    }

    // updateFogOfWar(posX, posY); // 设置玩家初始位置的可见范围
}

void PlayWindow::updateFogOfWar(int centerX, int centerY) {
    for (int x = 0; x < this->mapData->getMapSize(); x++) {
        for (int y = 0; y < this->mapData->getMapSize(); y++) {
            int distance = abs(x - centerX) + abs(y - centerY);

            // 超出视野范围，保持迷雾
            if (distance > visionRadius) {
                tiles[x][y]->setFog(true);
                continue;
            }

            // 使用光线投射判断可见性
            if (isVisibleByRayCasting(centerX, centerY, x, y)) {
                tiles[x][y]->setFog(false); // 可见
            } else {
                tiles[x][y]->setFog(true); // 被墙遮挡
            }
        }
    }
}

void PlayWindow::updateFogOfWarOptimized(int oldX, int oldY, int newX, int newY) {
    // 恢复旧位置附近的迷雾
    for (int x = oldX - visionRadius; x <= oldX + visionRadius; x++) {
        for (int y = oldY - visionRadius; y <= oldY + visionRadius; y++) {
            if (x < 0 || x >= this->mapData->getMapSize() || y < 0 || y >= this->mapData->getMapSize()) continue;
            tiles[x][y]->setFog(true);
        }
    }

    // 更新新位置附近的可见范围
    for (int x = newX - visionRadius; x <= newX + visionRadius; x++) {
        for (int y = newY - visionRadius; y <= newY + visionRadius; y++) {
            if (x < 0 || x >= this->mapData->getMapSize() || y < 0 || y >= this->mapData->getMapSize()) continue;

            int distance = abs(x - newX) + abs(y - newY);

            if (distance > visionRadius) {
                continue;
            }

            // 使用光线投射判断可见性
            if (isVisibleByRayCasting(newX, newY, x, y)) {
                tiles[x][y]->setFog(false);
            }
        }
    }
}

void PlayWindow::enterDeeperMaze(int taskX, int taskY)
{
    qDebug() << "Entering deeper maze from task point:" << taskX << taskY;

    // 阻塞逻辑，直到任务完成
    QEventLoop loop;
    taskMap++;  // 增加任务层级
    isStart = false;  // 停止当前迷宫的状态
    mapData->mazeMap[taskX][taskY] = STARTING; // 更新当前任务点为起始点

    // 执行进入任务迷宫的逻辑
    if(enterTaskMaze()){  // 进入新迷宫
        loop.quit();  // 任务完成后退出事件循环
    }
    // 阻塞直到任务完成
    loop.exec();
}

bool PlayWindow::multiTargetFindWay(bool drawPath)
{
    // 存储所有任务点的坐标
    QVector<QPair<int, int>> taskPoints;
    for (int x = 0; x < mapData->getMapSize(); x++) {
        for (int y = 0; y < mapData->getMapSize(); y++) {
            if (mapData->mazeMap[x][y] == STAR) { // STAR 为任务点的标志
                taskPoints.append(QPair<int, int>(x, y));
            }
            /*
            if (mapData->mazeMap[x][y] == TASK) { // STAR 为任务点的标志
                taskPoints.append(QPair<int, int>(x, y));
            }*/
        }
    }

    // 当前起点
    QPair<int, int> currentPos(posX, posY);

    while (!taskPoints.isEmpty()) {
        // 寻找最近的任务点
        QPair<int, int> nextTaskPoint = findClosestPoint(currentPos, taskPoints);
        if (!findPathTo(currentPos, nextTaskPoint, drawPath)) {
            QMessageBox::information(this, "提示", "无法找到任务点路径");
            return false; // 无法到达任务点
        }
        currentPos = nextTaskPoint;

        // 如果任务点是TASK点，进入更深层迷宫
        if (mapData->mazeMap[currentPos.first][currentPos.second] == TASK) {
            enterDeeperMaze(currentPos.first, currentPos.second);
            return true; // 成功进入更深层迷宫后返回
        }else{
            // 执行任务逻辑
            executeTaskAt(currentPos.first, currentPos.second);
        }

        // 从任务点集合中移除已完成的任务点
        taskPoints.removeOne(currentPos);
    }

    // 返回入口
    QPair<int, int> entrance(startX, startY);
    if (!findPathTo(currentPos, entrance, drawPath, true)) {
        QMessageBox::information(this, "提示", "无法返回入口");
        return false; // 无法返回入口
    }

    // 执行返回上一层逻辑
    returnToPreviousMaze();
    return true;
}

// 找到最近的目标点（基于 BFS 计算最短路径距离）
QPair<int, int> PlayWindow::findClosestPoint(QPair<int, int> start, QVector<QPair<int, int>> &targets)
{
    QQueue<QPair<int, int>> queue;
    QMap<QPair<int, int>, int> distance;
    QMap<QPair<int, int>, QPair<int, int>> prev_map;
    queue.enqueue(start);
    distance[start] = 0;

    while (!queue.isEmpty()) {
        QPair<int, int> current = queue.dequeue();
        int currentDist = distance[current];

        // 遍历四个方向
        for (int i = 0; i < 4; i++) {
            int nextX = current.first + MOVE_STRATEGY[i][0];
            int nextY = current.second + MOVE_STRATEGY[i][1];
            QPair<int, int> next(nextX, nextY);

            if (nextX < 0 || nextX >= mapData->getMapSize() || nextY < 0 || nextY >= mapData->getMapSize()) {
                continue; // 越界检查
            }
            if (mapData->mazeMap[nextX][nextY] == WALL || distance.contains(next)) {
                continue; // 墙或已访问
            }

            distance[next] = currentDist + 1;
            prev_map[next] = current;
            queue.enqueue(next);
        }
    }

    // 寻找距离最近的任务点
    QPair<int, int> closestPoint = targets[0];
    int minDist = INT_MAX;
    for (const auto &target : targets) {
        if (distance.contains(target) && distance[target] < minDist) {
            minDist = distance[target];
            closestPoint = target;
        }
    }
    return closestPoint;
}

bool PlayWindow::findPathTo(QPair<int, int> start, QPair<int, int> target, bool drawPath, bool isReturnPath)
{
    QQueue<QPair<int, int>> queue;
    QMap<QPair<int, int>, QPair<int, int>> prev_map;
    QMap<QPair<int, int>, bool> visited;

    queue.enqueue(start);
    visited[start] = true;

    while (!queue.isEmpty()) {
        QPair<int, int> current = queue.dequeue();

        if (current == target) {
            // 构建路径
            if (drawPath) {
                QVector<QPair<int, int>> path;
                QPair<int, int> p = current;
                while (p != start) {
                    path.append(p);
                    p = prev_map[p];
                }
                path.append(start);
                std::reverse(path.begin(), path.end());

                // 绘制路径
                drawPathOnMap(path, isReturnPath);
            }
            return true;
        }

        for (int i = 0; i < 4; i++) {
            int nextX = current.first + MOVE_STRATEGY[i][0];
            int nextY = current.second + MOVE_STRATEGY[i][1];
            QPair<int, int> next(nextX, nextY);

            if (nextX < 0 || nextX >= mapData->getMapSize() || nextY < 0 || nextY >= mapData->getMapSize()) {
                continue; // 越界检查
            }
            if (mapData->mazeMap[nextX][nextY] == WALL || visited.contains(next)) {
                continue; // 墙或已访问
            }

            visited[next] = true;
            prev_map[next] = current;
            queue.enqueue(next);
        }
    }
    return false; // 无法找到路径
}

// 绘制路径
void PlayWindow::drawPathOnMap(const QVector<QPair<int, int>> &path, bool isReturnPath)
{
     // 显示路径
    int lastx = startX, lasty = startY;
    for (const auto &p : path) {
        int tx = p.first, ty = p.second;
        tiles[tx][ty]->changeStatus(STARTING);
        if(!isReturnPath){
            tiles[lastx][lasty]->changeStatus(ROAD);
        }else{
            tiles[lastx][lasty]->changeStatus(RETURN);
        }
        // 更新迷雾
        updateFogOfWarOptimized(lastx, lasty, tx, ty);
        lastx = tx;
        lasty = ty;
        repaint();
        sleep(50);
        tiles[startX][startY]->changeStatus(ENDING);
    }
}

// 执行任务函数
void PlayWindow::executeTaskAt(int x, int y)
{
    qDebug() << "Executing task at:" << x << "," << y;
    thw = new TaskHandlerWindow();
    thw->show();

    // 阻塞逻辑，直到任务完成
    QEventLoop loop;
    QObject::connect(thw, &TaskHandlerWindow::correct, [&]() {
        mapData->setIsFinished(mapData->getIsFinished() + 1);
        user_score += 100;
        scoreLabel->setText("分数：" + QString::number(user_score));
        thw->close();
        thw = nullptr;
        loop.quit(); // 退出事件循环
    });
    loop.exec(); // 开启事件循环，阻塞当前逻辑
}

// 返回上一层迷宫函数
void PlayWindow::returnToPreviousMaze()
{
    qDebug() << "Returning to previous maze...";
    MapData *temp = mapData;
    mazeVector.removeLast();
    clearCurrentMaze();
    mapId--;
    taskMap--;
    qDebug() << taskMap;
    this->mapData = mazeVector[mapId];
    if(taskMap < 0){
        levelLabel->setText("第" + QString::number(mapId + 1) + "关");
        inTaskMaze = false;
        taskMap = 0;
        // 创建TileTexture对象并添加到窗口
        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                TileStatus status;
                if (this->mapData->mazeMap[i][j] == 0) {
                    status = WALL; // 0表示墙
                } else if (this->mapData->mazeMap[i][j] == 1) {
                    status = PATH; // 1表示通路
                } else if (this->mapData->mazeMap[i][j] == 2) {
                    status = STARTING; // 2表示起始点
                    this->posX = i;
                    this->posY = j;
                    this->mapData->mazeMap[i][j] = 1;
                } else if (this->mapData->mazeMap[i][j] == 3){
                    status = ENDING; // 3表示目的地
                    this->endX = i;
                    this->endY = j;
                } else if (this->mapData->mazeMap[i][j] == 4){
                    status = TASK;
                }else{
                    status = STAR;
                }
                if(mapId != 0 && i == 1 && j == 1){
                    status = LASTLAYER;
                }else if(mapId == 0 && i == 1 && j == 1){
                    status = PATH;
                }
                // 创建TileTexture对象
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this); // 设置父对象
                tile->setSize(600 / this->mapData->getMapSize());
                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                tiles[i][j] = tile;
                tile->lower();
                tile->show(); // 显示瓷砖
            }
        }
        delete temp;
        initializeFogOfWar();
    }else{
        levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                TileStatus status;
                if (this->mapData->mazeMap[i][j] == 0) {
                    status = WALL;
                } else if (this->mapData->mazeMap[i][j] == 1) {
                    status = PATH;
                } else if (this->mapData->mazeMap[i][j] == 2) {
                    this->posX = i;
                    this->posY = j;
                    status = STARTING;
                    this->mapData->mazeMap[i][j] = 1;
                }else if (this->mapData->mazeMap[i][j] == 4){
                    status = TASK;
                }else if (this->mapData->mazeMap[i][j] == 5){
                    status = STAR;
                }
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this);
                tile->setSize(600 / this->mapData->getMapSize());
                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize()));
                tiles[i][j] = tile;
                tile->lower();
                tile->show();
            }
        }
        initializeFogOfWar();
    }
}


void PlayWindow::updateTimeDisplay()
{
    // 增加一秒钟
    elapsedTime = elapsedTime.addSecs(1);

    // 更新显示的文本
    timeLabel->setText(QString("Time: %1:%2")
                           .arg(elapsedTime.minute(), 2, 10, QChar('0'))
                           .arg(elapsedTime.second(), 2, 10, QChar('0')));
}


void PlayWindow::askLastLevel()
{
    // 创建一个消息框
    QMessageBox msgBox;
    msgBox.setWindowTitle("上一关");
    msgBox.setText("是否进入上一关？");

    // 添加按钮
    QPushButton *lastButton = msgBox.addButton("上一关", QMessageBox::YesRole);
    QPushButton *exitButton = msgBox.addButton("取消", QMessageBox::NoRole);

    // 设置消息框为模态，确保在此消息框未关闭前用户不能操作其他界面
    msgBox.exec();
    isStart = false;
    // 判断用户选择的按钮
    if (msgBox.clickedButton() == lastButton) {
        // 重置计时器
        elapsedTime = QTime(0, 0);
        timeLabel->setText("Time: 00:00");
        clearCurrentMaze();
        mapId--;
        this->mapData = mazeVector[mapId];
        // 创建TileTexture对象并添加到窗口
        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                TileStatus status;
                if (this->mapData->mazeMap[i][j] == 0) {
                    status = WALL; // 0表示墙
                } else if (this->mapData->mazeMap[i][j] == 1) {
                    status = PATH; // 1表示通路
                } else if (this->mapData->mazeMap[i][j] == 2) {
                    if(mapId != 0){
                        status = LASTLAYER; // 2表示起始点
                    }else{
                        status = PATH;
                    }
                } else if (this->mapData->mazeMap[i][j] == 3){
                    status = ENDING; // 3表示目的地
                    this->endX = i;
                    this->endY = j;
                } else if (this->mapData->mazeMap[i][j] == 4){
                    status = TASK;
                }
                if(mapId != 0 && i == 1 && j == 1){
                    status = LASTLAYER;
                }
                // 创建TileTexture对象
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this); // 设置父对象
                tile->setSize(600 / this->mapData->getMapSize());
                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                tiles[i][j] = tile;
                tile->lower();
                tile->show(); // 显示瓷砖
            }
        }
        initializeFogOfWar();
        this->posX = this->endX;
        this->posY = this->endY;
    } else if (msgBox.clickedButton() == exitButton) {

    }
    levelLabel->setText("第" + QString::number(mapId + 1) +"关");
}

void PlayWindow::askNextLevel()
{
    // 创建一个消息框
    QMessageBox msgBox;
    msgBox.setWindowTitle("下一关");
    msgBox.setText("是否进入下一关？");

    // 添加按钮
    QPushButton *nextButton = msgBox.addButton("下一关", QMessageBox::YesRole);
    QPushButton *exitButton = msgBox.addButton("结束", QMessageBox::NoRole);

    // 设置消息框为模态，确保在此消息框未关闭前用户不能操作其他界面
    msgBox.exec();
    if(msgBox.clickedButton() == exitButton){
        addScoreToLeaderboard(username, user_score);
        showLeaderboard();  // 显示排行榜
        stopGame(true);
        QMessageBox tempMsgBox;
        tempMsgBox.setWindowTitle("通关");
        tempMsgBox.setText("恭喜您已通关！");
        tempMsgBox.exec();
        gameTimer->stop(); // 停止计时器
        QCoreApplication::quit();
    }
    mapId++;
    if(mapId > mapNumbers){
        mapNumbers++;
        // 判断用户选择的按钮
        if (msgBox.clickedButton() == nextButton) {
            // 重置计时器
            elapsedTime = QTime(0, 0);
            timeLabel->setText("Time: 00:00");
            clearCurrentMaze();
            int nextSize = mazeVector[mapNumbers-1]->getMapSize() + 2 <= 29
                            ?mazeVector[mapNumbers-1]->getMapSize() + 2
                            :mazeVector[mapNumbers-1]->getMapSize();
            qDebug() << QString::number(nextSize);
            MazeGenerator *mazeGenerator = new MazeGenerator(nextSize);
            mazeGenerator->generateStartAndEndPoints();
            this->startX = mazeGenerator->getStartPoint().first;
            this->startY = mazeGenerator->getStartPoint().second;
            QVector<QVector<int>>mp = mazeGenerator->getMazeMap();
            int tempMp[31][31];
            for(int i = 0; i < nextSize; i++){
                for(int j = 0; j < nextSize; j++){
                    tempMp[j][i] = mp[i][j];
                }
            }
            mazeVector.append(new MapData(nextSize, tempMp));
            this->mapData = mazeVector[mapId];
            // 创建TileTexture对象并添加到窗口
            for (int i = 0; i < nextSize; ++i) {
                for (int j = 0; j < nextSize; ++j) {
                    TileStatus status;
                    if (this->mapData->mazeMap[i][j] == 0) {
                        status = WALL; // 0表示墙
                    } else if (this->mapData->mazeMap[i][j] == 1) {
                        status = PATH; // 1表示通路
                    } else if (this->mapData->mazeMap[i][j] == 2) {
                        status = STARTING; // 2表示起始点
                    } else if (this->mapData->mazeMap[i][j] == 3){
                        status = ENDING; // 3表示目的地
                        this->endX = i;
                        this->endY = j;
                    } else if (this->mapData->mazeMap[i][j] == 4){
                        status = TASK;
                    }
                    if(mapId != 0 && i == 1 && j == 1){
                        status = LASTLAYER;
                    }
                    // 创建TileTexture对象
                    TileTexture *tile = new TileTexture(status);
                    tile->setParent(this); // 设置父对象
                    tile->setSize(600 / nextSize);
                    tile->move(j * (600 / nextSize), i * (600 / nextSize)); // 设置位置
                    tiles[i][j] = tile;
                    tile->lower();
                    tile->show(); // 显示瓷砖
                }
                this->posX = startX;
                this->posY = startY;
            }
        }
    }else{
        // 重置计时器
        elapsedTime = QTime(0, 0);
        timeLabel->setText("Time: 00:00");
        this->mapData = mazeVector[mapId-1];
        clearCurrentMaze();
        this->mapData = mazeVector[mapId];
        int nextSize = this->mapData->getMapSize();
        // 创建TileTexture对象并添加到窗口
        for (int i = 0; i < nextSize; ++i) {
            for (int j = 0; j < nextSize; ++j) {
                TileStatus status;
                if (this->mapData->mazeMap[i][j] == 0) {
                    status = WALL; // 0表示墙
                } else if (this->mapData->mazeMap[i][j] == 1) {
                    status = PATH; // 1表示通路
                } else if (this->mapData->mazeMap[i][j] == 2) {
                    status = STARTING; // 2表示起始点
                    this->posX = i;
                    this->posY = j;
                } else if (this->mapData->mazeMap[i][j] == 3){
                    status = ENDING; // 3表示目的地
                    this->endX = i;
                    this->endY = j;
                } else {
                    status = TASK;
                }
                if(mapId != 0 && i == 1 && j == 1){
                    status = LASTLAYER;
                }
                // 创建TileTexture对象
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this); // 设置父对象
                tile->setSize(600 / nextSize);
                tile->move(j * (600 / nextSize), i * (600 / nextSize)); // 设置位置
                tiles[i][j] = tile;
                tile->lower();
                tile->show(); // 显示瓷砖
            }
        }
        this->posX = 1;
        this->posY = 1;
    }
    initializeFogOfWar();
    levelLabel->setText("第" + QString::number(mapId + 1) +"关");
}

// 延时
void PlayWindow::sleep(int sec)
{
    QElapsedTimer t;
    t.start();
    while(t.elapsed()<sec);
}

bool PlayWindow::initializeLeaderboardFile() {
    // 获取本地可写目录
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QString leaderboardDirPath = writablePath + "/leaderboard/";
    QDir leaderboardDir(leaderboardDirPath);

    // 如果目录不存在，则创建该目录
    if (!leaderboardDir.exists()) {
        if (!leaderboardDir.mkpath(".")) {
            QMessageBox::critical(nullptr, "错误", "无法创建 leaderboard 目录");
            return false;
        }
    }

    // 初始化排行榜文件
    QString leaderboardFilePath = leaderboardDirPath + "leaderboard.txt";
    qDebug() << leaderboardFilePath;

    QFile file(leaderboardFilePath);

    // 如果排行榜文件不存在，从资源文件中复制初始数据
    if (!file.exists()) {
        QString resourceFilePath = ":/leaderboard.txt";  // 从资源文件中读取
        QFile resourceFile(resourceFilePath);
        qDebug() << resourceFilePath;

        if (resourceFile.open(QIODevice::ReadOnly)) {
            // 打开排行榜文件进行写入
            if (file.open(QIODevice::WriteOnly)) {
                QTextStream in(&resourceFile);
                QTextStream out(&file);
                out << in.readAll();  // 将资源文件的内容复制到本地文件
                file.close();
            } else {
                resourceFile.close();
                QMessageBox::critical(nullptr, "错误", "无法创建本地排行榜文件");
                return false;  // 无法创建本地文件
            }
            resourceFile.close();
        } else {
            QMessageBox::critical(nullptr, "错误", "无法从资源文件读取排行榜文件");
            return false;  // 无法从资源文件读取
        }
    }

    return true;  // 成功初始化排行榜文件
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

void PlayWindow::clearCurrentMaze() {
    // 删除之前的迷宫贴图
    for (int i = 0; i < mapData->getMapSize(); ++i) {
        for (int j = 0; j < mapData->getMapSize(); ++j) {
            if (tiles[i][j]) {
                delete tiles[i][j];  // 删除单元格
                tiles[i][j] = nullptr;  // 清空指针
            }
        }
    }
}


// 生成任务迷宫
void PlayWindow::generateTaskMaze() {
    MazeGenerator *taskGen = new MazeGenerator(this->mapData->getMapSize());
    taskGen->generateStartAndEndPoints(true);
    taskGen->generateTaskPoints();
    taskMaze = taskGen->getMazeMap();
    int tempMap[31][31];
    for(int i = 0; i < this->mapData->getMapSize(); i++){
        for(int j = 0; j < this->mapData->getMapSize(); j++){
            tempMap[i][j] = taskMaze[i][j];
        }
    }
    taskMapData = new MapData(this->mapData->getMapSize(), tempMap);
    taskMapData->setTaskPointCount(taskGen->getTaskPointCount());
    this->startX = taskGen->getEndPoint().first;
    this->startY = taskGen->getEndPoint().second;
    mazeVector.append(taskMapData);
    mapId++;
    levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
    this->mapData = mazeVector[mapId];
    this->mapData->startX = startX;
    this->mapData->startY = startY;
    delete taskGen;
}

// 进入任务迷宫
bool PlayWindow::enterTaskMaze()
{
    this->inTaskMaze = true;
    // 清空当前迷宫的状态
    clearCurrentMaze();

    // 生成新的任务迷宫
    generateTaskMaze();

    // 显示任务迷宫
    int taskSize = taskMaze.size();
    this->posX = this->startX;
    this->posY = this->startY;
    if (taskSize > 0) {
        for (int i = 0; i < taskSize; ++i) {
            for (int j = 0; j < taskSize; ++j) {
                TileStatus status;
                if (taskMaze[i][j] == 0) {
                    status = WALL;
                } else if (taskMaze[i][j] == 1) {
                    status = PATH;
                } else if (taskMaze[i][j] == 3) {
                    status = STARTING;
                }else if (taskMaze[i][j] == 4){
                    status = TASK;
                }else{
                    status = STAR;
                }
                TileTexture *tile = new TileTexture(status);
                tile->setParent(this);
                tile->setSize(600 / taskSize);
                tile->move(j * (600 / taskSize), i * (600 / taskSize));
                tiles[i][j] = tile;
                tile->lower();
                tile->show();
            }
        }
    }
    initializeFogOfWar();
    return true;
}

// BFS寻路
bool PlayWindow::findWay(bool drawPath)
{
    // 是否走过的标记
    bool gone_mark[31][31] = {{false}};
    // 路径记录，保存每个点的前驱，用于重建路径
    QMap<QPair<int, int>, QPair<int, int>> prev_map;

    QQueue<QPair<int, int>> queue; // BFS 队列
    queue.enqueue(QPair<int, int>(posX, posY));
    gone_mark[posX][posY] = true;

    while (!queue.isEmpty())
    {
        int pos_x = queue.head().first;
        int pos_y = queue.head().second;
        queue.dequeue();

        // 终点出口
        if (pos_x == endX && pos_y == endY)
        {
            if (drawPath)
            {
                QVector<QPair<int, int>> final_path;
                QPair<int, int> current = QPair<int, int>(endX, endY);
                while (current != QPair<int, int>(posX, posY))
                {
                    final_path.append(current);
                    current = prev_map[current];
                }
                final_path.append(QPair<int, int>(posX, posY));
                std::reverse(final_path.begin(), final_path.end());

                // 显示路径
                int lastx = startX, lasty = startY;
                for (const auto &p : final_path)
                {
                    int tx = p.first, ty = p.second;
                    tiles[tx][ty]->changeStatus(STARTING);
                    if(lastx != startX || lasty != startY){
                        tiles[lastx][lasty]->changeStatus(ROAD);
                        // 更新迷雾
                        updateFogOfWarOptimized(lastx, lasty, tx, ty);
                    }else{
                        tiles[lastx][lasty]->changeStatus(PATH);
                    }
                    if (mapData->mazeMap[tx][ty] == ENDING)
                    {
                        // 游戏结束
                        askNextLevel();
                    }
                    if (mapData->mazeMap[tx][ty] == TASK)
                    {
                        isStart = false;
                        mapData->mazeMap[tx][ty] = STARTING;
                        enterTaskMaze(); // 进入任务迷宫
                        return true;
                    }
                    lastx = tx;
                    lasty = ty;
                    repaint();
                    sleep(50);
                }
                tiles[posX][posY]->changeStatus(STARTING);
                tiles[endX][endY]->changeStatus(ENDING);
            }
            return true; // 找到路径
        }

        // 探索相邻节点
        for (int i = 0; i < 4; i++)
        {
            int next_pos_x = pos_x + MOVE_STRATEGY[i][0];
            int next_pos_y = pos_y + MOVE_STRATEGY[i][1];

            // 是否撞墙或已走过
            if (next_pos_x < 0 || next_pos_x >= mapData->getMapSize() ||
                next_pos_y < 0 || next_pos_y >= mapData->getMapSize() ||
                mapData->mazeMap[next_pos_x][next_pos_y] == WALL ||
                gone_mark[next_pos_x][next_pos_y])
            {
                continue;
            }

            // 标记已访问，并记录前驱
            gone_mark[next_pos_x][next_pos_y] = true;
            prev_map[QPair<int, int>(next_pos_x, next_pos_y)] = QPair<int, int>(pos_x, pos_y);
            queue.enqueue(QPair<int, int>(next_pos_x, next_pos_y));
        }
    }

    if (drawPath)
        QMessageBox::information(this, "提示", "该迷宫没有通路");
    return false; // 没有找到路径
}

//监听键盘事件，实现移动
void PlayWindow::keyPressEvent(QKeyEvent *e)
{
    this->mapData = mazeVector[mapId];
    if (!inTaskMaze) {
        if(mapId != 0 && ((this->posX != 1 || this->posY != 1) || !isStart)){
            tiles[1][1]->changeStatus(LASTLAYER);
        }
        mapData->mazeMap[startX][startY] = PATH;
        isStart = true;
        if(posX != endX || posY != endY){
            tiles[endX][endY]->changeStatus(ENDING);
        }
        // 按 W，向上移动
        if(e->key() == Qt::Key_W && posX > 0 && mapData->mazeMap[posX-1][posY] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posX--; // 减少 X 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapId != 0 &&
                isStart == true
                && posX == 1
                && posY == 1){
                askLastLevel();
            }
            if(isStart == true && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                askNextLevel();
            }
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX+1, posY, posX, posY);
        }

        // 按 S，向下移动
        if(e->key() == Qt::Key_S && posX < mapData->getMapSize() - 1 && mapData->mazeMap[posX+1][posY] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posX++; // 增加 X 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapId != 0 &&
                isStart == true
                && posX == 1
                && posY == 1){
                askLastLevel();
            }
            if(isStart == true && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                askNextLevel();
            }
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX-1, posY, posX, posY);
        }

        // 按 A，向左移动
        if(e->key() == Qt::Key_A && posY > 0 && mapData->mazeMap[posX][posY-1] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posY--; // 减少 Y 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapId != 0 &&
                isStart == true
                && posX == 1
                && posY == 1){
                askLastLevel();
            }
            if(isStart == true && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                askNextLevel();
            }
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX, posY+1, posX, posY);
        }

        // 按 D，向右移动
        if(e->key() == Qt::Key_D && posY < mapData->getMapSize() - 1 && mapData->mazeMap[posX][posY+1] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posY++; // 增加 Y 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapId != 0 &&
                isStart == true
                && posX == 1
                && posY == 1){
                askLastLevel();
            }
            if(isStart == true && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                askNextLevel();
            }
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX, posY-1, posX, posY);
        }
    } else {
        startX = this->mapData->startX;
        startY = this->mapData->startY;
        // 在任务迷宫中同样应用栈中的数据
        if(posX != this->mapData->startX || posY != this->mapData->startY){
            tiles[this->mapData->startX][this->mapData->startY]->changeStatus(ENDING);
            this->mapData->mazeMap[this->mapData->startX][this->mapData->startY] = ENDING;
        }
        // 按 W，向上移动
        if(e->key() == Qt::Key_W && posX > 0 && mapData->mazeMap[posX-1][posY] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posX--; // 减少 X 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
                return;
            }
            if(mapData->mazeMap[posX][posY] == STAR){
                mapData->mazeMap[posX][posY] = PATH;
                thw = new TaskHandlerWindow();
                thw->show();
                QObject::connect(thw, &TaskHandlerWindow::correct, [&]() {
                    thw->close();
                    mapData->setIsFinished(mapData->getIsFinished() + 1);
                    user_score += 100;
                    scoreLabel->setText("分数：" + QString::number(user_score));
                    thw = NULL;
                    return;
                });
            }
            if(mapData->getIsFinished() == mapData->getTaskPointCount()
                && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                if(mapData->getIsFinished() == mapData->getTaskPointCount()){
                    MapData *temp = mapData;
                    mazeVector.removeLast();
                    clearCurrentMaze();
                    mapId--;
                    taskMap--;
                    this->mapData = mazeVector[mapId];
                    if(taskMap <= 0){
                        levelLabel->setText("第" + QString::number(mapId + 1) + "关");
                        inTaskMaze = false;
                        taskMap = 0;
                        // 创建TileTexture对象并添加到窗口
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL; // 0表示墙
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH; // 1表示通路
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    status = STARTING; // 2表示起始点
                                    this->posX = i;
                                    this->posY = j;
                                    this->mapData->mazeMap[i][j] = 1;
                                } else if (this->mapData->mazeMap[i][j] == 3){
                                    status = ENDING; // 3表示目的地
                                    this->endX = i;
                                    this->endY = j;
                                } else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else{
                                    status = STAR;
                                }
                                if(mapId != 0 && i == 1 && j == 1){
                                    status = LASTLAYER;
                                }else if(mapId == 0 && i == 1 && j == 1){
                                    status = PATH;
                                }
                                // 创建TileTexture对象
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this); // 设置父对象
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show(); // 显示瓷砖
                            }
                        }
                        delete temp;
                        initializeFogOfWar();
                    }else{
                        levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL;
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH;
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    this->posX = i;
                                    this->posY = j;
                                    status = STARTING;
                                    this->mapData->mazeMap[i][j] = 1;
                                }else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else if (this->mapData->mazeMap[i][j] == 5){
                                    status = STAR;
                                }
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this);
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize()));
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show();
                            }
                        }
                        initializeFogOfWar();
                    }
                }
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX+1, posY, posX, posY);
        }

        // 按 S，向下移动
        if(e->key() == Qt::Key_S && posX < mapData->getMapSize() - 1 && mapData->mazeMap[posX+1][posY] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posX++; // 增加 X 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
                return;
            }
            if(mapData->mazeMap[posX][posY] == STAR){
                mapData->mazeMap[posX][posY] = PATH;
                thw = new TaskHandlerWindow();
                thw->show();
                QObject::connect(thw, &TaskHandlerWindow::correct, [&]() {
                    thw->close();
                    mapData->setIsFinished(mapData->getIsFinished() + 1);
                    user_score += 100;
                    scoreLabel->setText("分数：" + QString::number(user_score));
                    thw = NULL;
                    return;
                });
            }
            // 游戏结束
            if(mapData->getIsFinished() == mapData->getTaskPointCount()
                && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                if(mapData->getIsFinished() == mapData->getTaskPointCount()){
                    MapData *temp = mapData;
                    mazeVector.removeLast();
                    clearCurrentMaze();
                    mapId--;
                    taskMap--;
                    this->mapData = mazeVector[mapId];
                    if(taskMap <= 0){
                        levelLabel->setText("第" + QString::number(mapId + 1) + "关");
                        inTaskMaze = false;
                        taskMap = 0;
                        // 创建TileTexture对象并添加到窗口
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL; // 0表示墙
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH; // 1表示通路
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    status = STARTING; // 2表示起始点
                                    this->mapData->mazeMap[i][j] = 1;
                                } else if (this->mapData->mazeMap[i][j] == 3){
                                    status = ENDING; // 3表示目的地
                                    this->endX = i;
                                    this->endY = j;
                                } else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else{
                                    status = STAR;
                                }
                                if(mapId != 0 && i == 1 && j == 1){
                                    status = LASTLAYER;
                                }else if(mapId == 0 && i == 1 && j == 1){
                                    status = PATH;
                                }
                                // 创建TileTexture对象
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this); // 设置父对象
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show(); // 显示瓷砖
                            }
                        }
                        delete temp;
                        initializeFogOfWar();
                    }else{
                        levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL;
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH;
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    this->posX = i;
                                    this->posY = j;
                                    status = STARTING;
                                    this->mapData->mazeMap[i][j] = 1;
                                }else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else if (this->mapData->mazeMap[i][j] == 5){
                                    status = STAR;
                                }
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this);
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize()));
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show();
                            }
                        }
                        initializeFogOfWar();
                    }
                }
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX-1, posY, posX, posY);
        }

        // 按 A，向左移动
        if(e->key() == Qt::Key_A && posY > 0 && mapData->mazeMap[posX][posY-1] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posY--; // 减少 Y 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
                return;
            }
            if(mapData->mazeMap[posX][posY] == STAR){
                mapData->mazeMap[posX][posY] = PATH;
                thw = new TaskHandlerWindow();
                thw->show();
                QObject::connect(thw, &TaskHandlerWindow::correct, [&]() {
                    thw->close();
                    mapData->setIsFinished(mapData->getIsFinished() + 1);
                    user_score += 100;
                    scoreLabel->setText("分数：" + QString::number(user_score));
                    thw = NULL;
                    return;
                });
            }
            // 游戏结束
            if(mapData->getIsFinished() == mapData->getTaskPointCount()
                && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                if(mapData->getIsFinished() == mapData->getTaskPointCount()){
                    MapData *temp = mapData;
                    mazeVector.removeLast();
                    clearCurrentMaze();
                    mapId--;
                    taskMap--;
                    this->mapData = mazeVector[mapId];
                    if(taskMap <= 0){
                        levelLabel->setText("第" + QString::number(mapId + 1) + "关");
                        inTaskMaze = false;
                        taskMap = 0;
                        // 创建TileTexture对象并添加到窗口
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0 && (i != startX || j != startY)) {
                                    status = WALL; // 0表示墙
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH; // 1表示通路
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    status = STARTING; // 2表示起始点
                                    this->posX = i;
                                    this->posY = j;
                                    this->mapData->mazeMap[i][j] = 1;
                                } else if (this->mapData->mazeMap[i][j] == 3){
                                    status = ENDING; // 3表示目的地
                                    this->endX = i;
                                    this->endY = j;
                                } else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else{
                                    status = STAR;
                                }
                                if(mapId != 0 && i == 1 && j == 1){
                                    status = LASTLAYER;
                                }else if(mapId == 0 && i == 1 && j == 1){
                                    status = PATH;
                                }
                                // 创建TileTexture对象
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this); // 设置父对象
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show(); // 显示瓷砖
                            }
                        }
                        delete temp;
                        initializeFogOfWar();
                    }else{
                        levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL;
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH;
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    this->posX = i;
                                    this->posY = j;
                                    status = STARTING;
                                    this->mapData->mazeMap[i][j] = 1;
                                }else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else if (this->mapData->mazeMap[i][j] == 5){
                                    status = STAR;
                                }
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this);
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize()));
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show();
                            }
                        }
                        initializeFogOfWar();
                    }
                }
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX, posY+1, posX, posY);
        }

        // 按 D，向右移动
        if(e->key() == Qt::Key_D && posY < mapData->getMapSize() - 1 && mapData->mazeMap[posX][posY+1] > 0)
        {
            tiles[posX][posY]->changeStatus(PATH);
            posY++; // 增加 Y 坐标
            tiles[posX][posY]->changeStatus(STARTING);
            if(mapData->mazeMap[posX][posY] == TASK){
                taskMap++;
                isStart = false;
                mapData->mazeMap[posX][posY] = STARTING;
                enterTaskMaze();  // 进入任务迷宫
                return;
            }
            if(mapData->mazeMap[posX][posY] == STAR){
                mapData->mazeMap[posX][posY] = PATH;
                thw = new TaskHandlerWindow();
                thw->show();
                QObject::connect(thw, &TaskHandlerWindow::correct, [&]() {
                    thw->close();
                    mapData->setIsFinished(mapData->getIsFinished() + 1);
                    user_score += 100;
                    scoreLabel->setText("分数：" + QString::number(user_score));
                    thw = NULL;
                    return;
                });
            }
            // 游戏结束
            if(mapData->getIsFinished() == mapData->getTaskPointCount()
                && mapData->mazeMap[posX][posY] == ENDING){
                // 游戏结束
                if(mapData->getIsFinished() == mapData->getTaskPointCount()){
                    MapData *temp = mapData;
                    mazeVector.removeLast();
                    clearCurrentMaze();
                    mapId--;
                    taskMap--;
                    this->mapData = mazeVector[mapId];
                    if(taskMap <= 0){
                        levelLabel->setText("第" + QString::number(mapId + 1) + "关");
                        inTaskMaze = false;
                        taskMap = 0;
                        // 创建TileTexture对象并添加到窗口
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL; // 0表示墙
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH; // 1表示通路
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    status = STARTING; // 2表示起始点
                                    this->posX = i;
                                    this->posY = j;
                                    this->mapData->mazeMap[i][j] = 1;
                                } else if (this->mapData->mazeMap[i][j] == 3){
                                    status = ENDING; // 3表示目的地
                                    this->endX = i;
                                    this->endY = j;
                                } else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else{
                                    status = STAR;
                                }
                                if(mapId != 0 && i == 1 && j == 1){
                                    status = LASTLAYER;
                                }else if(mapId == 0 && i == 1 && j == 1){
                                    status = PATH;
                                }
                                // 创建TileTexture对象
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this); // 设置父对象
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize())); // 设置位置
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show(); // 显示瓷砖
                            }
                        }
                        delete temp;
                        initializeFogOfWar();
                    }else{
                        levelLabel->setText("任务" + QString::number(mapId - mapNumbers) + "关");
                        for (int i = 0; i < this->mapData->getMapSize(); ++i) {
                            for (int j = 0; j < this->mapData->getMapSize(); ++j) {
                                TileStatus status;
                                if (this->mapData->mazeMap[i][j] == 0) {
                                    status = WALL;
                                } else if (this->mapData->mazeMap[i][j] == 1) {
                                    status = PATH;
                                } else if (this->mapData->mazeMap[i][j] == 2) {
                                    this->posX = i;
                                    this->posY = j;
                                    status = STARTING;
                                    this->mapData->mazeMap[i][j] = 1;
                                }else if (this->mapData->mazeMap[i][j] == 4){
                                    status = TASK;
                                }else if (this->mapData->mazeMap[i][j] == 5){
                                    status = STAR;
                                }
                                TileTexture *tile = new TileTexture(status);
                                tile->setParent(this);
                                tile->setSize(600 / this->mapData->getMapSize());
                                tile->move(j * (600 / this->mapData->getMapSize()), i * (600 / this->mapData->getMapSize()));
                                tiles[i][j] = tile;
                                tile->lower();
                                tile->show();
                            }
                        }
                        initializeFogOfWar();
                    }
                }
            }
            // 更新迷雾
            updateFogOfWarOptimized(posX, posY-1, posX, posY);
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
