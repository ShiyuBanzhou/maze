#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <mapdata.h>

#include <QWidget>
#include "mapdata.h"

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
};

#endif // PLAYWINDOW_H
