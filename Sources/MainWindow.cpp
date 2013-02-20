#include "MainWindow.hpp"

#include <QMessageBox>
#include <QApplication>
#include <QMenu>
#include <QAction>
#include <QActionGroup>

#include "FirewallController.hpp"

MainWindow::MainWindow(QWidget* parent)
    : QWidget(parent)
    , m_controller(NULL)
{
    m_controller = new FirewallController(this);

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
}

void MainWindow::createTrayIcon()
{
    m_trayIcon = new QSystemTrayIcon(this);
    m_trayIcon->setIcon(QIcon(":/res/icons/network-firewall.png"));
    m_trayIcon->setContextMenu(m_contextMenu);
    m_trayIcon->setToolTip("Firewall Control");
    m_trayIcon->show();
}

void MainWindow::updateCurrentState()
{
    bool isFirewallEnabled = m_controller->isFirewallEnabled();
    NET_FW_ACTION inboundAction = m_controller->defaultInboundAction();
    NET_FW_ACTION outboundAction = m_controller->defaultOutboundAction();

    if (isFirewallEnabled)
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

    m_contextMenu->setTitle("Firewall settings");

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

    QObject::connect(m_contextMenu, SIGNAL(aboutToShow()), this, SLOT(updateCurrentState()));
}

void MainWindow::createActions()
{
    // Firewall
    m_actFirewallDisable = new QAction(this);
    m_actFirewallDisable->setCheckable(true);
    m_actFirewallDisable->setText("Firewall Disable");
    QObject::connect(m_actFirewallDisable, SIGNAL(triggered()), SLOT(menuFirewallDisable()));

    m_actFirewallEnable = new QAction(this);
    m_actFirewallEnable->setCheckable(true);
    m_actFirewallEnable->setText("Firewall Enable");
    QObject::connect(m_actFirewallEnable, SIGNAL(triggered()), SLOT(menuFirewallEnable()));

    m_groupFirewall = new QActionGroup(this);
    m_groupFirewall->addAction(m_actFirewallDisable);
    m_groupFirewall->addAction(m_actFirewallEnable);

    // Outbound
    m_actOutboundBlock = new QAction(this);
    m_actOutboundBlock->setCheckable(true);
    m_actOutboundBlock->setText("Outbound Block");
    QObject::connect(m_actOutboundBlock, SIGNAL(triggered()), SLOT(menuOutboundBlock()));

    m_actOutboundAllow = new QAction(this);
    m_actOutboundAllow->setCheckable(true);
    m_actOutboundAllow->setText("Outbound Allow");
    QObject::connect(m_actOutboundAllow, SIGNAL(triggered()), SLOT(menuOutboundAllow()));

    m_groupOutbound = new QActionGroup(this);
    m_groupOutbound->addAction(m_actOutboundBlock);
    m_groupOutbound->addAction(m_actOutboundAllow);

    // Inbound
    m_actInboundBlock = new QAction(this);
    m_actInboundBlock->setCheckable(true);
    m_actInboundBlock->setText("Inbound Block");
    QObject::connect(m_actInboundBlock, SIGNAL(triggered()), SLOT(menuInboundBlock()));

    m_actInboundAllow = new QAction(this);
    m_actInboundAllow->setCheckable(true);
    m_actInboundAllow->setText("Inbound Allow");
    QObject::connect(m_actInboundAllow, SIGNAL(triggered()), SLOT(menuInboundAllow()));

    m_groupInbound = new QActionGroup(this);
    m_groupInbound->addAction(m_actInboundBlock);
    m_groupInbound->addAction(m_actInboundAllow);

    // Exit
    m_actExit = new QAction(this);
    m_actExit->setCheckable(false);
    m_actExit->setText("Exit");
    QObject::connect(m_actExit, SIGNAL(triggered()), SLOT(menuExit()));

}

void MainWindow::menuExit()
{
    close();
    m_trayIcon->hide();
    QApplication::exit(0);
}

void MainWindow::menuFirewallEnable()
{
    Q_ASSERT(m_controller);

    if (!m_controller->isFirewallEnabled())
        m_controller->setFirewallEnabled(true);
}

void MainWindow::menuFirewallDisable()
{
    Q_ASSERT(m_controller);

    if (m_controller->isFirewallEnabled())
        m_controller->setFirewallEnabled(false);
}

void MainWindow::menuOutboundAllow()
{
    Q_ASSERT(m_controller);

    if (m_controller->defaultOutboundAction() != NET_FW_ACTION_ALLOW)
        m_controller->setDefaultOutboundAction(NET_FW_ACTION_ALLOW);
}

void MainWindow::menuOutboundBlock()
{
    Q_ASSERT(m_controller);

    if (m_controller->defaultOutboundAction() != NET_FW_ACTION_BLOCK)
        m_controller->setDefaultOutboundAction(NET_FW_ACTION_BLOCK);
}

void MainWindow::menuInboundAllow()
{
    Q_ASSERT(m_controller);

    if (m_controller->defaultInboundAction() != NET_FW_ACTION_ALLOW)
        m_controller->setDefaultInboundAction(NET_FW_ACTION_ALLOW);
}

void MainWindow::menuInboundBlock()
{
    Q_ASSERT(m_controller);

    if (m_controller->defaultInboundAction() != NET_FW_ACTION_BLOCK)
        m_controller->setDefaultInboundAction(NET_FW_ACTION_BLOCK);
}
