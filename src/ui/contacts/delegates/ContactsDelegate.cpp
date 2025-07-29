#include "../../src/utils/Structures.h"
#include "../../../models/ContactListModel.h"
#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>

class ContactsDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        painter->save();

        QRect rect = option.rect;

        // Extract data
        bool isActive = index.data(ContactListModel::IsActiveRole).toBool();
        QString clientID = index.data(ContactListModel::ClientIDRole).toString();
        QString lastMsg  = index.data(ContactListModel::LastMessageRole).toString();

        QColor bgColor;
        if (isActive) {
            bgColor = (option.state & QStyle::State_MouseOver) ? QColor("#f5f5f5") : QColor("#ffffff");
        } else {
            // Hover / selected background
            bgColor = (option.state & QStyle::State_MouseOver) ? QColor("#afe5ec") : QColor("#8ebcc2");
        }
        painter->fillRect(rect, bgColor);

        // Padding
        const int x_padding = 5;
        const int y_padding = 10;
        QRect contentRect = rect.adjusted(x_padding, y_padding, -x_padding, -y_padding);

        // Fonts
        QFont idFont = option.font;
        idFont.setBold(true);

        QFontMetrics idFm(idFont);
        QString elidedID = idFm.elidedText(clientID, Qt::ElideRight, contentRect.width());

        QFont msgFont = option.font;
        QFontMetrics msgFm(msgFont);
        QString elidedMsg = msgFm.elidedText(lastMsg, Qt::ElideRight, contentRect.width());

        // Draw texts
        painter->setFont(idFont);
        painter->setPen(QColor("#000000"));
        painter->drawText(contentRect, Qt::AlignTop | Qt::AlignLeft, elidedID);

        painter->setFont(msgFont);
        painter->setPen(QColor("#666666"));

        QRect msgRect = contentRect;
        msgRect.setTop(contentRect.top() + idFm.height() + 4);
        painter->drawText(msgRect, Qt::AlignTop | Qt::AlignLeft, elidedMsg);


        painter->restore();

    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QFont idFont = option.font;
        idFont.setBold(true);
        QFontMetrics idFm(idFont);

        QFont msgFont = option.font;
        QFontMetrics msgFm(msgFont);

        int padding = 10;
        int totalHeight = padding * 2 + idFm.height() + 4 + msgFm.height();

        return QSize(option.rect.width(), totalHeight);
    }
};
