#include "tea_querystu.h"
#include "ui_tea_querystu.h"
#include "QFile"
#include "QMessageBox"
#include "QTextStream"
#include "QDebug"
#include "changedata.h"
#include <QRegExp>
#include <QStandardItem>
#include <cmath>
#include <algorithm>

QString id1;
QString name1;
QString courseName1;
QString score1;

tea_querystu::tea_querystu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::tea_querystu)
{
    ui->setupUi(this);

    this->model=new QStandardItemModel;

    this->ui->tableView->setModel(model);
    reset();

//    int row=this->ui->tableView->currentIndex().row();
//    model->removeRow(row);
//    model->submit();

    if(readfile()==-1)
    {
        this->close();
        QMessageBox::critical(this,"错误","文件读取失败，无法进行查找","确认");
    }
}

tea_querystu::~tea_querystu()
{
    delete ui;
}

void tea_querystu::reset()
{
    this->model->setHorizontalHeaderItem(0, new QStandardItem("学号"));
    this->model->setHorizontalHeaderItem(1, new QStandardItem("姓名"));
    this->model->setHorizontalHeaderItem(2, new QStandardItem("课程名称"));
    this->model->setHorizontalHeaderItem(3, new QStandardItem("成绩"));

    this->ui->tableView->setColumnWidth(0, 120);
    this->ui->tableView->setColumnWidth(1, 80);
    this->ui->tableView->setColumnWidth(2, 220);
    this->ui->tableView->setColumnWidth(3, 80);
}

int tea_querystu::readfile()
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

        QStringList subs = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // score.txt 新格式：学号 姓名 课程名称 成绩
        if (subs.length() < 4)
            continue;

        score_line.append(line);
    }

    file.close();
    return 0;
}

int tea_querystu::readStudentFile()
{
    studentClassMap.clear();

    QFile file("student.txt");

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

        QStringList subs = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        // student.txt 格式：学号 姓名 班级 专业
        if (subs.size() < 3)
            continue;

        QString stuId = subs.at(0).trimmed();
        QString className = subs.at(2).trimmed();

        studentClassMap.insert(stuId, className);
    }

    file.close();
    return 0;
}

void tea_querystu::display(int row, QStringList score_line)
{
    if (score_line.size() < 4)
        return;

    this->model->setItem(row, 0, new QStandardItem(score_line.at(0))); // 学号
    this->model->setItem(row, 1, new QStandardItem(score_line.at(1))); // 姓名
    this->model->setItem(row, 2, new QStandardItem(score_line.at(2))); // 课程名称
    this->model->setItem(row, 3, new QStandardItem(score_line.at(3))); // 成绩
}

static void sortScoreListDesc(QStringList &resultList)
{
    for (int i = 0; i < resultList.size() - 1; ++i)
    {
        for (int j = 0; j < resultList.size() - 1 - i; ++j)
        {
            QStringList list1 = resultList.at(j).split(QRegExp("\\s+"), QString::SkipEmptyParts);
            QStringList list2 = resultList.at(j + 1).split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (list1.size() < 4 || list2.size() < 4)
                continue;

            bool ok1 = false;
            bool ok2 = false;

            double score1 = list1.at(3).toDouble(&ok1);
            double score2 = list2.at(3).toDouble(&ok2);

            if (!ok1 || !ok2)
                continue;

            if (score1 < score2)
            {
                resultList.swapItemsAt(j, j + 1);
            }
        }
    }
}

