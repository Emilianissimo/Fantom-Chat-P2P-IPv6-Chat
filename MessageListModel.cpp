#include "MessageListModel.h"

MessageListModel::MessageListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void MessageListModel::setMessages(const QList<Message>& messages)
{
    beginResetModel();
    m_messages = messages;
    endResetModel();
}

int  MessageListModel::rowCount(const QModelIndex& parent) const
{
    Q_UNUSED(parent);
    return m_messages.size();
}

QVariant MessageListModel::data(const QModelIndex& index, int role) const
{
    if (!index.isValid() || index.row() >= m_messages.size())
        return QVariant();

    if (role != Qt::DisplayRole)
        return QVariant();

    const Message& msg = m_messages[index.row()];
    return QString("%1: %2")
        .arg(msg.isIncoming ? msg.peerID : "You")
        .arg(msg.message);
}

void MessageListModel::addMessage(const Message& msg)
{
    beginInsertRows(QModelIndex(), m_messages.size(), m_messages.size());
    m_messages.append(msg);
    endInsertRows();
}
