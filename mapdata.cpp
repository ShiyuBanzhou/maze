#include "mapdata.h"

#include <QFile>
#include <QTextStream>

MapData::MapData(const int &mapSize, const int mazeMap[31][31])
{
    this->mapSize = mapSize;
    // 初始化地图数组
    for(int i = 0; i < mapSize; i++){
        for(int j = 0; j < mapSize; j++){
            this->mazeMap[i][j] = 0;
        }
    }
    // 地图赋值
    for(int i = 0; i < mapSize; i++){
        for(int j = 0; j < mapSize; j++){
            this->mazeMap[i][j] = mazeMap[i][j];
        }
    }
}

// 加载地图
void MapData::loadMap(const QString &mapPath)
{
    QFile data(mapPath);
    if (data.open(QFile::ReadOnly))
    {
        QTextStream in(&data);
        this->mapSize = in.readLine().toInt();
        for(int i = 0; i < mapSize; i++)
        {
            for(int j = 0; j < mapSize; j++)
            {
                mazeMap[i][j] = in.read(1).toInt();
            }
            // 跳过行结尾回车 用于换行
            in.read(1);
        }
    }
}

// 保存地图
void MapData::saveMap(const QString &mapPath)
{
    QFile data(mapPath);
    if(data.open(QFile::WriteOnly | QIODevice::Truncate)){
        /* QIODevice::Truncate
         * 打开的文件若已存在，则截取其文件长度为0
         * 即清空已存在文件内容
        */
        QTextStream out(&data);
        out << this->mapSize << "\n";
        for(int i = 0; i < mapSize; i++){
            for(int j = 0; j < mapSize; j++){
                out << mazeMap[i][j];
            }
            out << "\n";
        }
    }
}
