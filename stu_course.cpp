#include "stu_course.h"
#include "ui_stu_course.h"

#include <QFile>
#include <QTextStream>
#include <QMessageBox>
#include <QDebug>
#include <QRegExp>
#include <QStandardItemModel>
#include <QAbstractItemView>
#include <QStringList>

float sum = 0;

// 默认构造函数：保留，避免其他窗口调用时报错
stu_course::stu_course(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stu_course)
{
    ui->setupUi(this);

    currentStudentId = "";
    currentStudentName = "";

    initWindow();
}

// 带学生学号的构造函数：从学生窗口进入选课界面时使用
stu_course::stu_course(QString id, QWidget *parent) :
    QDialog(parent),
    ui(new Ui::stu_course)
{
    ui->setupUi(this);

    currentStudentId = id;
    currentStudentName = getStudentNameById(id);

    initWindow();
}

stu_course::~stu_course()
{
    delete ui;
}

// 初始化窗口、表格模型和初始数据
void stu_course::initWindow()
{
    this->model1 = new QStandardItemModel(this);
    this->ui->tb_course->setModel(model1);

    this->model2 = new QStandardItemModel(this);
    this->ui->tb_select->setModel(model2);

    reset();
    reset1();

    this->ui->tb_course->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tb_course->setEditTriggers(QAbstractItemView::NoEditTriggers);

    this->ui->tb_select->setSelectionBehavior(QAbstractItemView::SelectRows);
    this->ui->tb_select->setEditTriggers(QAbstractItemView::NoEditTriggers);

    if (readcoursefile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "课程文件 module.txt 读取失败！", "确认");
        return;
    }

    if (readselectedfile() == -1)
    {
        this->close();
        QMessageBox::critical(this, "错误", "选课文件 selected.txt 读取失败！", "确认");
        return;
    }

    loadSelectedToTable();

    // 进入界面后显示所有可选课程
    showAllCourses();
}

// 根据学号在 student.txt 中查找当前学生姓名
QString stu_course::getStudentNameById(const QString &id)
{
    QFile file("student.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return "";
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // student.txt 格式：学号 姓名 班级 专业
        if (list.size() >= 2 && list.at(0) == id)
        {
            file.close();
            return list.at(1);
        }
    }

    file.close();
    return "";
}

// 设置可选课程表头
void stu_course::reset()
{
    this->model1->setHorizontalHeaderItem(0, new QStandardItem("课程编号"));
    this->model1->setHorizontalHeaderItem(1, new QStandardItem("课程名称"));
    this->model1->setHorizontalHeaderItem(2, new QStandardItem("学分"));
    this->model1->setHorizontalHeaderItem(3, new QStandardItem("学时"));
    this->model1->setHorizontalHeaderItem(4, new QStandardItem("课程类别"));

    this->ui->tb_course->setColumnWidth(0, 100);
    this->ui->tb_course->setColumnWidth(1, 200);
    this->ui->tb_course->setColumnWidth(2, 60);
    this->ui->tb_course->setColumnWidth(3, 60);
    this->ui->tb_course->setColumnWidth(4, 80);
}

// 设置已选课程表头
void stu_course::reset1()
{
    this->model2->setHorizontalHeaderItem(0, new QStandardItem("课程编号"));
    this->model2->setHorizontalHeaderItem(1, new QStandardItem("课程名称"));
    this->model2->setHorizontalHeaderItem(2, new QStandardItem("学分"));
    this->model2->setHorizontalHeaderItem(3, new QStandardItem("课程类别"));

    this->ui->tb_select->setColumnWidth(0, 100);
    this->ui->tb_select->setColumnWidth(1, 200);
    this->ui->tb_select->setColumnWidth(2, 60);
    this->ui->tb_select->setColumnWidth(3, 80);
}

// 读取 module.txt，并用 MyVector 保存课程行
int stu_course::readcoursefile()
{
    course_line.clear();

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

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // module.txt 格式：课程编号 课程名称 学分 学时 课程类别
        if (list.size() < 5)
            continue;

        course_line.append(line);
    }

    file.close();
    return 0;
}

// 读取 selected.txt，并只保留当前学生的选课记录
int stu_course::readselectedfile()
{
    sum = 0;
    selected_line.clear();

    QFile file("selected.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        return -1;
    }

    QTextStream in(&file);
    in.setCodec("GBK");

    while (!in.atEnd())
    {
        QString line = in.readLine().trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // selected.txt 格式：学号 姓名 课程编号 课程名称 学分 课程类别
        if (list.size() < 6)
            continue;

        if (!currentStudentId.isEmpty() && list.at(0) != currentStudentId)
            continue;

        sum += list.at(4).toFloat();
        selected_line.append(line);
    }

    file.close();
    return 0;
}

// 显示一行可选课程
void stu_course::display(int row, QStringList course_line)
{
    if (course_line.size() < 5)
        return;

    this->model1->setItem(row, 0, new QStandardItem(course_line.at(0)));
    this->model1->setItem(row, 1, new QStandardItem(course_line.at(1)));
    this->model1->setItem(row, 2, new QStandardItem(course_line.at(2)));
    this->model1->setItem(row, 3, new QStandardItem(course_line.at(3)));
    this->model1->setItem(row, 4, new QStandardItem(course_line.at(4)));
}

// 显示一行已选课程
void stu_course::display1(int row, QStringList selected_line)
{
    if (selected_line.size() < 6)
        return;

    this->model2->setItem(row, 0, new QStandardItem(selected_line.at(2)));
    this->model2->setItem(row, 1, new QStandardItem(selected_line.at(3)));
    this->model2->setItem(row, 2, new QStandardItem(selected_line.at(4)));
    this->model2->setItem(row, 3, new QStandardItem(selected_line.at(5)));
}

