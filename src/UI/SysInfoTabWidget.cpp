#include "SysInfoTabWidget.h"
#include "TableView.h"

SysInfoTabWidget::SysInfoTabWidget(QWidget *parent)
    : QTabWidget(parent)
{
    setWindowTitle(QStringLiteral("系统支持信息"));
}

SysInfoTabWidget::~SysInfoTabWidget()
{

}
