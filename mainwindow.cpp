#include "mainwindow.h"
#include "ui_mainwindow.h"
#include "system.h"
#include "student.h"
#include "teacher.h"
#include "QFile"
#include "QTextStream"
#include "QMessageBox"

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

int MainWindow::readstudentfile()
{
    QFile file("student.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return -1;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
       QString line=in.readLine();
       student_line.append(line);
    }
    file.close();
    return 0;
}

int MainWindow::readteacherfile()
{
    QFile file("staff.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return -1;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
       QString line=in.readLine();
       teacher_line.append(line);
    }
    file.close();
    return 0;
}

int MainWindow::readcontroller()
{
    QFile file("controller.txt");
    if(!file.open(QIODevice::ReadOnly|QIODevice::Text))
    {
        return -1;
    }
    QTextStream in(&file);
    while (!in.atEnd())
    {
       QString line=in.readLine();
       controller_line.append(line);
    }
    file.close();
    return 0;
}

void MainWindow::on_btn_denglu_clicked()
{
    account = this->ui->le_zhanghao->text().trimmed();
    password = this->ui->le_mima->text().trimmed();

    // 1. 先判断空输入，避免 account.at(0) 越界
    if (account.length() < 1 || password.length() < 1)
    {
        QMessageBox::critical(this, "错误", "请输入账号和密码！", "确认");
        return;
    }

    // 2. 管理员账号不依赖 student.txt / staff.txt，先判断
    if (account == "controller" && password == "0000")
    {
        class system a;
        this->close();
        a.exec();
        return;
    }

    // 3. 读取学生文件
    if (readstudentfile() == -1)
    {
        QMessageBox::critical(this, "错误",
                              "学生信息读取失败，学生账户无法登录！",
                              "确认");
        return;
    }

    // 4. 读取教师文件
    if (readteacherfile() == -1)
    {
        QMessageBox::critical(this, "错误",
                              "教师信息读取失败，教师账户无法登录！",
                              "确认");
        return;
    }

    int flag = 100;

    // 5. 学生登录：账号 = 学号，密码统一为 123456
    for (int i = 0; i < student_line.length(); i++)
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

    // 6. 教师登录：账号 = 工号，密码统一为 123456
    if (flag == 100)
    {
        for (int j = 0; j < teacher_line.length(); j++)
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

    // 7. 根据登录结果进入不同窗口
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
