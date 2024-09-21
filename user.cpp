#include "user.h"
#include <QStandardPaths>
#include <QDir>

// 定义静态成员变量
QString User::usersFilePath = "";

// 构造函数，初始化用户名和密码
User::User(const QString &username, const QString &password)
    : username(username), password(password)
{

}

// 静态函数：初始化用户信息文件
bool User::initializeUserFile()
{
    // 获取本地可写目录
    QString writablePath = QStandardPaths::writableLocation(QStandardPaths::AppDataLocation);
    QDir dir;
    if (!dir.exists(writablePath)) {
        dir.mkpath(writablePath); // 创建目录
    }
    QString usersFilePath = writablePath + "/users.txt";
    qDebug() << "Full path to users.txt:" << usersFilePath;

    // 如果文件不存在，从资源文件中复制初始数据
    QFile file(usersFilePath);
    if (!file.exists()) {
        QFile resourceFile(":/users.txt");  // 从资源文件读取
        if (resourceFile.open(QIODevice::ReadOnly | QIODevice::Text)) {
            if (file.open(QIODevice::WriteOnly | QIODevice::Text)) {
                QTextStream in(&resourceFile);
                QTextStream out(&file);
                out << in.readAll();  // 将资源文件的内容复制到本地文件
                file.close();
            } else {
                resourceFile.close();
                QMessageBox::critical(nullptr, "错误", "无法创建本地用户信息文件");
                return false; // 无法创建本地文件
            }
            resourceFile.close();
        } else {
            QMessageBox::critical(nullptr, "错误", "无法从资源文件读取用户信息");
            return false; // 无法从资源文件读取
        }
    }
    User::usersFilePath = usersFilePath;
    return true; // 成功初始化用户文件
}

// 检查用户是否已注册
bool User::isRegistered()
{
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "错误", "用户名和密码不能为空");
        return false;
    }

    QFile file(usersFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "错误", "无法打开用户信息文件");
        return false; // 无法打开文件
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        // 用户信息存储格式为 "username,password"
        QStringList userInfo = line.split(",");
        if (userInfo.size() == 2 && userInfo[0] == username) {
            file.close();
            return true; // 用户已注册
        }
    }

    file.close();
    return false; // 未找到用户
}

// 注册用户
bool User::registerUser()
{
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "错误", "用户名和密码不能为空");
        return false;
    }

    if (isRegistered()) {
        QMessageBox::warning(nullptr, "提示", "用户已存在，请您直接进行登录");
        return true; // 用户已存在，无法注册
    }

    QFile file(usersFilePath);
    if (!file.open(QIODevice::Append | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "错误", "无法打开用户信息文件");
        return false; // 无法打开文件
    }

    QTextStream out(&file);
    out << username << "," << password << "\n"; // 将用户名和密码以 "username,password" 格式保存

    file.close();
    QMessageBox::information(nullptr, "成功", "恭喜您，注册成功！");
    return true; // 注册成功
}

// 设置密码
void User::setPassword(QString &password)
{
    this->password = password;
}

// 获取最大尝试次数
int User::getMaxAttempts()
{
    return this->maxAttempts;
}

// 设置用户名
void User::setUsername(QString &username)
{
    this->username = username;
}

// 登录验证
bool User::login()
{
    if (username.isEmpty() || password.isEmpty()) {
        QMessageBox::warning(nullptr, "错误", "用户名和密码不能为空");
        return false;
    }

    QFile file(usersFilePath);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        QMessageBox::critical(nullptr, "错误", "无法打开用户信息文件");
        return false; // 无法打开文件
    }

    QTextStream in(&file);
    while (!in.atEnd()) {
        QString line = in.readLine();
        QStringList userInfo = line.split(",");
        if (userInfo.size() == 2 && userInfo[0] == username && userInfo[1] == password) {
            file.close();
            loginAttempts = 0; // 登录成功，重置错误次数
            return true; // 登录成功
        }
    }

    file.close();

    // 登录失败时增加错误次数
    loginAttempts++;
    int remainingAttempts = maxAttempts - loginAttempts; // 计算剩余尝试次数

    if (loginAttempts >= 3) {
        QMessageBox::critical(nullptr, "错误", "密码错误次数过多，程序将退出");
        exit(0); // 错误次数超过三次，退出程序
    } else {
        QMessageBox::warning(nullptr, "错误",
                             QString("用户名或密码错误！剩余尝试次数: %1").arg(remainingAttempts));
    }

    return false; // 登录失败
}
