#include "TableView.h"
#include <QHeaderView>

TableView::TableView(QWidget *parent)
    : QTableView(parent)
{
    verticalHeader()->setVisible(false);
    setEditTriggers(QTableView::NoEditTriggers);
    setMouseTracking(true);
    setSelectionMode(QTableView::SingleSelection);
    setSelectionBehavior(QAbstractItemView::SelectRows);
}

TableView::~TableView()
{

}

void TableView::setHorizontal(QStringList horizontal)
{
    m_model = new QStandardItemModel(this);
    m_model->setColumnCount(horizontal.size());
    for(int i = 0; i < horizontal.size(); i++)
    {
        m_model->setHeaderData(i, Qt::Horizontal, horizontal[i]);
    }

    setModel(m_model);
}

void TableView::setInfo(QVector<QStringList> data)
{
    m_model->removeRows(0, m_model->rowCount());
    for(int i = 0; i < data.size(); i++)
    {
        QStringList text = data[i];
        for (int column = 0; column < text.size(); column++)
        {
            QStandardItem* item = new QStandardItem();
            item->setText(text[column]);
            item->setTextAlignment(Qt::AlignCenter);
            m_model->setItem(i, column, item);
        }
    }
}

void TableView::addInfo(QStringList data)
{
    int rowIndex = m_model->rowCount();
	for (int column = 0; column < data.size(); column++)
	{
		QStandardItem* item = new QStandardItem();
		item->setText(data[column]);
		item->setTextAlignment(Qt::AlignCenter);
		m_model->setItem(rowIndex, column, item);
	}
}

void TableView::clear()
{
    m_model->removeRows(0, m_model->rowCount());
}

int TableView::rowCount()
{
    return m_model->rowCount();
}
