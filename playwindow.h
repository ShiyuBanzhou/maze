#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <mapdata.h>

#include <QWidget>
#include "mapdata.h"
#include "tiletexture.h"

namespace Ui {
class PlayWindow;
}

class PlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayWindow(QString mapPath, QWidget *parent = nullptr);
    ~PlayWindow();
    void setMapPath(QString mapPath); // 设置地图地址
    // 非递归深度优先搜索实现寻路，如果有通路，返回true
    bool findWay(bool drawPath); // 参数设置true，会绘制可行路线，否则不绘制

protected:
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent

private:
    Ui::PlayWindow *ui;
    MapData *mapData; //迷宫数据工具
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
};

#endif // PLAYWINDOW_H
