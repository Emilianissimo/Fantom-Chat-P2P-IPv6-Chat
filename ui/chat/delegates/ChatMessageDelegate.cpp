#include <QStyledItemDelegate>
#include <QPainter>
#include <QModelIndex>

class ChatMessageDelegate : public QStyledItemDelegate {
public:
    using QStyledItemDelegate::QStyledItemDelegate;

    void paint(QPainter* painter, const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        painter->save();

        QString text = index.data(Qt::DisplayRole).toString();

        QTextOption textOption;
        textOption.setWrapMode(QTextOption::WordWrap);
        textOption.setAlignment(Qt::AlignLeft | Qt::AlignVCenter);

        QRect textRect = option.rect.adjusted(8, 4, -8, -4); // padding
        painter->setPen(option.palette.color(QPalette::Text));
        painter->drawText(textRect, text, textOption);

        painter->restore();
    }

    QSize sizeHint(const QStyleOptionViewItem& option, const QModelIndex& index) const override {
        QString text = index.data(Qt::DisplayRole).toString();

        QFontMetrics fm(option.font);
        int width = option.rect.width() > 0 ? option.rect.width() : 300; // fallback
        QRect rect = fm.boundingRect(QRect(0, 0, width - 16, INT_MAX), Qt::TextWordWrap, text);
        return rect.size() + QSize(16, 8); // padding
    }
};
