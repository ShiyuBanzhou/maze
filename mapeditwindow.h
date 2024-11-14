#ifndef MAPEDITWINDOW_H
#define MAPEDITWINDOW_H

#include "mapdata.h"
#include "qvalidator.h"
#include "tiletexture.h"

#include <QWidget>

namespace Ui {
class MapEditWindow;
}

class MapEditWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MapEditWindow(QWidget *parent = nullptr);
    ~MapEditWindow();
    //绘制背景图
    void paintEvent(QPaintEvent * e);

private:
    Ui::MapEditWindow *ui;
    QVector<QVector<int>> mp;
    TileTexture * tiles[31][31]; //图块
    int tempSize = 15; //迷宫宽度（大小），默认15
    void createTileTexture(); // 在窗口加载地图图块
    QVector<QVector<int> > createMaze(); //利用DFS随机生成迷宫地图
    MapData *md;
signals:
    void backBtnClicked();
    void randomMap(MapData *md);
};

// 自定义奇数验证器
class OddIntValidator : public QValidator
{
    Q_OBJECT
public:
    OddIntValidator(int min, int max, QObject *parent = nullptr)
        : QValidator(parent), minValue(min), maxValue(max) {}

    State validate(QString &input, int &pos) const override {
        bool ok;
        int value = input.toInt(&ok);

        // 如果转换失败，直接返回Invalid
        if (!ok) {
            return Invalid;
        }

        // 检查是否在有效范围内且是奇数
        if (value >= minValue && value <= maxValue && value % 2 != 0) {
            return Acceptable;  // 合法输入
        }

        return Invalid;  // 非法输入
    }

private:
    int minValue;
    int maxValue;
};

#endif // MAPEDITWINDOW_H
