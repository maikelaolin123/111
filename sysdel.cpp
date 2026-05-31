#include "sysdel.h"
#include "ui_sysdel.h"
#include "QMessageBox"
#include "QDebug"
#include "QFile"
#include "QTextStream"

#include <QRegExp>

QString ID;

sysdel::sysdel(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::sysdel)
{
    ui->setupUi(this);

    if(readstudentfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "学生文件读取失败!", "确认");
    }

    if(readteacherfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "教师文件读取失败!", "确认");
    }

    if(readcoursefile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "课程文件读取失败!", "确认");
    }
}

// 读取 student.txt，并保存到模板容器
int sysdel::readstudentfile()
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

// 读取 staff.txt，并保存到模板容器
int sysdel::readteacherfile()
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

// 读取 module.txt，并保存到模板容器
int sysdel::readcoursefile()
{
    course_line.clear();

    QFile file("module.txt");
    if(!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    while (!in.atEnd())
    {
        QString line = in.readLine();
        course_line.append(line);
    }

    file.close();
    return 0;
}

// 删除指定学号的学生记录
void sysdel::deletestudent()
{
    bool found = false;

    QFile file("student_temp.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "临时文件打开失败，学生账号未删除", "确认");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < student_line.size(); i++)
    {
        QString line = student_line.at(i).trimmed();

        if(line.isEmpty())
        {
            continue;
        }

        if(line == "#END")
        {
            out << line << "\n";
            continue;
        }

        QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // student.txt 格式：学号 姓名 班级 专业
        if(linesplit.size() > 0 && ID == linesplit.at(0))
        {
            found = true;
            continue;
        }

        out << line << "\n";
    }

    file.close();

    if(!found)
    {
        QFile::remove("student_temp.txt");
        QMessageBox::warning(this, "提示", "未找到该学生账号，删除失败！", "确认");
        return;
    }

    QFile file_old("student.txt");
    QFile file_new("student_temp.txt");

    if(file_old.exists())
    {
        file_old.remove();
        file_new.rename("student.txt");
    }
    else
    {
        QMessageBox::critical(this, "错误", "未找到 student.txt，无法删除", "确认");
        return;
    }

    QMessageBox::information(this, "通知", "学生删除成功！", "确认");
}

// 删除指定工号的教师记录
void sysdel::deleteteacher()
{
    bool found = false;

    QFile file("teacher_temp.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "临时文件打开失败，教师账号未删除", "确认");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < teacher_line.size(); i++)
    {
        QString line = teacher_line.at(i).trimmed();

        if(line.isEmpty())
        {
            continue;
        }

        if(line == "#END")
        {
            out << line << "\n";
            continue;
        }

        QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // staff.txt 格式：工号 姓名 授课课程...
        if(linesplit.size() > 0 && ID == linesplit.at(0))
        {
            found = true;
            continue;
        }

        out << line << "\n";
    }

    file.close();

    if(!found)
    {
        QFile::remove("teacher_temp.txt");
        QMessageBox::warning(this, "提示", "未找到该教师账号，删除失败！", "确认");
        return;
    }

    QFile file_old("staff.txt");
    QFile file_new("teacher_temp.txt");

    if(file_old.exists())
    {
        file_old.remove();
        file_new.rename("staff.txt");
    }
    else
    {
        QMessageBox::critical(this, "错误", "未找到 staff.txt，无法删除", "确认");
        return;
    }

    QMessageBox::information(this, "通知", "教师删除成功！", "确认");
}

// 删除指定课程编号的课程记录
void sysdel::deletecourse()
{
    bool found = false;

    QFile file("course_temp.txt");
    if(!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "临时文件打开失败，课程信息未删除", "确认");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < course_line.size(); i++)
    {
        QString line = course_line.at(i).trimmed();

        if(line.isEmpty())
        {
            continue;
        }

        if(line == "#END")
        {
            out << line << "\n";
            continue;
        }

        QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // module.txt 格式：课程编号 课程名称 学分 学时 课程类别
        if(linesplit.size() > 0 && ID == linesplit.at(0))
        {
            found = true;
            continue;
        }

        out << line << "\n";
    }

    file.close();

    if(!found)
    {
        QFile::remove("course_temp.txt");
        QMessageBox::warning(this, "提示", "未找到该课程编号，删除失败！", "确认");
        return;
    }

    QFile file_old("module.txt");
    QFile file_new("course_temp.txt");

    if(file_old.exists())
    {
        file_old.remove();
        file_new.rename("module.txt");
    }
    else
    {
        QMessageBox::critical(this, "错误", "未找到 module.txt，无法删除", "确认");
        return;
    }

    QMessageBox::information(this, "通知", "课程删除成功！", "确认");
}

// 根据 ID 位数判断删除对象类型
void sysdel::on_btn_del_clicked()
{
    ID = this->ui->le_id->text().trimmed();

    if(ID.isEmpty())
    {
        QMessageBox::critical(this, "错误", "请输入要删除的ID！", "确定");
        return;
    }

    int flag = 100;

    if(ID.length() == 11)
    {
        flag = 0;   // 学生学号
    }
    else if(ID.length() == 6)
    {
        flag = 1;   // 教师工号
    }
    else if(ID.length() == 8)
    {
        flag = 2;   // 课程编号
    }
    else
    {
        QMessageBox::critical(this,
                              "错误",
                              "ID位数不正确，请检查！\n学生学号为11位，教师工号为6位，课程编号为8位。",
                              "确定");
        return;
    }

    switch (flag)
    {
    case 0:
        deletestudent();
        break;
    case 1:
        deleteteacher();
        break;
    case 2:
        deletecourse();
        break;
    default:
        break;
    }
}

void sysdel::on_btn_cancel_clicked()
{
    this->close();
}