// 将当前学生已选课程加载到右侧表格
void stu_course::loadSelectedToTable()
{
    this->model2->clear();
    reset1();

    int row = 0;

    for (int i = 0; i < selected_line.size(); i++)
    {
        QString line = selected_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 6)
            continue;

        display1(row++, list);
    }

    this->ui->le_sum->setText(QString::number(sum, 'f', 1));
}

// 写入 selected.txt，并保证新记录插入到 #END 前
void stu_course::writeIn(QString info)
{
    QFile file("selected.txt");
    QStringList lines;

    if (file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QTextStream in(&file);
        in.setCodec("GBK");

        while (!in.atEnd())
        {
            lines << in.readLine();
        }

        file.close();
    }

    // 文件为空时自动补充表头和结束标识
    if (lines.isEmpty())
    {
        lines << "#学号 姓名 课程编号 课程名称 学分 课程类别";
        lines << "#END";
    }

    bool inserted = false;

    for (int i = 0; i < lines.size(); i++)
    {
        if (lines.at(i).trimmed() == "#END")
        {
            lines.insert(i, info);
            inserted = true;
            break;
        }
    }

    if (!inserted)
    {
        lines << info;
        lines << "#END";
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "selected.txt 写入失败，课程没有添加！", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines.at(i) << "\n";
    }

    file.close();
}

// 双击左侧课程表进行选课
void stu_course::on_tb_course_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    int row = this->ui->tb_course->currentIndex().row();

    if (row < 0)
        return;

    QString courseId = model1->data(model1->index(row, 0)).toString();
    QString courseName = model1->data(model1->index(row, 1)).toString();
    QString credit = model1->data(model1->index(row, 2)).toString();
    QString hours = model1->data(model1->index(row, 3)).toString();
    QString type = model1->data(model1->index(row, 4)).toString();

    if (currentStudentId.isEmpty())
    {
        QMessageBox::critical(this, "错误", "当前学生账号为空，无法选课！", "确认");
        return;
    }

    if (currentStudentName.isEmpty())
    {
        currentStudentName = getStudentNameById(currentStudentId);
    }

    if (currentStudentName.isEmpty())
    {
        QMessageBox::critical(this, "错误", "无法从 student.txt 中找到当前学生姓名！", "确认");
        return;
    }

    // 重新读取已选课程，确保重复判断和总学分是最新数据
    readselectedfile();

    for (int i = 0; i < selected_line.size(); i++)
    {
        QString line = selected_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 6)
            continue;

        if (list.at(0) == currentStudentId && list.at(2) == courseId)
        {
            QMessageBox::critical(this, "错误", "不可重复选择课程！", "确认");
            return;
        }
    }

    float newSum = sum + credit.toFloat();

    if (newSum > 50)
    {
        QMessageBox::critical(this, "错误", "当前总学分超过 50，不能继续选课！", "确认");
        return;
    }

    QString messagebox_out =
            "课程编号：" + courseId + "\n" +
            "课程名称：" + courseName + "\n" +
            "学分：" + credit + "\n" +
            "学时：" + hours + "\n" +
            "课程类别：" + type + "\n\n" +
            "确认选择该课程？";

    QString info = currentStudentId + " " +
                   currentStudentName + " " +
                   courseId + " " +
                   courseName + " " +
                   credit + " " +
                   type;

    int ret = QMessageBox::question(this, "请确认", messagebox_out, "选课", "取消");

    if (ret == 0)
    {
        writeIn(info);
        readselectedfile();
        loadSelectedToTable();
        QMessageBox::information(this, "通知", "选课成功！", "确认");
    }
}

// 双击右侧已选课程表进行退课
void stu_course::on_tb_select_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    int row = this->ui->tb_select->currentIndex().row();

    if (row < 0)
        return;

    QString courseId = model2->data(model2->index(row, 0)).toString();
    QString courseName = model2->data(model2->index(row, 1)).toString();

    int ret = QMessageBox::question(this,
                                    "请确认",
                                    "确定退出课程：" + courseName + "？",
                                    "退课",
                                    "取消");

    if (ret != 0)
        return;

    QFile file("selected.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "selected.txt 打开失败！", "确认");
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

        if (trimmed.isEmpty() || trimmed.startsWith("#"))
        {
            lines << line;
            continue;
        }

        QStringList list = trimmed.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // 找到当前学生对应课程后跳过写回，实现退课
        if (list.size() >= 6 &&
            list.at(0) == currentStudentId &&
            list.at(2) == courseId)
        {
            removed = true;
            continue;
        }

        lines << line;
    }

    file.close();

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "selected.txt 写入失败！", "确认");
        return;
    }

    QTextStream out(&file);
    out.setCodec("GBK");

    for (int i = 0; i < lines.size(); i++)
    {
        out << lines.at(i) << "\n";
    }

    file.close();

    if (removed)
    {
        QMessageBox::information(this, "通知", "退课成功！", "确认");
    }
    else
    {
        QMessageBox::warning(this, "通知", "未找到对应选课记录！", "确认");
    }

    readselectedfile();
    loadSelectedToTable();
}

// 显示全部可选课程
void stu_course::showAllCourses()
{
    this->model1->clear();
    reset();

    if (readcoursefile() == -1)
    {
        QMessageBox::critical(this, "错误", "module.txt 读取失败！", "确认");
        return;
    }

    int row = 0;

    for (int i = 0; i < course_line.size(); i++)
    {
        QString line = course_line.at(i).trimmed();

        if (line.isEmpty() || line.startsWith("#"))
            continue;

        QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (list.size() < 5)
            continue;

        display(row++, list);
    }
}
