#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include <QVector>
#include <QPair>
#include <QStack>
#include <QRandomGenerator>

class MazeGenerator{
public:
    // 构造函数
    MazeGenerator(int size);
    // 获取地图信息
    QVector<QVector<int>> getMazeMap();
    // 获取起点
    QPair<int, int> getStartPoint();
    // 获取终点
    QPair<int, int> getEndPoint();

private:
    // 生成迷宫
    QVector<QVector<int>> createMaze();
    // 获取当前节点周围复合邻居特征的邻居集合
    QVector<QPair<int, int>> getValidNeighbors(const QVector<QVector<int>>& mp, int x, int y);
    // 打破当前节点到邻居之间的墙
    void breakWall(QVector<QVector<int>>& mp, int x1, int y1, int x2, int y2);
    // 检测周围是否都是墙
    bool isSurroundingWalls(const QVector<QVector<int>>& mp, int x, int y);

    int size; // 迷宫的边长
    QPair<int, int> startPoint; // 起点
    QPair<int, int> endPoint;   // 终点
    QVector<QVector<int>> mazeMap;
};

#endif // MAZEGENERATOR_H
