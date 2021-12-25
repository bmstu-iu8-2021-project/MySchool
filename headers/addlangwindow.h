#ifndef ADDLANGWINDOW_H
#define ADDLANGWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <mutex>
#include <thread>

namespace Ui {
class AddLangWindow;
}

class AddLangWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit AddLangWindow(QWidget *parent = nullptr, QSqlQuery* query = nullptr);
    ~AddLangWindow();
    void closeEvent(QCloseEvent *event);


private slots:

    void CloseButtonClicked();

    void on_LangLineEdit_textChanged(const QString &arg1);

    void on_AddButton_clicked();

    void on_DeleteButton_clicked();

signals:
    void WindowClosed();
    void LangDeleted();

private:
    Ui::AddLangWindow *ui;
    QSqlQuery* Query;
    std::mutex mtx;
    QSqlQueryModel* QueryModel;
};

#endif // ADDLANGWINDOW_H
