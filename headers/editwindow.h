#ifndef EDITWINDOW_H
#define EDITWINDOW_H

#include <QMainWindow>
#include <QSqlDatabase>
#include <QSqlQuery>
#include <QSqlQueryModel>
#include <mutex>
#include <thread>

namespace Ui {
class EditWindow;
}

class EditWindow : public QMainWindow
{
    Q_OBJECT

public:
    explicit EditWindow(QWidget *parent = nullptr, QSqlQuery* StudQuery = nullptr);
    ~EditWindow();
    void closeEvent(QCloseEvent *event);


private slots:
    void CloseButtonClicked();
    void SaveButtonSetEnabled();
    void on_tableView_clicked(const QModelIndex &index);

    void on_SaveButton_clicked();

    void on_DeleteButton_clicked();

signals:
    void WindowClosed();
    void StudentEdited();
    void StudentDeleted();
private:
    void FillGroupsComboBox(const size_t row);
    void FillLangComboBox(const size_t row);
    Ui::EditWindow *ui;
    QSqlQuery* Query;
    QSqlQueryModel* QueryModel;
    QVector<QString> OldValues;
    std::mutex mtx;
};

#endif // EDITWINDOW_H
