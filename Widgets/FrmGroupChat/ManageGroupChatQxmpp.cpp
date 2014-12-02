#include "ManageGroupChatQxmpp.h"
#include "Global/Global.h"
#include "Client/ClientXmpp.h"
#include "GroupChatQxmpp.h"
#include "qxmpp/QXmppUtils.h"

CManageGroupChatQxmpp::CManageGroupChatQxmpp(QObject *parent) :
    CManageGroupChat(parent)
{
}

int CManageGroupChatQxmpp::Create(const QString &szName,
                                  const QString &szSubject,
                                  const QString &szPassword,
                                  const QString &szDescription,
                                  bool bProtracted,
                                  bool bPrivated, 
                                  const QString &szNick)
{
    QSharedPointer<CGroupChatQxmpp> gc = Join1(szName, szNick);
    if(gc.isNull())
        return -1;

    return gc->SetConfigure(szName, szSubject, szPassword, szDescription, bProtracted, bPrivated, szNick);
}

int CManageGroupChatQxmpp::Join(const QString &szId, const QString &szNick)
{
    Join1(szId, szNick);
    return 0;
}

QSharedPointer<CGroupChatQxmpp> CManageGroupChatQxmpp::Join1(const QString &szId, const QString &szNick)
{
    QString id = szId;
    if(szId.indexOf("@") == -1)
    {
        //TODO:这里按照xmpp协议是需要向服务器查询会议服务器的id，但是这会产生比较大的流量。  
        //所以现在的解决方案是直接在服务器上配置好一个固定的会议服务器，  
        //直接访问这个配置好的服务器即可  
        //当以后用户量大了后，服务器做集群，就需要完成xmpp的查询服务了。  
        id = szId + "@conference." + CGlobal::Instance()->GetXmppDomain();
    }
    if(m_GroupChat.find(id) != m_GroupChat.end())
        return QSharedPointer<CGroupChatQxmpp>();
    if(GET_CLIENT.isNull())
    {
        LOG_MODEL_ERROR("CManageGroupChatQxmpp", "GET_CLIENT is null");
        return QSharedPointer<CGroupChatQxmpp>();
    }
    CClientXmpp* client =(CClientXmpp*) GET_CLIENT.data();
    QXmppMucRoom *room = client->m_MucManager.addRoom(id);
    QString szNickName = szNick;
    if(szNick.isEmpty())
    {
        szNickName = USER_INFO_LOCALE->GetInfo()->GetName();
    }
    room->setNickName(szNickName);
    if(room->join())
    {
        QSharedPointer<CGroupChatQxmpp> groupChat(new CGroupChatQxmpp(room));
        m_GroupChat.insert(id, groupChat);
        return groupChat;
    }
    return QSharedPointer<CGroupChatQxmpp>();
}

QSharedPointer<CGroupChat> CManageGroupChatQxmpp::Get(const QString &szId)
{
    QMap<QString, QSharedPointer<CGroupChat> >::iterator it;
    it = m_GroupChat.find(szId);
    if(m_GroupChat.end() == it)
        return QSharedPointer<CGroupChat>();
    return it.value();
}

