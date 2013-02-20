#include "FirewallController.hpp"

#include <QDebug>
#include <QMessageBox>
#include <QApplication>

// MSDN:
// You must include the #define _WIN32_DCOM preprocessor directive
// at the beginning of your code to be able to use CoInitializeEx.
#define _WIN32_DCOM

FirewallController::FirewallController(QObject* parent)
    : QObject(parent)
    , m_policy(NULL)
{
    if (!acquireFirewallPolicy()) {
        QMessageBox::critical(0, "Fatal error", "Could not acquire firewall status");
        QApplication::exit(1);
    }
}

FirewallController::~FirewallController()
{
    if (m_policy) {
        m_policy->Release();
        m_policy = NULL;
    }
}

NET_FW_ACTION FirewallController::defaultInboundAction()
{
    Q_ASSERT_X(m_policy, "FirewallController::defaultInboundAction()", "m_policy is NULL");

    NET_FW_ACTION action;
    HRESULT res = m_policy->get_DefaultInboundAction(NET_FW_PROFILE2_PUBLIC, &action);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed get_DefaultInboundAction");

    return action;
}

NET_FW_ACTION FirewallController::defaultOutboundAction()
{
    Q_ASSERT_X(m_policy, "FirewallController::defaultOutboundAction()", "m_policy is NULL");

    NET_FW_ACTION action;
    HRESULT res = m_policy->get_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, &action);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed get_DefaultOutboundAction");

    return action;
}

void FirewallController::setDefaultInboundAction(NET_FW_ACTION action)
{
    Q_ASSERT_X(m_policy, "FirewallController::setDefaultInboundAction", "m_policy is NULL");

    HRESULT res = m_policy->put_DefaultInboundAction(NET_FW_PROFILE2_PUBLIC, action);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed put_DefaultInboundAction");
}

void FirewallController::setDefaultOutboundAction(NET_FW_ACTION action)
{
    Q_ASSERT_X(m_policy, "FirewallController::setDefaultOutboundAction", "m_policy is NULL");

    HRESULT res = m_policy->put_DefaultOutboundAction(NET_FW_PROFILE2_PUBLIC, action);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed put_DefaultOutboundAction");
}

bool FirewallController::isFirewallEnabled()
{
    Q_ASSERT_X(m_policy, "FirewallController::isFirewallEnabled", "m_policy is NULL");

    VARIANT_BOOL status;
    HRESULT res = m_policy->get_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, &status);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed get_FirewallEnabled");

    return boolVariantToNative(status);
}

void FirewallController::setFirewallEnabled(bool enabled)
{
    Q_ASSERT_X(m_policy, "FirewallController::setFirewallEnabled", "m_policy is NULL");

    if (isFirewallEnabled() == enabled)
        return;

    VARIANT_BOOL status = boolNativeToVariant(enabled);

    HRESULT res = m_policy->put_FirewallEnabled(NET_FW_PROFILE2_PUBLIC, status);

    if (FAILED(res))
        QMessageBox::warning(0, "Error", "Failed put_FirewallEnabled");
}

bool FirewallController::acquireFirewallPolicy()
{
    HRESULT res = S_OK;

    if (m_policy) {
        qDebug() << "Something goes wrong, m_policy not null";
        return false;
    }

    res = CoInitializeEx(NULL, COINIT_APARTMENTTHREADED);

    if (FAILED(res)) {
        qDebug() << "Could not initialize COM";
        return false;
    }

    res = CoCreateInstance(__uuidof(NetFwPolicy2), NULL, CLSCTX_INPROC_SERVER,
                           __uuidof(INetFwPolicy2), reinterpret_cast<void**>(&m_policy));

    if (FAILED(res)) {
        qDebug() << "Could not initialize instance of INetFwPolicy2";
        return false;
    }

    return true;
}

VARIANT_BOOL FirewallController::boolNativeToVariant(bool value)
{
    return (value ? VARIANT_TRUE : VARIANT_FALSE);
}

bool FirewallController::boolVariantToNative(VARIANT_BOOL value)
{
    return ((value == VARIANT_TRUE) ? (true) : (false));
}
