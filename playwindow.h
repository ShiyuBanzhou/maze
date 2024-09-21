#ifndef PLAYWINDOW_H
#define PLAYWINDOW_H

#include <QWidget>

namespace Ui {
class PlayWindow;
}

class PlayWindow : public QWidget
{
    Q_OBJECT

public:
    explicit PlayWindow(QWidget *parent = nullptr);
    ~PlayWindow();

protected:
    void paintEvent(QPaintEvent *event) override; // 重写paintEvent

private:
    Ui::PlayWindow *ui;
    QVector<QVector<int>> mazeMap; // 用于保存迷宫地图
    QPair<int, int> startPoint; // 用于保存迷宫起点
    QPair<int, int> endPoint; // 用于保存迷宫终点
};

#endif // PLAYWINDOW_H
