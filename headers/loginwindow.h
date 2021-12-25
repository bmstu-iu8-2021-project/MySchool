#ifndef LOGINWINDOW_H
#define LOGINWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlTableModel>
#include <mutex>
#include <thread>

namespace Ui {
class LoginWindow;
}

class LoginWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit LoginWindow(QWidget *parent = nullptr);
    ~LoginWindow();


private slots:
    void on_OKButton_clicked();

    void OKButtonSetEnabled();

    void on_ShowPasswordCheckBox_stateChanged(int arg1);

signals:
    void LineEditsTextsChanged(const QString& login, const QString& password);


private:
    Ui::LoginWindow *ui;
    QSqlDatabase DB;
    QSqlQuery* Query;
};

#endif // LOGINWINDOW_H
