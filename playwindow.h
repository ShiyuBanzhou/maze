#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <mapdata.h>
#include "mapdata.h"
#include "qdatetime.h"
#include "taskhandlerwindow.h"
#include "tiletexture.h"

#include <QWidget>
#include <QStack>

namespace Ui {
class PlayWindow;
}

class PlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayWindow(QString mapPath, QWidget *parent = nullptr);
    explicit PlayWindow(MapData *md, QWidget *parent = nullptr);
    ~PlayWindow();
    void setMapPath(QString mapPath); // 设置地图地址
    // 非递归深度优先搜索实现寻路，如果有通路，返回true
    // BFS寻路
    bool findWay(bool drawPath); // 参数设置true，会绘制可行路线，否则不绘制

protected:
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent

private:
    Ui::PlayWindow *ui;
    MapData *mapData; //迷宫数据工具
    MapData *taskMapData;
    TileTexture * tiles[31][31]; //地图图块
    void keyPressEvent(QKeyEvent *) override;    //监听键盘事件，实现移动
    int posX, posY, startX, startY, endX, endY; //当前位置、起点和终点的坐标
    // 探索路径策略
    const int MOVE_STRATEGY[4][2] = {
        {0, 1},     // 向下
        {1, 0},     // 向右
        {0,-1},     // 向上
        {-1,0}      // 向左
    };
    void sleep(int sec);
    // 静态函数：初始化地图信息文件
    // 静态函数：初始化地图信息文件
    static bool initializeMapFile();
    /*
     * 处理嵌套迷宫
    */
    QVector<QVector<int>> currentMaze;  // 当前迷宫的二维数据结构
    QVector<QVector<int>> taskMaze;     // 任务迷宫的二维数据结构
    QVector<MapData*> mazeVector;  // 用于存储多层迷宫的栈

    bool isStart = false;
    int mapId = 0;
    int taskMap = 0;
    int mapNumbers = 0;
    bool enterTaskMaze();  // 进入任务迷宫的函数
    void generateTaskMaze();  // 生成任务迷宫
    void clearCurrentMaze();
    bool inTaskMaze = false;  // 标记是否在任务迷宫中
    void askLastLevel(); // 询问用户是否进入上一关
    void askNextLevel(); // 询问用户是否进入下一关
    TaskHandlerWindow *thw;
    // 计时
    QTimer* gameTimer; // 游戏计时器
    QLabel* timeLabel; // 显示时间的标签
    QTime elapsedTime; // 记录用时
    void updateTimeDisplay();
    // 显示处于第几关
    QLabel* levelLabel;
    // 多任务目标寻路
    bool multiTargetFindWay(bool drawPath);
    // 寻找最近任务点
    QPair<int, int> findClosestPoint(QPair<int, int> start, QVector<QPair<int, int>> &targets);
    // 从当前点寻找路径到目标点
    bool findPathTo(QPair<int, int> start, QPair<int, int> target, bool drawPath, bool isReturnPath = false);
    // 绘制路径
    void drawPathOnMap(const QVector<QPair<int, int>> &path, bool isReturnPath);
    // 执行任务函数
    void executeTaskAt(int x, int y);
    // 返回上一层迷宫函数
    void returnToPreviousMaze();
    // 初始化战争迷雾
    void initializeFogOfWar();
    // 更新战争迷雾
    void updateFogOfWar(int centerX, int centerY);

    void updateFogOfWarOptimized(int oldX, int oldY, int newX, int newY);

    int visionRadius = 3; // 可见范围

    bool isVisibleByRayCasting(int startX, int startY, int targetX, int targetY);

    void enterDeeperMaze(int taskX, int taskY);
};

#endif // PLAYWINDOW_H
