#ifndef TABLEVIEW_H
#define TABLEVIEW_H

#include <QTableView>
#include <QStandardItemModel>

class TableView : public QTableView
{
    Q_OBJECT

public:
    TableView(QWidget *parent = nullptr);
    ~TableView();

    void setHorizontal(QStringList horizontal);
    void setInfo(QVector<QStringList> data);
    void addInfo(QStringList data);
    void clear();
    int rowCount();
private:
    QStandardItemModel* m_model;
};

#endif // TABLEVIEW_H
