#include "student.h"
#include "ui_student.h"
#include "mainwindow.h"
#include "QDebug"
#include "QFile"
#include "QMessageBox"
#include "stu_course.h"
#include "mainwindow.h"

#include <QRegExp>
#include <QDialog>
#include <QTextEdit>
#include <QVBoxLayout>
#include <QPushButton>
#include <QTextStream>
#include <QHBoxLayout>
#include <QLabel>
#include <QComboBox>
#include <QSpinBox>

student::student(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::student)
{
    ui->setupUi(this);

    currentStudentId = "";

    ui->lb_info->setText("未登录学生账号");

    if (readfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "文件读取失败，无法进行查找", "确认");
    }
}

student::student(QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::student)
{
    ui->setupUi(this);

    currentStudentId = id;

    showCurrentStudentInfo();

    if (readfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "文件读取失败，无法进行查找", "确认");
    }
}

void student::showCurrentStudentInfo()
{
    QFile file("student.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        ui->lb_info->setText("学生信息读取失败！");
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

        if (list.size() < 4)
            continue;

        QString id = list.at(0);
        QString name = list.at(1);
        QString className = list.at(2);
        QString major = list.at(3);

        if (id == currentStudentId)
        {
            QString info;
            info += "当前学生信息：\n";
            info += "学号：" + id + "\n";
            info += "姓名：" + name + "\n";
            info += "班级：" + className + "\n";
            info += "专业：" + major;

            ui->lb_info->setText(info);

            file.close();
            return;
        }
    }

    file.close();

    ui->lb_info->setText("未找到当前学生信息！");
}

student::~student()
{
    delete ui;
}

int student::readfile()
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
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith("#"))
            continue;

        score_line.append(line);
    }

    file.close();
    return 0;
}

int student::readCourseCreditFile()
{
    courseCreditMap.clear();

    QFile file("module.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 3)
            continue;

        QString courseName = list.at(1).trimmed();

        bool ok = false;
        double credit = list.at(2).toDouble(&ok);

        if (!ok)
            continue;

        courseCreditMap.insert(courseName, credit);
    }

    file.close();
    return 0;
}

int student::readTeacherFile()
{
    courseTeacherIdMap.clear();
    courseTeacherNameMap.clear();

    QFile file("staff.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty())
            continue;

        if (line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 3)
            continue;

        QString teacherId = list.at(0).trimmed();
        QString teacherName = list.at(1).trimmed();

        for (int i = 2; i < list.size(); ++i)
        {
            QString courseName = list.at(i).trimmed();

            if (!courseName.isEmpty())
            {
                courseTeacherIdMap.insert(courseName, teacherId);
                courseTeacherNameMap.insert(courseName, teacherName);
            }
        }
    }

    file.close();
    return 0;
}

void student::on_btn_stu_query_clicked()
{
    if (currentStudentId.isEmpty())
    {
        QMessageBox::critical(this, "错误", "当前学生账号为空，无法查询成绩！", "确认");
        return;
    }

    if (score_line.isEmpty())
    {
        if (readfile() == -1)
        {
            QMessageBox::critical(this, "错误", "score.txt 读取失败！", "确认");
            return;
        }
    }

    if (readCourseCreditFile() == -1)
    {
        QMessageBox::warning(this,
                             "警告",
                             "module.txt 读取失败，无法计算当前选修总学分！",
                             "确认");
    }

    QString stuIdShow;
    QString nameShow;
    QString courseInfo;

    int count = 0;
    double totalCredit = 0.0;

    for (int i = 0; i < score_line.length(); i++)
    {
        QString line = score_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList linesplit = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (linesplit.size() < 4)
            continue;

        QString stuId = linesplit.at(0).trimmed();
        QString name = linesplit.at(1).trimmed();
        QString courseName = linesplit.at(2).trimmed();
        QString score = linesplit.at(3).trimmed();

        if (stuId == currentStudentId)
        {
            if (count == 0)
            {
                stuIdShow = stuId;
                nameShow = name;
            }

            double credit = courseCreditMap.value(courseName, 0.0);
            totalCredit += credit;

            courseInfo += "课程：" + courseName + "\n";
            courseInfo += "学分：" + QString::number(credit, 'g', 10) + "\n";
            courseInfo += "成绩：" + score + "\n\n";

            count++;
        }
    }

    if (count == 0)
    {
        QMessageBox::information(this, "成绩显示", "未查询到当前学生的成绩记录。", "确认");
        return;
    }

    QString messagebox_out;
    messagebox_out += "学号：" + stuIdShow + "\n";
    messagebox_out += "姓名：" + nameShow + "\n";
    messagebox_out += "当前选修总学分：" + QString::number(totalCredit, 'g', 10) + "\n\n";
    messagebox_out += courseInfo;

    QDialog dialog(this);
    dialog.setWindowTitle("成绩显示");
    dialog.resize(520, 600);

    QVBoxLayout *layout = new QVBoxLayout(&dialog);

    QTextEdit *textEdit = new QTextEdit(&dialog);
    textEdit->setReadOnly(true);
    textEdit->setPlainText(messagebox_out);

    QPushButton *btnClose = new QPushButton("关闭", &dialog);

    layout->addWidget(textEdit);
    layout->addWidget(btnClose);

    connect(btnClose, &QPushButton::clicked, &dialog, &QDialog::accept);

    dialog.exec();
}

void student::on_btn_stu_select_clicked()
{
    if (currentStudentId.isEmpty())
    {
        QMessageBox::critical(this, "错误", "当前学生账号为空，无法进入选课退课界面！", "确认");
        return;
    }

    stu_course a(currentStudentId);
    a.exec();
}

