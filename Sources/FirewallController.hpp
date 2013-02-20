#ifndef FIREWALLCONTROLLER_HPP
#define FIREWALLCONTROLLER_HPP

#include <QObject>

#include <netfw.h>
#include <windows.h>

class FirewallController : public QObject
{
    Q_OBJECT

public:
    explicit FirewallController(QObject *parent = 0);
    virtual ~FirewallController();

    NET_FW_ACTION defaultInboundAction();
    NET_FW_ACTION defaultOutboundAction();

    void setDefaultInboundAction(NET_FW_ACTION action);
    void setDefaultOutboundAction(NET_FW_ACTION action);

    bool isFirewallEnabled();
    void setFirewallEnabled(bool enabled);

private:
    bool acquireFirewallPolicy();
    VARIANT_BOOL boolNativeToVariant(bool value);
    bool boolVariantToNative(VARIANT_BOOL value);

private:
    INetFwPolicy2* m_policy;
};

#endif // FIREWALLCONTROLLER_HPP
