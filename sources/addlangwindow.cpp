#include "./headers/addlangwindow.h"
#include "ui_addlangwindow.h"
#include <QRegExpValidator>
#include <QMessageBox>
#include <QCloseEvent>
#include <QDebug>

AddLangWindow::AddLangWindow(QWidget *parent, QSqlQuery* query) :
    QMainWindow(parent),
    ui(new Ui::AddLangWindow)
{
    ui->setupUi(this);
    Query = query;
    ui->AddButton->setEnabled(false);

    QRegExpValidator* validator = new QRegExpValidator(QRegExp("[a-zA-Z]{1,20}"), this);
    ui->LangLineEdit->setValidator(validator);

    QueryModel = new QSqlQueryModel();
    Query->exec("select * from Languages");
    QueryModel->setQuery(*Query);
    ui->tableView->setModel(QueryModel);
    ui->tableView->setEditTriggers(QAbstractItemView::NoEditTriggers);
    ui->tableView->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);

}

AddLangWindow::~AddLangWindow() {
    delete ui;
}

void AddLangWindow::on_LangLineEdit_textChanged(const QString &arg1) {
    ui->AddButton->setEnabled(!arg1.isEmpty());
}


void AddLangWindow::on_AddButton_clicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->exec("select * from Languages where Language='"+ui->LangLineEdit->text()+"'");
        size_t counter = 0;
        while (Query->next()) {
            ++counter;
        }
        if (counter > 0) {
            QMessageBox::warning(this, "Not added", "There is already such a language");
        } else {
            Query->exec("insert into Languages (Language) values ('"+ui->LangLineEdit->text()+"')");
            Query->exec("select * from Languages");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
            ui->LangLineEdit->clear();
        }
    });
    th.detach();
}


void AddLangWindow::on_DeleteButton_clicked() {
    std::thread th([&](){
        std::lock_guard<std::mutex> lk(mtx);
        Query->exec("select * from Students where Language='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
        size_t counter = 0;
        while (Query->next()) {
            ++counter;
        }
        if (counter > 0) {
            QMessageBox MSG;
            MSG.setText("There are students with this language");
            MSG.setInformativeText("Their language will be set to 'deleted'");
            MSG.setStandardButtons(QMessageBox::Cancel | QMessageBox::Ok);
            int ret = MSG.exec();
            if (ret == 1024) {
                Query->exec("update Students set Language='deleted' where Language='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
                Query->exec("delete from Languages where Language='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
                Query->exec("select * from Languages");
                QueryModel->setQuery(*Query);
                ui->tableView->setModel(QueryModel);
                emit LangDeleted();

            }
        } else {
            Query->exec("delete from Languages where Language='"+QueryModel->data(ui->tableView->currentIndex(), Qt::DisplayRole).toString()+"'");
            Query->exec("select * from Languages");
            QueryModel->setQuery(*Query);
            ui->tableView->setModel(QueryModel);
        }
        ui->LangLineEdit->clear();
    });
    th.detach();
}

void AddLangWindow::closeEvent(QCloseEvent *event) {
    QMessageBox::StandardButton resBtn = QMessageBox::question( this, "Exit from adding languages",
                                                                tr("Are you sure?\n"),
                                                                QMessageBox::Cancel | QMessageBox::No | QMessageBox::Yes,
                                                                QMessageBox::Yes);
    if (resBtn != QMessageBox::Yes) {
        event->ignore();
    } else {
        CloseButtonClicked();
    }
}

void AddLangWindow::CloseButtonClicked() {
    QueryModel->~QSqlQueryModel();
    this->close();
    emit WindowClosed();
}