void tea_querystu::on_btn_doquery_clicked()
{
    this->model->clear();
    reset();

    if (readfile() == -1)
    {
        QMessageBox::critical(this, "错误", "score.txt 读取失败！", "确认");
        return;
    }

    if (readStudentFile() == -1)
    {
        QMessageBox::warning(this, "警告", "student.txt 读取失败，按班级查询无法使用。", "确认");
    }

    QString mode = this->ui->cbb_querymode->currentText().trimmed();
    QString key = this->ui->le_info->text().trimmed();

    if (key.isEmpty())
    {
        QMessageBox::critical(this, "错误", "请输入查询内容！", "确认");
        return;
    }

    QStringList resultList;

    // 新增：按班级查询
    // 输入格式：班级 课程名称
    // 示例：123091 高等数学
    if (mode.contains("班级"))
    {
        QStringList queryList = key.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (queryList.size() < 2)
        {
            QMessageBox::warning(this,
                                 "输入格式错误",
                                 "请输入：班级 课程名称\n例如：123091 高等数学",
                                 "确认");
            return;
        }

        QString queryClassName = queryList.at(0).trimmed();
        QString queryCourseName = queryList.at(1).trimmed();

        for (int i = 0; i < score_line.length(); i++)
        {
            QString line = score_line.at(i).trimmed();

            if (line.isEmpty() || line.startsWith("#"))
                continue;

            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (list.size() < 4)
                continue;

            QString stuId = list.at(0).trimmed();
            QString courseName = list.at(2).trimmed();

            QString stuClassName = studentClassMap.value(stuId);

            if (stuClassName.contains(queryClassName, Qt::CaseSensitive)
                    && courseName.contains(queryCourseName, Qt::CaseSensitive))
            {
                resultList.append(line);
            }
        }
    }
    // 课程成绩分数段查询
    // 输入格式：课程名称 最低分 最高分
    // 示例：高等数学 60 80
    else if (mode.contains("分数段") || mode.contains("成绩段"))
    {
        QStringList queryList = key.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (queryList.size() < 3)
        {
            QMessageBox::warning(this,
                                 "输入格式错误",
                                 "请输入：课程名称 最低分 最高分\n例如：高等数学 60 80",
                                 "确认");
            return;
        }

        QString queryCourseName = queryList.at(0).trimmed();

        bool okMin = false;
        bool okMax = false;

        double minScore = queryList.at(1).toDouble(&okMin);
        double maxScore = queryList.at(2).toDouble(&okMax);

        if (!okMin || !okMax)
        {
            QMessageBox::warning(this,
                                 "输入格式错误",
                                 "最低分和最高分必须是数字！",
                                 "确认");
            return;
        }

        if (minScore > maxScore)
        {
            QMessageBox::warning(this,
                                 "输入错误",
                                 "最低分不能大于最高分！",
                                 "确认");
            return;
        }

        for (int i = 0; i < score_line.length(); i++)
        {
            QString line = score_line.at(i).trimmed();

            if (line.isEmpty() || line.startsWith("#"))
                continue;

            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (list.size() < 4)
                continue;

            QString courseName = list.at(2).trimmed();
            QString scoreText = list.at(3).trimmed();

            bool okScore = false;
            double scoreValue = scoreText.toDouble(&okScore);

            if (!okScore)
                continue;

            if (courseName.contains(queryCourseName, Qt::CaseSensitive)
                    && scoreValue >= minScore
                    && scoreValue <= maxScore)
            {
                resultList.append(line);
            }
        }
    }
    else
    {
        // 原有查询功能：姓名、学号、课程名称、成绩
        for (int i = 0; i < score_line.length(); i++)
        {
            QString line = score_line.at(i).trimmed();

            if (line.isEmpty() || line.startsWith("#"))
                continue;

            QStringList list = line.split(QRegExp("\\s+"), QString::SkipEmptyParts);

            if (list.size() < 4)
                continue;

            QString stuId = list.at(0);
            QString name = list.at(1);
            QString courseName = list.at(2);
            QString score = list.at(3);

            bool matched = false;

            if (mode.contains("姓名"))
            {
                matched = name.contains(key, Qt::CaseSensitive);
            }
            else if (mode.contains("学号"))
            {
                matched = stuId.contains(key, Qt::CaseSensitive);
            }
            else if (mode.contains("课程"))
            {
                matched = courseName.contains(key, Qt::CaseSensitive);
            }
            else
            {
                matched = stuId.contains(key, Qt::CaseSensitive)
                        || name.contains(key, Qt::CaseSensitive)
                        || courseName.contains(key, Qt::CaseSensitive)
                        || score.contains(key, Qt::CaseSensitive);
            }

            if (matched)
            {
                resultList.append(line);
            }
        }
    }

    sortScoreListDesc(resultList);

    for (int i = 0; i < resultList.size(); ++i)
    {
        QStringList list = resultList.at(i).split(QRegExp("\\s+"), QString::SkipEmptyParts);
        display(i, list);
    }

    if (resultList.size() == 0)
    {
        QMessageBox::information(this, "查询结果", "未查询到符合条件的成绩记录。", "确认");
    }
}

void tea_querystu::on_tableView_doubleClicked(const QModelIndex &index)
{
    Q_UNUSED(index);

    int row = this->ui->tableView->currentIndex().row();

    if (row < 0)
        return;
    QString scoreText = model->data(model->index(row, 3)).toString().trimmed();
    bool okScore = false;
    scoreText.toDouble(&okScore);

    if (!okScore)
    {
        QMessageBox::information(this, "提示", "统计分析行不能修改成绩。", "确认");
        return;
    }

    // 当前表格格式：
    // 0 学号
    // 1 姓名
    // 2 课程名称
    // 3 成绩
    id1 = model->data(model->index(row, 0)).toString();
    name1 = model->data(model->index(row, 1)).toString();
    courseName1 = model->data(model->index(row, 2)).toString();
    score1 = model->data(model->index(row, 3)).toString();

    changedata a;
    a.exec();

    // 修改完成后重新读取文件
    readfile();

    // 清空表格并恢复表头
    this->model->clear();
    reset();
}

