#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include "addstudentwindow.h"
#include "addstudentwindow.h"
#include "./headers/addgroupswindow.h"
#include "addlangwindow.h"
#include "editwindow.h"
#include <QThread>


QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:
    void on_AddStudentButton_clicked();
    void on_EditButton_clicked();
    void NewWindowOpenedSlot();
    void on_AddGroupsButton_clicked();

    void ReloadTable();
    void NewWindowClosed();
    void on_AddLangButton_clicked();

signals:
    void NewWindowOpenedSignal();

private:
    Ui::MainWindow *ui;
    QSqlDatabase StudentsDB;
    QSqlQuery* StudentsQuery;
    QSqlQueryModel* StudentsQM;
};
#endif // MAINWINDOW_H
