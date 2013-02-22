#include "MainWindow.hpp"

#include <QAction>
#include <QActionGroup>
#include <QApplication>
#include <QDebug>
#include <QIcon>
#include <QMenu>
#include <QMessageBox>
#include <QPixmap>

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_fwPolicy(NULL)
    , m_currProfile(NET_FW_PROFILE2_PUBLIC)
{
    if (!acquireFirewallPolicy() || !m_fwPolicy) {
        QMessageBox::critical(this, "Fatal Error", "Could not acquire firewall policy");
        QApplication::exit(1);
    }

    createActions();
    createMenu();
    updateCurrentState();
    createTrayIcon();

    resize(1, 1);
    move(0, 0);
    hide();
}

MainWindow::~MainWindow()
{
    if (m_fwPolicy) {
        m_fwPolicy->Release();
        m_fwPolicy = NULL;
    }

    CoUninitialize();
}

void MainWindow::createTrayIcon()
{
    HICON iconHandle = LoadIcon(GetModuleHandle(NULL), MAKEINTRESOURCE(2000));

    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(QPixmap::fromWinHICON(iconHandle)));
    m_trayIcon->setContextMenu(m_contextMenu);
    m_trayIcon->setToolTip("Firewall Control");
    m_trayIcon->show();

    connect(m_trayIcon, SIGNAL(activated(QSystemTrayIcon::ActivationReason)),
            SLOT(trayIconActivated(QSystemTrayIcon::ActivationReason)));

    DestroyIcon(iconHandle);
}

void MainWindow::updateCurrentState()
{
    bool enabled = isFirewallEnabled();
    NET_FW_ACTION inboundAction = getDefaultInboundAction();
    NET_FW_ACTION outboundAction = getDefaultOutboundAction();

    if (enabled)
        m_actFirewallEnable->setChecked(true);
    else
        m_actFirewallDisable->setChecked(true);

    if (inboundAction == NET_FW_ACTION_ALLOW)
        m_actInboundAllow->setChecked(true);
    else
        m_actInboundBlock->setChecked(true);

    if (outboundAction == NET_FW_ACTION_ALLOW)
        m_actOutboundAllow->setChecked(true);
    else
        m_actOutboundBlock->setChecked(true);
}

void MainWindow::createMenu()
{
    m_contextMenu = new QMenu(this);

    m_contextMenu->addAction(m_actFirewallEnable);
    m_contextMenu->addAction(m_actFirewallDisable);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_actOutboundAllow);
    m_contextMenu->addAction(m_actOutboundBlock);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_actInboundAllow);
    m_contextMenu->addAction(m_actInboundBlock);
    m_contextMenu->addSeparator();
    m_contextMenu->addAction(m_actExit);

    connect(m_contextMenu, SIGNAL(aboutToShow()), SLOT(updateCurrentState()));
}

void MainWindow::createActions()
{
#define CREATE_ACTION(obj, text, slot_func) \
            obj = new QAction(this);        \
            obj->setCheckable(true);        \
            obj->setText(QString(text));    \
            connect(obj, SIGNAL(triggered()), SLOT(slot_func()))

    CREATE_ACTION(m_actFirewallDisable, "Firewall Disable", menuFirewallDisable);
    CREATE_ACTION(m_actFirewallEnable,  "Firewall Enable",  menuFirewallEnable);
    CREATE_ACTION(m_actOutboundBlock,   "Outbound Block",   menuOutboundBlock);
    CREATE_ACTION(m_actOutboundAllow,   "Outbound Allow",   menuOutboundAllow);
    CREATE_ACTION(m_actInboundBlock,    "Inbound Block",    menuInboundBlock);
    CREATE_ACTION(m_actInboundAllow,    "Inbound Allow",    menuInboundAllow);

#undef CREATE_ACTION

    m_groupFirewall = new QActionGroup(this);
    m_groupFirewall->addAction(m_actFirewallDisable);
    m_groupFirewall->addAction(m_actFirewallEnable);

    m_groupOutbound = new QActionGroup(this);
    m_groupOutbound->addAction(m_actOutboundBlock);
    m_groupOutbound->addAction(m_actOutboundAllow);

    m_groupInbound = new QActionGroup(this);
    m_groupInbound->addAction(m_actInboundBlock);
    m_groupInbound->addAction(m_actInboundAllow);

    m_actExit = new QAction(this);
    m_actExit->setCheckable(false);
    m_actExit->setText("Exit");
    connect(m_actExit, SIGNAL(triggered()), SLOT(menuExit()));

}