void tea_querystu::on_btn_statistics_clicked()
{
    int rowCount = model->rowCount();

    if (rowCount == 0)
    {
        QMessageBox::warning(this, "提示", "请先查询出成绩记录，再进行统计分析。", "确认");
        return;
    }

    int count = 0;
    int passCount = 0;
    double sum = 0.0;
    QList<double> scoreValues;

    QString firstCourseName = "";
    QString firstClassName = "";

    for (int i = 0; i < rowCount; ++i)
    {
        QString stuId;
        QString courseName;
        QString scoreText;

        if (model->item(i, 0) != nullptr)
            stuId = model->item(i, 0)->text().trimmed();

        if (model->item(i, 2) != nullptr)
            courseName = model->item(i, 2)->text().trimmed();

        if (model->item(i, 3) != nullptr)
            scoreText = model->item(i, 3)->text().trimmed();

        bool ok = false;
        double score = scoreText.toDouble(&ok);

        // 避免重复点击统计分析时，把前面的统计行也算进去
        if (!ok)
            continue;

        if (firstCourseName.isEmpty())
            firstCourseName = courseName;

        QString className = studentClassMap.value(stuId);

        if (firstClassName.isEmpty() && !className.isEmpty())
            firstClassName = className;

        scoreValues.append(score);
        sum += score;
        count++;

        if (score >= 60)
        {
            passCount++;
        }
    }

    if (count == 0)
    {
        QMessageBox::warning(this, "提示", "当前列表中没有可统计的有效成绩。", "确认");
        return;
    }

    double average = sum / count;

    double varianceSum = 0.0;
    for (int i = 0; i < scoreValues.size(); ++i)
    {
        varianceSum += (scoreValues.at(i) - average) * (scoreValues.at(i) - average);
    }

    double standardDeviation = sqrt(varianceSum / count);
    double passRate = passCount * 100.0 / count;

    QString mode = this->ui->cbb_querymode->currentText().trimmed();
    QString key = this->ui->le_info->text().trimmed();

    QString statisticObject;

    if (mode.contains("班级"))
    {
        QStringList queryList = key.split(QRegExp("\\s+"), QString::SkipEmptyParts);

        if (queryList.size() >= 2)
        {
            statisticObject = "班级：" + queryList.at(0) + "，课程：" + queryList.at(1);
        }
        else
        {
            statisticObject = "班级：" + firstClassName + "，课程：" + firstCourseName;
        }
    }
    else if (mode.contains("课程"))
    {
        statisticObject = "所有选课学生，课程：" + key;
    }
    else
    {
        statisticObject = "当前查询结果，课程：" + firstCourseName;
    }

    // 统计结果插入到表格最前面
    model->insertRow(0);
    model->setItem(0, 0, new QStandardItem("统计对象"));
    model->setItem(0, 1, new QStandardItem(statisticObject));
    model->setItem(0, 2, new QStandardItem(""));
    model->setItem(0, 3, new QStandardItem(""));

    model->insertRow(1);
    model->setItem(1, 0, new QStandardItem("统计人数"));
    model->setItem(1, 1, new QStandardItem(QString::number(count)));
    model->setItem(1, 2, new QStandardItem("及格人数"));
    model->setItem(1, 3, new QStandardItem(QString::number(passCount)));

    model->insertRow(2);
    model->setItem(2, 0, new QStandardItem("平均成绩"));
    model->setItem(2, 1, new QStandardItem(QString::number(average, 'f', 2)));
    model->setItem(2, 2, new QStandardItem("标准差"));
    model->setItem(2, 3, new QStandardItem(QString::number(standardDeviation, 'f', 2)));

    model->insertRow(3);
    model->setItem(3, 0, new QStandardItem("及格率"));
    model->setItem(3, 1, new QStandardItem(QString::number(passRate, 'f', 2) + "%"));
    model->setItem(3, 2, new QStandardItem(""));
    model->setItem(3, 3, new QStandardItem(""));

    model->insertRow(4);
    model->setItem(4, 0, new QStandardItem(""));
    model->setItem(4, 1, new QStandardItem(""));
    model->setItem(4, 2, new QStandardItem(""));
    model->setItem(4, 3, new QStandardItem(""));
}
