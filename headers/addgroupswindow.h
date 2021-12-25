#ifndef ADDGROUPSWINDOW_H
#define ADDGROUPSWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <mutex>
#include <thread>

namespace Ui {
class AddGroupsWindow;
}

class AddGroupsWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddGroupsWindow(QWidget *parent = nullptr, QSqlQuery* query = nullptr);
    ~AddGroupsWindow();
    void closeEvent(QCloseEvent *event);



private slots:
    void on_AddButton_clicked();
    void CloseButtonClicked();

    void on_GroupsLineEdit_textChanged(const QString &arg1);


    void on_DeleteButton_clicked();
signals:
    void GroupsDeleted();
    void WindowClosed();

private:
    Ui::AddGroupsWindow *ui;
    QSqlQuery* Query;
    QSqlQueryModel* QueryModel;
    std::mutex mtx;
};

#endif // ADDGROUPSWINDOW_H
