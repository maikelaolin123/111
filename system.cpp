#include "system.h"
#include "ui_system.h"
#include "addstu.h"
#include "addtea.h"
#include "addcourse.h"
#include "sysdel.h"
#include "mainwindow.h"
#include <QFile>
#include <QTextStream>
#include <QStringList>
#include <QMap>
#include <QDialog>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>
#include <QMessageBox>
#include <algorithm>

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
    MainWindow *main_window=new MainWindow;
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

    QMap<QString, double> scoreSumMap;      // 教师工号 -> 评分总和
    QMap<QString, int> countMap;            // 教师工号 -> 评价人数
    QMap<QString, QString> teacherNameMap;  // 教师工号 -> 教师姓名

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

        scoreSumMap[teacherId] += score;
        countMap[teacherId] += 1;
        teacherNameMap[teacherId] = teacherName;
    }

    file.close();

    if (countMap.isEmpty())
    {
        QMessageBox::information(this, "教师评价", "暂无有效评价记录。", "确认");
        return;
    }

    struct TeacherEvaluationInfo
    {
        QString teacherId;
        QString teacherName;
        int count;
        double average;
    };

    QList<TeacherEvaluationInfo> infoList;

    QMap<QString, int>::iterator it;
    for (it = countMap.begin(); it != countMap.end(); ++it)
    {
        QString teacherId = it.key();
        int count = it.value();
        double average = scoreSumMap.value(teacherId) / count;

        TeacherEvaluationInfo info;
        info.teacherId = teacherId;
        info.teacherName = teacherNameMap.value(teacherId);
        info.count = count;
        info.average = average;

        infoList.append(info);
    }

    std::sort(infoList.begin(), infoList.end(),
              [](const TeacherEvaluationInfo &a, const TeacherEvaluationInfo &b) {
                  return a.average > b.average;
              });

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
