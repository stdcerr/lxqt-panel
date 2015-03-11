#include "statusnotifierwidget.h"
#include <QApplication>

StatusNotifierWidget::StatusNotifierWidget(ILxQtPanelPlugin *plugin, QWidget *parent) :
    QWidget(parent),
    mPlugin(plugin)
{
    QString dbusName = QString("org.kde.StatusNotifierHost-%1-%2").arg(QApplication::applicationPid()).arg(1);
    if (!QDBusConnection::sessionBus().registerService(dbusName))
        qDebug() << QDBusConnection::sessionBus().lastError().message();

    mWatcher = new StatusNotifierWatcher;
    mWatcher->RegisterStatusNotifierHost(dbusName);

    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemRegistered,
            this, &StatusNotifierWidget::itemAdded);
    connect(mWatcher, &StatusNotifierWatcher::StatusNotifierItemUnregistered,
            this, &StatusNotifierWidget::itemRemoved);

    setLayout(new LxQt::GridLayout(this));
    realign();

    qDebug() << mWatcher->RegisteredStatusNotifierItems();

}

void StatusNotifierWidget::itemAdded(QString serviceAndPath)
{
    QStringList item = serviceAndPath.split('/');
    QString serv = item.at(0);
    QString path = QLatin1Char('/') + item.at(1);
    StatusNotifierButton *button = new StatusNotifierButton(serv, path, this);

    mServices.insert(serviceAndPath, button);

    layout()->addWidget(button);
    layout()->setAlignment(button, Qt::AlignCenter);
    button->show();
}

void StatusNotifierWidget::itemRemoved(const QString &serviceAndPath)
{
    StatusNotifierButton *button = mServices.value(serviceAndPath, NULL);
    if (button)
    {
        button->deleteLater();
        layout()->removeWidget(button);
    }
}

void StatusNotifierWidget::realign()
{
    LxQt::GridLayout *layout = qobject_cast<LxQt::GridLayout*>(this->layout());
    layout->setEnabled(false);

    ILxQtPanel *panel = mPlugin->panel();
    if (panel->isHorizontal())
    {
        layout->setRowCount(panel->lineCount());
        layout->setColumnCount(0);
    }
    else
    {
        layout->setColumnCount(panel->lineCount());
        layout->setRowCount(0);
    }

    layout->setEnabled(true);
}