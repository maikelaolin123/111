#include "changedata.h"
#include "ui_changedata.h"
#include "tea_querystu.h"

#include <QString>
#include <QDebug>
#include <QMessageBox>
#include <QFile>
#include <QTextStream>
#include <QRegExp>

// extern 变量来自 tea_querystu 双击表格传入的当前行信息
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

    // 打开窗口时显示当前双击选中的原成绩记录
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

// 读取 score.txt 文件到模板容器
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

        // 保存到模板容器
        score_line.append(line);
    }

    file.close();
    return 0;
}

// 写入信息到 score.txt（追加模式或重写逻辑可结合 #END）
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

    // 学号必须是数字
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

    // 姓名不能是纯数字
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

    // 成绩必须是整数0-100
    bool ok = false;
    int scoreValue = score.toInt(&ok);
    if (!ok || scoreValue < 0 || scoreValue > 100)
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

    // 读取原文件到模板容器
    QFile file("score.txt");
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "score.txt 打开失败，无法修改！", "确认");
        return;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    MyVector<QString> lines;  // 模板容器保存文件每行
    bool found = false;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        if (trimmed.isEmpty() || trimmed.startsWith("#"))
        {
            lines.append(line);
            continue;
        }

        QStringList list = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (list.size() < 4)
        {
            lines.append(line);
            continue;
        }

        // 定位要修改的行
        if (list.at(0) == id1 && list.at(1) == name1 &&
            list.at(2) == courseName1 && list.at(3) == score1)
        {
            QString newLine = id + "\t" + name + "\t" + courseName + "\t" + score;
            lines.append(newLine);
            found = true;
        }
        else
        {
            lines.append(line);
        }
    }

    file.close();

    if (!found)
    {
        QMessageBox::warning(this, "通知", "未找到原成绩记录，修改失败！", "确认");
        return;
    }

    // 写回文件
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

    MyVector<QString> lines;
    bool removed = false;

    while (!in.atEnd())
    {
        QString line = in.readLine();
        QString trimmed = line.trimmed();

        if (trimmed.isEmpty() || trimmed.startsWith("#"))
        {
            lines.append(line);
            continue;
        }

        QStringList list = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);
        if (list.size() < 4)
        {
            lines.append(line);
            continue;
        }

        if (list.at(0) == id1 && list.at(1) == name1 &&
            list.at(2) == courseName1 && list.at(3) == score1)
        {
            removed = true;
            continue;
        }

        lines.append(line);
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

// 取消按钮
void changedata::on_btn_changedata_cancel_clicked()
{
    this->close();
}
