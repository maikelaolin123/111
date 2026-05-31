#include "system.h"
#include "ui_system.h"
#include "addstu.h"
#include "addtea.h"
#include "addcourse.h"
#include "sysdel.h"
#include "mainwindow.h"
#include "myvector.h"

#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>

system::system(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::system)
{
    ui->setupUi(this);
}

system::~system()
{
    delete ui;
}

void system::on_btn_addstu_clicked()
{
    addstu a;
    a.exec();
}

void system::on_btn_addtea_clicked()
{
    addtea a;
    a.exec();
}

void system::on_btn_addcourse_clicked()
{
    addcourse a;
    a.exec();
}

void system::on_btn_del_clicked()
{
    sysdel a;
    a.exec();
}

void system::on_btn_cancel_clicked()
{
    this->close();
}

void system::on_btn_return_clicked()
{
    this->close();
    MainWindow *main_window = new MainWindow;
    main_window->show();
}

void system::on_btn_teacher_evaluation_clicked()
{
    QFile file("evaluation.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::information(this, "教师评价", "暂无学生评价记录。", "确认");
        return;
    }

    QTextStream in(&file);
    in.setCodec("UTF-8");

    struct TeacherEvaluationInfo
    {
        QString teacherId;
        QString teacherName;
        int count;
        double totalScore;
        double average;
    };

    MyVector<TeacherEvaluationInfo> infoList;

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

        QString teacherId = list.at(3).trimmed();
        QString teacherName = list.at(4).trimmed();
        QString scoreText = list.at(5).trimmed();

        bool ok = false;
        double score = scoreText.toDouble(&ok);

        if (!ok)
            continue;

        bool found = false;

        for (int i = 0; i < infoList.size(); ++i)
        {
            if (infoList[i].teacherId == teacherId)
            {
                infoList[i].totalScore += score;
                infoList[i].count += 1;
                infoList[i].average = infoList[i].totalScore / infoList[i].count;
                found = true;
                break;
            }
        }

        if (!found)
        {
            TeacherEvaluationInfo info;
            info.teacherId = teacherId;
            info.teacherName = teacherName;
            info.count = 1;
            info.totalScore = score;
            info.average = score;

            infoList.append(info);
        }
    }

    file.close();

    if (infoList.size() == 0)
    {
        QMessageBox::information(this, "教师评价", "暂无有效评价记录。", "确认");
        return;
    }

    // 按平均满意度从高到低排序
    for (int i = 0; i < infoList.size() - 1; ++i)
    {
        for (int j = 0; j < infoList.size() - 1 - i; ++j)
        {
            if (infoList[j].average < infoList[j + 1].average)
            {
                TeacherEvaluationInfo temp = infoList[j];
                infoList[j] = infoList[j + 1];
                infoList[j + 1] = temp;
            }
        }
    }

    QString result;
    result += "教师教学效果满意度排序：\n\n";

    for (int i = 0; i < infoList.size(); ++i)
    {
        result += "第" + QString::number(i + 1) + "名\n";
        result += "教师工号：" + infoList.at(i).teacherId + "\n";
        result += "教师姓名：" + infoList.at(i).teacherName + "\n";
        result += "评价人数：" + QString::number(infoList.at(i).count) + "\n";
        result += "平均满意度：" + QString::number(infoList.at(i).average, 'f', 2) + "\n\n";
    }

    QDialog dialog(this);
    dialog.setWindowTitle("教师评价排序");
    dialog.resize(520, 560);

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
