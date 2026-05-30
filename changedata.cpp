#include "changedata.h"
#include "ui_changedata.h"
#include "tea_querystu.h"

#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QRegExp>

// 来自 tea_querystu.cpp
extern QString id1;
extern QString name1;
extern QString courseName1;
extern QString score1;

changedata::changedata(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::changedata)
{
    ui->setupUi(this);

    ui->pushButton->setText("显示原信息");

    // 打开窗口时自动显示当前双击选中的原成绩记录
    ui->le_changedata_id->setText(id1);
    ui->le_changedata_name->setText(name1);
    ui->le_changedata_course->setText(courseName1);
    ui->le_changedata_score->setText(score1);

    if (readfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "文件读取失败!", "确认");
    }
}

changedata::~changedata()
{
    delete ui;
}

int changedata::readfile()
{
    score_line.clear();

    QFile file("score.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine();

        if (line.trimmed().isEmpty())
            continue;

        score_line.append(line);
    }

    file.close();
    return 0;
}

// 这里保留函数名，防止 changedata.h 里已有声明。
// 但修改逻辑已经不再使用追加写入，而是整体重写 score.txt。
void changedata::writeIn(QString information)
{
    QFile file("score.txt");

    if (!file.open(QIODevice::Append | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "文件打开失败，信息没有写入", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");
    out << information << "\n";

    file.close();
}

// “显示原信息”按钮
void changedata::on_pushButton_clicked()
{
    this->ui->le_changedata_id->setText(id1);
    this->ui->le_changedata_name->setText(name1);
    this->ui->le_changedata_course->setText(courseName1);
    this->ui->le_changedata_score->setText(score1);
}

// 修改成绩
void changedata::on_btn_changedata_change_clicked()
{
    QString id = this->ui->le_changedata_id->text().trimmed();
    QString name = this->ui->le_changedata_name->text().trimmed();
    QString courseName = this->ui->le_changedata_course->text().trimmed();
    QString score = this->ui->le_changedata_score->text().trimmed();

    if (id.isEmpty() || name.isEmpty() || courseName.isEmpty() || score.isEmpty())
    {
        QMessageBox::critical(this, "错误", "信息填写不完整，无法修改！", "确定");
        return;
    }

    // 学号应为数字，防止把姓名误填到学号框
    bool okId = false;
    id.toLongLong(&okId);

    if (!okId)
    {
        QMessageBox::critical(this,
                              "错误",
                              "学号必须是数字！请检查是否把姓名误填到了学号框。",
                              "确定");
        return;
    }

    // 姓名不应是纯数字，防止把学号误填到姓名框
    bool nameIsNumber = false;
    name.toLongLong(&nameIsNumber);

    if (nameIsNumber)
    {
        QMessageBox::critical(this,
                              "错误",
                              "姓名不能是纯数字！请检查是否把学号误填到了姓名框。",
                              "确定");
        return;
    }

    // 成绩必须是数字，并限制在 0 到 100
    bool ok = false;
    int scoreValue = score.toInt(&ok);

    if (!ok)
    {
        QMessageBox::critical(this, "错误", "成绩必须是数字！", "确定");
        return;
    }

    if (scoreValue < 0 || scoreValue > 100)
    {
        QMessageBox::critical(this, "错误", "成绩必须在 0 到 100 之间！", "确定");
        return;
    }

    QString messagebox_out =
            "学号：" + id + "\n" +
            "姓名：" + name + "\n" +
            "课程名称：" + courseName + "\n" +
            "成绩：" + score + "\n";

    int ret = QMessageBox::question(this, "请确认", messagebox_out, "确认", "取消");

    if (ret != 0)
        return;

    QFile file("score.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "score.txt 打开失败，无法修改！", "确认");
        return;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    QStringList lines;
    bool found = false;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        if (trimmed.isEmpty())
        {
            lines << line;
            continue;
        }

        // 表头和 #END 原样保留
        if (trimmed.startsWith("#"))
        {
            lines << line;
            continue;
        }

        QStringList list = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // score.txt 格式：学号 姓名 课程名称 成绩
        if (list.size() < 4)
        {
            lines << line;
            continue;
        }

        QString oldId = list.at(0);
        QString oldName = list.at(1);
        QString oldCourseName = list.at(2);
        QString oldScore = list.at(3);

        // 用双击表格时保存的原记录定位要修改的那一行
        if (oldId == id1 &&
            oldName == name1 &&
            oldCourseName == courseName1 &&
            oldScore == score1)
        {
            QString newLine = id + "\t" + name + "\t" + courseName + "\t" + score;
            lines << newLine;
            found = true;
        }
        else
        {
            lines << line;
        }
    }

    file.close();

    if (!found)
    {
        QMessageBox::warning(this, "通知", "未找到原成绩记录，修改失败！", "确认");
        return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "score.txt 写入失败，无法修改！", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines.at(i) << "\n";
    }

    file.close();

    QMessageBox::information(this, "通知", "修改成功！", "确认");
    this->close();
}

// 删除成绩记录
void changedata::on_btn_changedata_del_clicked()
{
    int ret = QMessageBox::question(this,
                                    "请确认",
                                    "确定删除当前成绩记录？",
                                    "删除",
                                    "取消");

    if (ret != 0)
        return;

    QFile file("score.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "score.txt 打开失败，无法删除！", "确认");
        return;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    QStringList lines;
    bool removed = false;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        if (trimmed.isEmpty())
        {
            lines << line;
            continue;
        }

        // 表头和 #END 原样保留
        if (trimmed.startsWith("#"))
        {
            lines << line;
            continue;
        }

        QStringList list = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // score.txt 格式：学号 姓名 课程名称 成绩
        if (list.size() < 4)
        {
            lines << line;
            continue;
        }

        QString oldId = list.at(0);
        QString oldName = list.at(1);
        QString oldCourseName = list.at(2);
        QString oldScore = list.at(3);

        // 只删除当前双击选中的这一条成绩记录
        if (oldId == id1 &&
            oldName == name1 &&
            oldCourseName == courseName1 &&
            oldScore == score1)
        {
            removed = true;
            continue;
        }

        lines << line;
    }

    file.close();

    if (!removed)
    {
        QMessageBox::warning(this, "通知", "未找到原成绩记录，删除失败！", "确认");
        return;
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "score.txt 写入失败，无法删除！", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines.at(i) << "\n";
    }

    file.close();

    QMessageBox::information(this, "通知", "删除成功！", "确认");
    this->close();
}

void changedata::on_btn_changedata_cancel_clicked()
{
    this->close();
}
