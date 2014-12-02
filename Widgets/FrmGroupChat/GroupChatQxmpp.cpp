#include "GroupChatQxmpp.h"
#include "Global/Global.h"
#include "ManageGroupChatQxmpp.h"
#include "qxmpp/QXmppUtils.h"
#include "qxmpp/QXmppMessage.h"
#include "ChatActionGroupChat.h"
#include "Manager/Manager.h"
#include "MainWindow.h"

#ifdef WIN32
#undef GetMessage
#undef SendMessage
#endif

CGroupChatQxmpp::CGroupChatQxmpp(QXmppMucRoom *pRoom, QObject *parent) :
    CGroupChat(parent)
{
    m_pRoom = pRoom;
    bool check = connect(pRoom, 
                         SIGNAL(joined()),
                         this,
                         SLOT(slotJoined()));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(allowedActionsChanged(QXmppMucRoom::Actions)),
            SLOT(slotAllowedActionsChanged(QXmppMucRoom::Actions)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(configurationReceived(QXmppDataForm)),
                    SLOT(slotConfigurationReceived(QXmppDataForm)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(error(QXmppStanza::Error)),
                    SLOT(slotError(QXmppStanza::Error)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(kicked(QString,QString)),
                    SIGNAL(sigKick(QString,QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(left()),
                    SLOT(slotLeft()));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(messageReceived(QXmppMessage)),
                    SLOT(slotMessageReceived(QXmppMessage)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(nameChanged(QString)),
                    SLOT(slotNameChanged(QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(nickNameChanged(QString)),
                    SLOT(slotNickNameChanged(QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(participantAdded(QString)),
                    SIGNAL(sigParticipantAdd(QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(participantRemoved(QString)),
                    SIGNAL(sigParticipantRemoved(QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(participantChanged(QString)),
                    SLOT(slotParticipantChanged(QString)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(participantsChanged()),
                    SLOT(slotParticipantsChanged()));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(permissionsReceived(QList<QXmppMucItem>)),
                    SLOT(slotPermissionsReceived(QList<QXmppMucItem>)));
    Q_ASSERT(check);

    check = connect(pRoom, SIGNAL(subjectChanged(QString)),
                    SLOT(slotSubjectChanged(QString)));
    Q_ASSERT(check);
}

void CGroupChatQxmpp::slotJoined()
{
    emit GETMANAGER->GetManageGroupChat()->sigJoined(Id());
}

void CGroupChatQxmpp::slotAllowedActionsChanged(QXmppMucRoom::Actions actions)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotAllowedActionsChanged:%d", actions);
}

void CGroupChatQxmpp::slotConfigurationReceived(const QXmppDataForm &configuration)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotConfigurationReceived");
}

void CGroupChatQxmpp::slotError(const QXmppStanza::Error &error)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotError");
}

void CGroupChatQxmpp::slotKicked(const QString &jid, const QString &reason)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotKicked:%s", qPrintable(jid));
}

void CGroupChatQxmpp::slotIsJoinedChanged()
{
     LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotIsJoinedChanged");
}

void CGroupChatQxmpp::slotLeft()
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotLeft");
    GETMANAGER->GetManageGroupChat()->slotLeave(Id());
}

void CGroupChatQxmpp::slotMessageReceived(const QXmppMessage &message)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotMessageReceived:%s", message.body().toStdString().c_str());
    //是组消息,qxmpp在房间中响应此消息  
    if(QXmppMessage::GroupChat == message.type()
            && QXmppMessage::None == message.state())
    {
        QString szJid = message.from();
        //是本地用户发送的消息，忽略  
        if(QXmppUtils::jidToBareJid(m_pRoom->participantFullJid(szJid)) == USER_INFO_LOCALE->GetInfo()->GetId())
            return;
        //组消息  
        QSharedPointer<CMessage> msg = GetMessage();
        QSharedPointer<CChatActionGroupChat> cgc(new CChatActionGroupChat(Id(), message.body(), szJid, QTime::currentTime(), false));
        msg->AddMessage(cgc);
        GET_MAINWINDOW->ShowTrayIconMessage(this->ParticipantNick(szJid) + ":", message.body());
        emit GETMANAGER->GetManageGroupChat()->sigUpdateMessage(Id());
    }
}

void CGroupChatQxmpp::slotNameChanged(const QString &name)
{
     LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotNameChanged:%s", qPrintable(name));
}

void CGroupChatQxmpp::slotNickNameChanged(const QString &nickName)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotNickNameChanged:%s", qPrintable(nickName));
}

void CGroupChatQxmpp::slotParticipantAdded(const QString &jid)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotParticipantAdded:%s", qPrintable(jid));
    emit sigParticipantAdd(jid);
}

void CGroupChatQxmpp::slotParticipantChanged(const QString &jid)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotParticipantChanged:%s", qPrintable(jid));
}

void CGroupChatQxmpp::slotParticipantRemoved(const QString &jid)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotParticipantRemoved:%s", qPrintable(jid));
    emit sigParticipantRemoved(jid);
}

void CGroupChatQxmpp::slotParticipantsChanged()
{
     LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotParticipantsChanged");
}

void CGroupChatQxmpp::slotPermissionsReceived(const QList<QXmppMucItem> &permissions)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotPermissionsReceived");
}

void CGroupChatQxmpp::slotSubjectChanged(const QString &subject)
{
    LOG_MODEL_DEBUG("CGroupChatQxmpp", "CGroupChatQxmpp::slotSubjectChanged:%s", qPrintable(subject));
}

QString CGroupChatQxmpp::Id()
{
    RABBITIM_ASSERT(m_pRoom, QString());
    return m_pRoom->jid();
}

QString CGroupChatQxmpp::ShowName()
{
    QString szName;
    if(m_pRoom)
    {
        szName = m_pRoom->name();
        LOG_MODEL_DEBUG("CGroupChatQxmpp", "name:%s", m_pRoom->name().toStdString().c_str());
        if(szName.isEmpty())
        {
            szName = QXmppUtils::jidToUser(m_pRoom->jid());
        }
    }
    return szName;
}

QString CGroupChatQxmpp::Nick()
{
    RABBITIM_ASSERT(m_pRoom, QString());
    return m_pRoom->nickName();
}

QString CGroupChatQxmpp::Subject()
{
    RABBITIM_ASSERT(m_pRoom, QString());
    return m_pRoom->subject();
}

QStringList CGroupChatQxmpp::Participants()
{
    RABBITIM_ASSERT(m_pRoom, QStringList());
    return m_pRoom->participants();
}

QString CGroupChatQxmpp::ParticipantNick(const QString &szId)
{
    return QXmppUtils::jidToResource(szId);
    return m_pRoom->participantPresence(szId).mucItem().nick();
}

QString CGroupChatQxmpp::ParticipantId(const QString &szId)
{
    return QXmppUtils::jidToBareJid(m_pRoom->participantPresence(szId).mucItem().jid());
}

int CGroupChatQxmpp::Leave()
{
    RABBITIM_ASSERT(m_pRoom, -1);
    m_pRoom->leave();
    return 0;
}

int CGroupChatQxmpp::SendMessage(const QString &szMsg)
{
    RABBITIM_ASSERT(m_pRoom, -1);
    m_pRoom->sendMessage(szMsg);
    QSharedPointer<CMessage> msg = GetMessage();
    QSharedPointer<CChatActionGroupChat> cgc(new CChatActionGroupChat(Id(), szMsg, QString(), QTime::currentTime(), true));
    msg->AddMessage(cgc);
    emit GETMANAGER->GetManageGroupChat()->sigUpdateMessage(Id());
    return 0;
}

int CGroupChatQxmpp::Kick(const QString &szId, const QString &reason)
{
    m_pRoom->kick(szId, reason);
}

int CGroupChatQxmpp::SetConfigure(const QString &szName, const QString &szSubject, const QString &szPassword, const QString &szDescription, bool bProtracted, bool bPrivated, const QString &szNick)
{
    QXmppDataForm form(QXmppDataForm::Submit);
    QList<QXmppDataForm::Field> fields;
    {
        QXmppDataForm::Field field(QXmppDataForm::Field::HiddenField);
        field.setKey("FORM_TYPE");
        field.setValue("http://jabber.org/protocol/muc#roomconfig");
        fields.append(field);
    }
    QXmppDataForm::Field field;
    field.setKey("muc#roomconfig_roomname");
    field.setValue(szName);
    fields.append(field);
    field.setKey("muc#roomconfig_subject");
    field.setValue(szSubject);
    fields.append(field);
    field.setKey("muc#roomconfig_roomdesc");
    field.setValue(szDescription);
    fields.append(field);

    field.setKey("muc#roomconfig_roomsecret");
    field.setValue(szPassword);
    fields.append(field);
    QXmppDataForm::Field field1(QXmppDataForm::Field::BooleanField);
    field1.setKey("muc#roomconfig_passwordprotectedroom");
    field1.setValue(true);
    fields.append(field1);

    field1.setKey("muc#roomconfig_membersonly");
    field1.setValue(bPrivated);
    fields.append(field1);
    
    field1.setKey("muc#roomconfig_persistentroom");
    field1.setValue(bProtracted);
    fields.append(field1);
    
    form.setFields(fields);
    m_pRoom->setConfiguration(form);
    return 0;
}
