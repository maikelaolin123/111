#include "addtea.h"
#include "ui_addtea.h"
#include "myvector.h"

#include "QMessageBox"
#include "QFile"
#include "QTextStream"

addtea::addtea(QWidget *parent) :
    QDialog(parent),
    ui(new Ui::addtea)
{
    ui->setupUi(this);
}

addtea::~addtea()
{
    delete ui;
}

void addtea::on_btn_sys_addtea_clicked()
{
    QString id = this->ui->le_tea_id->text().trimmed();
    QString name = this->ui->le_tea_name->text().trimmed();
    QString course = this->ui->le_tea_course->text().trimmed();

    QString messagebox_out = "工号：" + id + "\n" +
                             "姓名：" + name + "\n" +
                             "授课：" + course + "\n" +
                             "默认密码：123456";

    // staff.txt 格式：工号 姓名 授课课程
    QString info = id + " " + name + " " + course;

    // 检查输入完整性和工号位数
    bool charge = id.length() != 6 ||
                  name.length() < 1 ||
                  course.length() < 1;

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

void addtea::writeIn(QString info)
{
    this->ui->le_tea_id->clear();
    this->ui->le_tea_name->clear();
    this->ui->le_tea_course->clear();
    this->ui->le_tea_id->setFocus();

    QFile file("staff.txt");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        QMessageBox::critical(this, "错误", "教师文件打开失败，信息没有写入", "确认");
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

    // 查找 #END，将新增教师插入到 #END 前
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
        QMessageBox::critical(this, "错误", "教师文件写入失败，信息没有写入", "确认");
        return;
    }

    QTextStream out(&file);

    for (int i = 0; i < lines.size(); ++i)
    {
        out << lines.at(i) << "\n";
    }

    file.close();

    QMessageBox::information(this, "成功", "教师信息添加成功！\n默认登录密码为：123456", "确认");
}

void addtea::on_btn_sys_addteacancel_clicked()
{
    this->close();
}
