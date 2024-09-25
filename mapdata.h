#ifndef MAPDATA_H
#define MAPDATA_H

#include <QString>

class MapData{
public:
    // 构造函数
    MapData();
    MapData(const int &mapSize, const int mazeMap[31][31]);
    void loadMap(const QString &mapPath); // 加载地图
    void saveMap(const QString &mapPath); // 保存地图
    int getMapSize(); // 获取地图尺寸
private:
    static const int maxSize = 31;
    /*
     * 地图容纳最大空间
     * 0:墙
     * 1:普通道路
     * 2:起点
     * 3:终点
    */
    int mazeMap[maxSize][maxSize];
    int mapSize; // 地图大小
    QString *mapPath; // 地图路径
};

#endif // MAPDATA_H