bool MainWindow::acquireFirewallPolicy()
{
    HRESULT res = S_OK;

    if (m_fwPolicy) {
        qDebug() << "Something goes wrong, m_fwPolicy not null";
        return false;
    }

    res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (FAILED(res)) {
        qDebug() << "Could not initialize COM";
        return false;
    }

    res = CoCreateInstance(CLSID_NetFwPolicy2, NULL, CLSCTX_INPROC_SERVER,
                           IID_INetFwPolicy2, (void**)&m_fwPolicy);

    if (FAILED(res)) {
        qDebug() << "Could not initialize instance of INetFwPolicy2";
        return false;
    }

    return true;
}

NET_FW_ACTION MainWindow::getDefaultInboundAction()
{
    NET_FW_ACTION action;
    HRESULT res = m_fwPolicy->get_DefaultInboundAction(m_currProfile, &action);

    if (FAILED(res))
        qDebug() << "Failed get_DefaultInboundAction";

    return action;
}

NET_FW_ACTION MainWindow::getDefaultOutboundAction()
{
    NET_FW_ACTION action;
    HRESULT res = m_fwPolicy->get_DefaultOutboundAction(m_currProfile, &action);

    if (FAILED(res))
        qDebug() << "Failed get_DefaultOutboundAction";

    return action;
}

void MainWindow::setDefaultInboundAction(NET_FW_ACTION action)
{
    if (getDefaultInboundAction() != action) {
        HRESULT res = m_fwPolicy->put_DefaultInboundAction(m_currProfile, action);

        if (FAILED(res))
            qDebug() << "Failed put_DefaultInboundAction";
    }
}

void MainWindow::setDefaultOutboundAction(NET_FW_ACTION action)
{
    if (getDefaultOutboundAction() != action) {
        HRESULT res = m_fwPolicy->put_DefaultOutboundAction(m_currProfile, action);

        if (FAILED(res))
            qDebug() << "Failed put_DefaultOutboundAction";
    }
}

bool MainWindow::isFirewallEnabled()
{
    VARIANT_BOOL status;

    HRESULT res = m_fwPolicy->get_FirewallEnabled(m_currProfile, &status);

    if (FAILED(res))
        qDebug() << "Failed get_FirewallEnabled";

    return (status == VARIANT_TRUE);
}

void MainWindow::setFirewallEnabled(bool enabled)
{
    if (isFirewallEnabled() != enabled) {
        VARIANT_BOOL status = (enabled ? VARIANT_TRUE : VARIANT_FALSE);

        HRESULT res = m_fwPolicy->put_FirewallEnabled(m_currProfile, status);

        if (FAILED(res))
            qDebug() << "Failed put_FirewallEnabled";
    }
}

void MainWindow::menuExit()
{
    close();
    m_trayIcon->hide();
    QApplication::exit(0);
}

void MainWindow::menuFirewallEnable()
{
    setFirewallEnabled(true);
}

void MainWindow::menuFirewallDisable()
{
    setFirewallEnabled(false);
}

void MainWindow::menuOutboundAllow()
{
    setDefaultOutboundAction(NET_FW_ACTION_ALLOW);
}

void MainWindow::menuOutboundBlock()
{
    setDefaultOutboundAction(NET_FW_ACTION_BLOCK);
}

void MainWindow::menuInboundAllow()
{
    setDefaultInboundAction(NET_FW_ACTION_ALLOW);
}

void MainWindow::menuInboundBlock()
{
    setDefaultInboundAction(NET_FW_ACTION_BLOCK);
}

void MainWindow::trayIconActivated(QSystemTrayIcon::ActivationReason reason)
{
    if (reason == QSystemTrayIcon::Trigger) {
        if (m_trayIcon->contextMenu()) {
            m_trayIcon->contextMenu()->popup(QCursor::pos());
            m_trayIcon->contextMenu()->activateWindow();
        }
    }
}
