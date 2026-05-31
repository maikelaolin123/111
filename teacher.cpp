#include "teacher.h"
#include "ui_teacher.h"
#include "tea_scoreimport.h"
#include "tea_querystu.h"
#include "mainwindow.h"
#include "myvector.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QVBoxLayout>
#include <QTextEdit>
#include <QPushButton>

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
    MainWindow *main_window = new MainWindow;
    main_window->show();
}

// 显示教师基本信息及授课列表
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

    courseList.clear();  // 使用模板容器保存授课课程

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
            // 将授课课程保存到模板容器
            for (int i = 2; i < list.size(); i++)
            {
                if (!list.at(i).isEmpty())
                    courseList.append(list.at(i));
            }

            QString info;
            info += "当前教师信息：\n";
            info += "工号：" + id + "\n";
            info += "姓名：" + name + "\n";
            info += "授课：";

            for (int i = 0; i < courseList.size(); ++i)
            {
                if (i > 0) info += "、";
                info += courseList.at(i);
            }

            ui->lb_info->setText(info);
            file.close();
            return;
        }
    }

    file.close();
    ui->lb_info->setText("未找到当前教师信息！");
}

// 查看教师收到的学生评价
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

    // 使用模板容器存储评价信息
    struct TeacherEvaluationInfo
    {
        QString courseName;
        QString stuId;
        QString stuName;
        double score;
        QString comment;
    };
    MyVector<TeacherEvaluationInfo> evaluationList;

    QString teacherNameShow;
    double sum = 0.0;

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();
        if (line.isEmpty() || line.startsWith("#"))
            continue;

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
            if (i > 6) comment += " ";
            comment += list.at(i).trimmed();
        }

        if (teacherId != currentTeacherId)
            continue;

        bool ok = false;
        double score = scoreText.toDouble(&ok);
        if (!ok) continue;

        TeacherEvaluationInfo info;
        info.courseName = courseName;
        info.stuId = stuId;
        info.stuName = stuName;
        info.score = score;
        info.comment = comment;

        evaluationList.append(info);

        sum += score;
        if (teacherNameShow.isEmpty())
            teacherNameShow = teacherName;
    }

    file.close();

    if (evaluationList.isEmpty())
    {
        QMessageBox::information(this, "教师评价", "当前教师暂无学生评价。", "确认");
        return;
    }

    // 计算平均满意度
    double average = sum / evaluationList.size();

    QString result;
    result += "教师工号：" + currentTeacherId + "\n";
    result += "教师姓名：" + teacherNameShow + "\n";
    result += "评价人数：" + QString::number(evaluationList.size()) + "\n";
    result += "平均满意度：" + QString::number(average, 'f', 2) + "\n\n";

    // 拼接每条评价显示
    for (int i = 0; i < evaluationList.size(); ++i)
    {
        result += "课程：" + evaluationList.at(i).courseName + "\n";
        result += "学生：" + evaluationList.at(i).stuName + "    学号：" + evaluationList.at(i).stuId + "\n";
        result += "满意度评分：" + QString::number(evaluationList.at(i).score, 'g', 10) + "\n";
        result += "评语：" + evaluationList.at(i).comment + "\n\n";
    }

    // 创建显示窗口
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
