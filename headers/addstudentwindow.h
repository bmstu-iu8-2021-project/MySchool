#ifndef ADDSTUDENTWINDOW_H
#define ADDSTUDENTWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <./headers/mainwindow.h>
#include <QCloseEvent>
#include <thread>
#include <mutex>

namespace Ui {
class AddStudentWindow;
}


class AddStudentWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddStudentWindow(QWidget *parent = nullptr, QSqlQuery* StudQuer = nullptr);
    ~AddStudentWindow();

    void FillGroupsComboBox();
    void FillLangComboBox();
    void closeEvent(QCloseEvent *event);

private:
    Ui::AddStudentWindow *ui;
    QSqlQuery* Query;
    QSqlQueryModel* QueryModel;
    std::mutex mtx;

private slots:
    void AddButtonSetEnabled();
    void AddButtonClicked();
    void CancelButtonClicked();
signals:
    void StudentAdded();
    void WindowClosed();
    void WorkStarted();
    void WorkEnded();
};


#endif // ADDSTUDENTWINDOW_H
