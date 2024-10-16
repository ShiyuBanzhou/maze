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

protected:
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent

private:
    Ui::PlayWindow *ui;
    MapData *mapData; //迷宫数据工具
    TileTexture * tiles[31][31]; //地图图块
    void keyPressEvent(QKeyEvent *) override;    //监听键盘事件，实现移动
    int posX, posY, startX, startY, endX, endY; //当前位置、起点和终点的坐标
};

#endif // PLAYWINDOW_H