void student::on_pushButton_clicked()
{
    this->close();
}

void student::on_btn_return_clicked()
{
    this->close();
    MainWindow *main_window = new MainWindow;
    main_window->show();
}

void student::on_btn_stu_evaluate_clicked()
{
    if (currentStudentId.isEmpty())
    {
        QMessageBox::critical(this, "错误", "当前学生账号为空，无法进行教师评价！", "确认");
        return;
    }

    if (score_line.isEmpty())
    {
        if (readfile() == -1)
        {
            QMessageBox::critical(this, "错误", "score.txt 读取失败，无法获取已选课程！", "确认");
            return;
        }
    }

    if (readTeacherFile() == -1)
    {
        QMessageBox::critical(this, "错误", "staff.txt 读取失败，无法获取课程对应教师！", "确认");
        return;
    }

    QString studentName;
    MyVector<QString> courseList;

    for (int i = 0; i < score_line.length(); ++i)
    {
        QString line = score_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 4)
            continue;

        QString stuId = list.at(0).trimmed();
        QString name = list.at(1).trimmed();
        QString courseName = list.at(2).trimmed();

        if (stuId == currentStudentId)
        {
            studentName = name;

            bool exists = false;
            for (int j = 0; j < courseList.size(); ++j)
            {
                if (courseList.at(j) == courseName)
                {
                    exists = true;
                    break;
                }
            }

            if (!exists)
            {
                courseList.append(courseName);
            }
        }
    }

    if (courseList.isEmpty())
    {
        QMessageBox::information(this,
                                 "提示",
                                 "当前学生没有已选课程，无法进行教师评价。",
                                 "确认");
        return;
    }

    QDialog dialog(this);
    dialog.setWindowTitle("教师评价");
    dialog.resize(460, 360);

    QVBoxLayout *mainLayout = new QVBoxLayout(&dialog);

    QLabel *labelCourse = new QLabel("请选择要评价的课程：", &dialog);
    QComboBox *comboCourse = new QComboBox(&dialog);

    for (int i = 0; i < courseList.size(); ++i)
    {
        comboCourse->addItem(courseList.at(i));
    }

    QLabel *labelTeacher = new QLabel(&dialog);

    QLabel *labelScore = new QLabel("满意度评分：", &dialog);
    QSpinBox *spinScore = new QSpinBox(&dialog);
    spinScore->setRange(0, 100);
    spinScore->setValue(90);

    QLabel *labelComment = new QLabel("评语：", &dialog);
    QTextEdit *textComment = new QTextEdit(&dialog);
    textComment->setPlaceholderText("请输入对该课程教师的评价");

    QHBoxLayout *buttonLayout = new QHBoxLayout;
    QPushButton *btnSubmit = new QPushButton("提交评价", &dialog);
    QPushButton *btnCancel = new QPushButton("取消", &dialog);

    buttonLayout->addWidget(btnSubmit);
    buttonLayout->addWidget(btnCancel);

    mainLayout->addWidget(labelCourse);
    mainLayout->addWidget(comboCourse);
    mainLayout->addWidget(labelTeacher);
    mainLayout->addWidget(labelScore);
    mainLayout->addWidget(spinScore);
    mainLayout->addWidget(labelComment);
    mainLayout->addWidget(textComment);
    mainLayout->addLayout(buttonLayout);

    auto updateTeacherLabel = [&]() {
        QString courseName = comboCourse->currentText().trimmed();
        QString teacherId = courseTeacherIdMap.value(courseName, "未知");
        QString teacherName = courseTeacherNameMap.value(courseName, "未知");

        labelTeacher->setText("任课教师：" + teacherName + "    工号：" + teacherId);
    };

    updateTeacherLabel();

    connect(comboCourse, &QComboBox::currentTextChanged, [&]() {
        updateTeacherLabel();
    });

    connect(btnCancel, &QPushButton::clicked, &dialog, &QDialog::reject);

    connect(btnSubmit, &QPushButton::clicked, [&]() {
        QString courseName = comboCourse->currentText().trimmed();
        QString teacherId = courseTeacherIdMap.value(courseName, "未知");
        QString teacherName = courseTeacherNameMap.value(courseName, "未知");
        QString comment = textComment->toPlainText().trimmed();
        int satisfactionScore = spinScore->value();

        if (courseName.isEmpty())
        {
            QMessageBox::warning(&dialog, "提示", "请选择课程！", "确认");
            return;
        }

        if (teacherId == "未知" || teacherName == "未知")
        {
            QMessageBox::warning(&dialog,
                                 "提示",
                                 "未找到该课程对应的教师信息，无法提交评价！",
                                 "确认");
            return;
        }

        if (comment.isEmpty())
        {
            QMessageBox::warning(&dialog, "提示", "请输入评语！", "确认");
            return;
        }

        QFile file("evaluation.txt");

        bool isNewFile = !file.exists();

        if (!file.open(QIODevice::Append | QIODevice::Text))
        {
            QMessageBox::critical(&dialog, "错误", "evaluation.txt 打开失败，评价未保存！", "确认");
            return;
        }

        QTextStream out(&file);
        out.setCodec("UTF-8");
        out.setGenerateByteOrderMark(true);

        if (isNewFile)
        {
            out << "#学号\t姓名\t课程名称\t教师工号\t教师姓名\t满意度评分\t评语\n";
        }

        out << currentStudentId << "\t"
            << studentName << "\t"
            << courseName << "\t"
            << teacherId << "\t"
            << teacherName << "\t"
            << satisfactionScore << "\t"
            << comment << "\n";

        file.close();

        QMessageBox::information(&dialog, "提示", "评价提交成功！", "确认");
        dialog.accept();
    });

    dialog.exec();
}
