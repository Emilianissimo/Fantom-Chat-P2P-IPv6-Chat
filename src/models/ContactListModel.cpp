#include "ContactListModel.h"

ContactListModel::ContactListModel(QObject *parent)
    : QAbstractListModel{parent}
{}

void ContactListModel::onNewMessage(const QString& chatID, const QString& clientID, const QString& message) {
    auto it = std::find_if(
        m_contacts.begin(),
        m_contacts.end(),
        [&](const Contact& c) { return c.chatID == chatID; }
    );

    if (it != m_contacts.end()) {
        // Only if contact in the list
        it->lastMessage = message;

        if (it != m_contacts.begin()) {
            // Move contact on top if it is somewhere else
            for (Contact& c : m_contacts)
                c.order += 1;
            it->order = 0;

            std::sort(m_contacts.begin(), m_contacts.end(), [](const Contact& a, const Contact& b) {
                return a.order < b.order;
            });

            beginResetModel();
            endResetModel();
        } else {
            // Update data if conctact already on top
            emit dataChanged(index(0), index(0));
        }
    } else {
        Contact newContact{ chatID, clientID, message, 0 };
        for (Contact& c : m_contacts)
            c.order += 1;

        m_contacts.push_back(newContact);
        std::sort(m_contacts.begin(), m_contacts.end(), [](const Contact& a, const Contact& b) {
            return a.order < b.order;
        });

        beginResetModel();
        endResetModel();
    }
}

void ContactListModel::setContacts(const QVector<Contact>& contacts) {
    beginResetModel();
    m_contacts = contacts;
    std::sort(m_contacts.begin(), m_contacts.end(), [](const Contact& a, const Contact& b) {
        return a.order < b.order;
    });
    endResetModel();
}

QVariant ContactListModel::data(const QModelIndex& index, int role) const {
    if (!index.isValid() || index.row() < 0 || index.row() >= m_contacts.size())
        return {};

    const Contact& contact = m_contacts.at(index.row());

    switch (role) {
        case ChatIDRole: return contact.chatID;
        case ClientIDRole: return contact.clientID;
        case LastMessageRole: return contact.lastMessage;
        case OrderRole: return contact.order;
        default: return {};
    }
}

QHash<int, QByteArray> ContactListModel::roleNames() const {
    return {
        { ChatIDRole, "chatID" },
        { ClientIDRole, "clientID" },
        { LastMessageRole, "lastMessage" },
        { OrderRole, "order" }
    };
}

int ContactListModel::rowCount(const QModelIndex& parent) const {
    if (parent.isValid())
        return 0;
    return m_contacts.size();
}
