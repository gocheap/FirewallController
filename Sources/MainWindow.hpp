#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QWidget>
#include <QSystemTrayIcon>

#include <netfw.h>
#include <windows.h>

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
    bool acquireFirewallPolicy();

    NET_FW_ACTION getDefaultInboundAction();
    NET_FW_ACTION getDefaultOutboundAction();
    void setDefaultInboundAction(NET_FW_ACTION action);
    void setDefaultOutboundAction(NET_FW_ACTION action);
    bool isFirewallEnabled();
    void setFirewallEnabled(bool enabled);

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

    INetFwPolicy2* m_fwPolicy;
    NET_FW_PROFILE_TYPE2 m_currProfile;
};

#endif // MAINWINDOW_HPP
