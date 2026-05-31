#include "addstu.h"
#include "ui_addstu.h"
#include "myvector.h"

#include "QString"
#include "QDebug"
#include "QMessageBox"
#include "QFile"
#include "QTextStream"

addstu::addstu(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addstu)
{
    ui->setupUi(this);
}

addstu::~addstu()
{
    delete ui;
}

void addstu::on_btn_sys_addstu_clicked()
{
    QString id = this->ui->le_stu_id->text().trimmed();
    QString name = this->ui->le_stu_name->text().trimmed();
    QString classnum = this->ui->le_stu_class->text().trimmed();
    QString major = this->ui->le_stu_major->text().trimmed();

    QString messagebox_out = "学号：" + id + "\n" +
                             "姓名：" + name + "\n" +
                             "班级：" + classnum + "\n" +
                             "专业：" + major + "\n" +
                             "默认密码：123456";

    // student.txt 格式：学号 姓名 班级 专业
    QString info = id + " " + name + " " + classnum + " " + major;

    // 检查输入完整性和学号位数
    bool charge = id.length() != 11 ||
                  name.length() < 1 ||
                  classnum.length() < 1 ||
                  major.length() < 1;

    if(charge)
    {
        QMessageBox::critical(this, "错误", "信息填写不完整，请检查", "确定");
    }
    else
    {
        int ret = QMessageBox::question(this, "请确认", messagebox_out, "确认", "取消");
        if(ret == 0)
        {
            writeIn(info);
        }
    }
}

void addstu::writeIn(QString info)
{
    this->ui->le_stu_id->clear();
    this->ui->le_stu_name->clear();
    this->ui->le_stu_class->clear();
    this->ui->le_stu_major->clear();
    this->ui->le_stu_id->setFocus();

    QFile file("student.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "学生文件打开失败，信息没有写入", "确认");
        return;
    }

    // 使用自定义模板容器保存文件所有行
    MyVector<QString> lines;

    QTextStream in(&file);
    while (!in.atEnd())
    {
        lines.append(in.readLine());
    }
    file.close();

    int endIndex = -1;

    // 查找 #END，将新增学生插入到 #END 前
    for (int i = 0; i < lines.size(); ++i)
    {
        if (lines.at(i).trimmed() == "#END")
        {
            endIndex = i;
            break;
        }
    }

    if (endIndex == -1)
    {
        lines.append(info);
        lines.append("#END");
    }
    else
    {
        lines.insert(endIndex, info);
    }

    if (!file.open(QIODevice::WriteOnly | QIODevice::Text | QIODevice::Truncate))
    {
        QMessageBox::critical(this, "错误", "学生文件写入失败，信息没有写入", "确认");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < lines.size(); ++i)
    {
        out << lines.at(i) << "\n";
    }

    file.close();

    QMessageBox::information(this, "成功", "学生信息添加成功！\n默认登录密码为：123456", "确认");
}

void addstu::on_btn_sys_addstu_cancel_clicked()
{
    this->close();
}
