#ifndef MAZEGENERATOR_H
#define MAZEGENERATOR_H

#include <QVector>
#include <QPair>
#include <QStack>
#include <QRandomGenerator>

class MazeGenerator {
public:
    // 构造函数
    MazeGenerator(int size);

    // 获取迷宫信息
    QVector<QVector<int>> getMazeMap();

    // 获取起点
    QPair<int, int> getStartPoint();

    // 获取终点
    QPair<int, int> getEndPoint();

    // 生成任务点
    void generateTaskPoints();

    // 生成起点和终点
    void generateStartAndEndPoints(bool isTaskMaze = false);

    // 获取任务点数量
    int getTaskPointCount();
private:
    // 生成迷宫
    QVector<QVector<int>> createMaze();

    // 获取当前节点周围符合条件的邻居集合
    QVector<QPair<int, int>> getValidNeighbors(const QVector<QVector<int>>& mp, int x, int y);

    // 打破当前节点到邻居之间的墙
    void breakWall(QVector<QVector<int>>& mp, int x1, int y1, int x2, int y2);

    // 检测周围是否都是墙
    bool isSurroundingWalls(const QVector<QVector<int>>& mp, int x, int y);

    // 计算曼哈顿距离
    int distance(int x1, int y1, int x2, int y2);

    int size; // 迷宫的边长
    QPair<int, int> startPoint; // 起点
    QPair<int, int> endPoint;   // 终点
    QVector<QVector<int>> mazeMap; // 迷宫地图
    QPair<int, int> findFarthestPoint(const QVector<QVector<int>>& mp, QPair<int, int> start);
    int taskPointCount; // 任务点数量
};

#endif // MAZEGENERATOR_H
