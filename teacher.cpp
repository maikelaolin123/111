#include "teacher.h"
#include "ui_teacher.h"
#include "tea_scoreimport.h"
#include "tea_querystu.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QRegExp>
#include <QStringList>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

teacher::teacher(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::teacher)
{
    ui->setupUi(this);
}

teacher::teacher(QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::teacher)
{
    ui->setupUi(this);

    currentTeacherId = id;

    showCurrentTeacherInfo();
}

teacher::~teacher()
{
    delete ui;
}

void teacher::on_btn_close_clicked()
{
    this->close();
}

void teacher::on_btn_tea_scoreInport_clicked()
{
    tea_scoreimport a;
    a.exec();
}

void teacher::on_btn_tea_querystu_clicked()
{
    tea_querystu a;
    a.exec();
}

void teacher::on_btn_return_clicked()
{
    this->close();
    MainWindow *main_window=new MainWindow;
    main_window->show();
}

void teacher::showCurrentTeacherInfo()
{
    QFile file("staff.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->lb_info->setText("教师信息读取失败！");
        return;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // staff.txt 格式：工号 姓名 授课课程1 授课课程2 ...
        if (list.size() < 2)
            continue;

        QString id = list.at(0);
        QString name = list.at(1);

        if (id == currentTeacherId)
        {
            QStringList courses;

            for (int i = 2; i < list.size(); i++)
            {
                courses << list.at(i);
            }

            QString info;
            info += "当前教师信息：\n";
            info += "工号：" + id + "\n";
            info += "姓名：" + name + "\n";
            info += "授课：" + courses.join("、");

            ui->lb_info->setText(info);

            file.close();
            return;
        }
    }

    file.close();

    ui->lb_info->setText("未找到当前教师信息！");
}

void teacher::on_btn_tea_evaluation_clicked()
{
    if (currentTeacherId.isEmpty())
    {
        QMessageBox::critical(this, "错误", "当前教师账号为空，无法查看评价！", "确认");
        return;
    }

    QFile file("evaluation.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "教师评价", "暂无学生评价记录。", "确认");
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    QString result;
    int count = 0;
    double sum = 0.0;
    QString teacherNameShow;

    result += "当前教师收到的学生评价：\n\n";

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith("#"))
            continue;

        // evaluation.txt 格式：
        // 学号    学生姓名    课程名称    教师工号    教师姓名    满意度评分    评语
        QStringList list = line.split("\t");

        if (list.size() < 7)
            continue;

        QString stuId = list.at(0).trimmed();
        QString stuName = list.at(1).trimmed();
        QString courseName = list.at(2).trimmed();
        QString teacherId = list.at(3).trimmed();
        QString teacherName = list.at(4).trimmed();
        QString scoreText = list.at(5).trimmed();

        QString comment;
        for (int i = 6; i < list.size(); ++i)
        {
            if (i > 6)
                comment += " ";
            comment += list.at(i).trimmed();
        }

        if (teacherId != currentTeacherId)
            continue;

        bool ok = false;
        double score = scoreText.toDouble(&ok);

        if (!ok)
            continue;

        if (teacherNameShow.isEmpty())
            teacherNameShow = teacherName;

        count++;
        sum += score;

        result += "课程：" + courseName + "\n";
        result += "学生：" + stuName + "    学号：" + stuId + "\n";
        result += "满意度评分：" + QString::number(score, 'g', 10) + "\n";
        result += "评语：" + comment + "\n\n";
    }

    file.close();

    if (count == 0)
    {
        QMessageBox::information(this, "教师评价", "当前教师暂无学生评价。", "确认");
        return;
    }

    double average = sum / count;

    QString header;
    header += "教师工号：" + currentTeacherId + "\n";
    header += "教师姓名：" + teacherNameShow + "\n";
    header += "评价人数：" + QString::number(count) + "\n";
    header += "平均满意度：" + QString::number(average, 'f', 2) + "\n\n";

    result = header + result;

    QDialog dialog(this);
    dialog.setWindowTitle("查看学生评价");
    dialog.resize(560, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(result);

    QPushButton *btnClose = new QPushButton("关闭", &dialog);

    layout->addWidget(textEdit);
    layout->addWidget(btnClose);

    connect(btnClose, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
}
