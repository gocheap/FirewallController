#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QSystemTrayIcon>

class FirewallController;
class QMenu;
class QSystemTrayIcon;
class QAction;
class QActionGroup;

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    explicit MainWindow(QWidget *parent = 0);
    virtual ~MainWindow();

private:
    void createTrayIcon();
    void createMenu();
    void createActions();

private slots:
    void updateCurrentState();
    void menuExit();
    void menuFirewallEnable();
    void menuFirewallDisable();
    void menuOutboundAllow();
    void menuOutboundBlock();
    void menuInboundAllow();
    void menuInboundBlock();

private:
    FirewallController* m_controller;

    QSystemTrayIcon* m_trayIcon;

    QMenu* m_contextMenu;

    QAction* m_actFirewallEnable;
    QAction* m_actFirewallDisable;
    QAction* m_actOutboundAllow;
    QAction* m_actOutboundBlock;
    QAction* m_actInboundAllow;
    QAction* m_actInboundBlock;
    QAction* m_actExit;

    QActionGroup* m_groupFirewall;
    QActionGroup* m_groupOutbound;
    QActionGroup* m_groupInbound;
};

#endif // MAINWINDOW_HPP
