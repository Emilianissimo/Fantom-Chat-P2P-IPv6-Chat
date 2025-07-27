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

    if (role != Qt::UserRole)
        return QVariant();

    return QVariant::fromValue(m_messages[index.row()]);
}

void MessageListModel::addMessage(const Message& msg)
{
    int newRow = m_messages.count();
    beginInsertRows(QModelIndex(), newRow, newRow);
    m_messages.append(msg);
    endInsertRows();
}
