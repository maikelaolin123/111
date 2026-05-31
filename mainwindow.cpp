#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "system.h"
#include "student.h"
#include "teacher.h"

#include "QFile"
#include "QTextStream"
#include "QMessageBox"
#include <QRegExp>
#include <QStringList>

QString account;
QString password;

MainWindow::MainWindow(QWidget *parent) :
    QMainWindow(parent),
    ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

// 读取 student.txt，保存所有学生信息行
int MainWindow::readstudentfile()
{
    student_line.clear();

    QFile file("student.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        student_line.append(line);
    }

    file.close();
    return 0;
}

// 读取 staff.txt，保存所有教师信息行
int MainWindow::readteacherfile()
{
    teacher_line.clear();

    QFile file("staff.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        teacher_line.append(line);
    }

    file.close();
    return 0;
}

// 读取 controller.txt，保留接口，便于后续扩展管理员账号
int MainWindow::readcontroller()
{
    controller_line.clear();

    QFile file("controller.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);

    while (!in.atEnd())
    {
        QString line = in.readLine();
        controller_line.append(line);
    }

    file.close();
    return 0;
}

void MainWindow::on_btn_denglu_clicked()
{
    account = this->ui->le_zhanghao->text().trimmed();
    password = this->ui->le_mima->text().trimmed();

    // 防止空输入导致后续判断异常
    if (account.length() < 1 || password.length() < 1)
    {
        QMessageBox::critical(this, "错误", "请输入账号和密码！", "确认");
        return;
    }

    // 管理员账号固定，不依赖普通数据文件
    if (account == "controller" && password == "0000")
    {
        class system a;
        this->close();
        a.exec();
        return;
    }

    if (readstudentfile() == -1)
    {
        QMessageBox::critical(this,
                              "错误",
                              "学生信息读取失败，学生账户无法登录！",
                              "确认");
        return;
    }

    if (readteacherfile() == -1)
    {
        QMessageBox::critical(this,
                              "错误",
                              "教师信息读取失败，教师账户无法登录！",
                              "确认");
        return;
    }

    int flag = 100;

    // 学生登录：账号为学号，密码统一为 123456
    for (int i = 0; i < student_line.size(); i++)
    {
        QString line = student_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // student.txt 格式：学号 姓名 班级 专业
        if (linesplit.length() < 4)
            continue;

        QString stuId = linesplit.at(0);

        if (account == stuId && password == "123456")
        {
            flag = 0;
            break;
        }
    }

    // 教师登录：账号为工号，密码统一为 123456
    if (flag == 100)
    {
        for (int j = 0; j < teacher_line.size(); j++)
        {
            QString line = teacher_line.at(j).trimmed();

            if (line.isEmpty() || line.startsWith("#"))
                continue;

            QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            // staff.txt 格式：工号 姓名 授课课程...
            if (linesplit.length() < 2)
                continue;

            QString teacherId = linesplit.at(0);

            if (account == teacherId && password == "123456")
            {
                flag = 1;
                break;
            }
        }
    }

    // 根据登录身份进入对应窗口
    switch (flag)
    {
    case 0:
    {
        student a(account);
        this->close();
        a.exec();
        break;
    }
    case 1:
    {
        teacher a(account);
        this->close();
        a.exec();
        break;
    }
    default:
    {
        QMessageBox::critical(this, "错误", "账号密码错误!\n或账号不存在!", "确认");
        break;
    }
    }
}

void MainWindow::on_btn_tuichu_clicked()
{
    this->close();
}
