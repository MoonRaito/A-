#include "cellitem.h"

#include <QPainter>
#include <QPen>

#include <QDebug>

CellItem::CellItem(const QRectF &rect, QGraphicsItem *parent, SharedOption option) :
    QGraphicsRectItem(rect, parent),
    m_option(option),
    m_cellType(kCellNormal),
    m_topLeftText("F"),
    //m_topRightText(),
    m_bottomLeftText("G"),
    m_bottomRightText("H"),
    m_isTextDrawn(false)
{
    if (!m_option) {
        m_option = SharedOption(new CellItemOption);
    }
}

CellItem::~CellItem()
{
}

CellItem &CellItem::operator=(const CellItem &cell)
{
    m_option = cell.m_option;
    m_cellType = cell.m_cellType;
    m_topLeftText = cell.m_topLeftText;
    m_topRightText = cell.m_topRightText;
    m_bottomLeftText = cell.m_bottomLeftText;
    m_bottomRightText = cell.m_bottomRightText;
    m_isTextDrawn = cell.m_isTextDrawn;
    return *this;
}

void CellItem::paint(QPainter *painter,
                     const QStyleOptionGraphicsItem *option,
                     QWidget *widget)
{
    Q_UNUSED(option)
    Q_UNUSED(widget)
    painter->setRenderHints(QPainter::Antialiasing);
    painter->setViewport(rect().toRect());
    painter->setWindow(0, 0, 100, 100);
    draw(*painter);
}

void CellItem::draw(QPainter &painter)
{
    // draw background
    const QRectF &bgRect = painter.window();
    QPen bgPen(m_option->m_strokeColor, m_option->m_strokeWidth,
             Qt::SolidLine, Qt::FlatCap, Qt::MiterJoin);
    painter.setPen(bgPen);
    painter.setBrush(m_option->m_fillColorMap[m_cellType]);
    painter.drawRect(bgRect);
    // draw text
    if (m_isTextDrawn) {
        qreal w = bgRect.width(), h = bgRect.height();
        QFont font("Microsoft YaHei");
        font.setPixelSize(qMin(w / 5, h / 5));
        painter.setFont(font);
        painter.setPen(m_option->m_textColor);
        qreal halfW = w / 2, halfH = h / 2,
              paddingW = halfW / 8, paddingH = halfH / 8;
        QRectF topLeftRect(bgRect.topLeft(), bgRect.center());
        drawText(painter, topLeftRect.adjusted(paddingW, paddingH,
                                               -paddingW, -paddingH),
                 m_topLeftText, Qt::AlignTop | Qt::AlignLeft);
        drawText(painter, topLeftRect.adjusted(halfW + paddingW, paddingH,
                                               halfW - paddingW, -paddingH),
                 m_topRightText, Qt::AlignTop | Qt::AlignRight);
        drawText(painter, topLeftRect.adjusted(paddingW, halfH + paddingH,
                                               -paddingW, halfH - paddingH),
                 m_bottomLeftText, Qt::AlignBottom | Qt::AlignLeft);
        drawText(painter, topLeftRect.adjusted(halfW + paddingW, halfH + paddingH,
                                               halfW - paddingW, halfH - paddingH),
                 m_bottomRightText, Qt::AlignBottom | Qt::AlignRight);
    }
}

void CellItem::drawText(QPainter &painter, const QRectF &rect,
                        const QString &text, Qt::Alignment alignment)
{
    if (text.isEmpty())
        return;
    //painter.save();
    painter.drawText(rect, text, QTextOption(alignment));
    //painter.restore();
}


CellItemOption::CellItemOption() :
    m_strokeWidth(1),
    m_strokeColor(QColor(0, 0, 0, 0x33)),
    m_textColor(Qt::black)
{
    m_fillColorMap[CellItem::kCellNormal] = Qt::white;
    m_fillColorMap[CellItem::kCellUnwalkable] = QColor(0x80, 0x80, 0x80);
    m_fillColorMap[CellItem::kCellStart] = QColor(0, 0xdd, 0);
    m_fillColorMap[CellItem::kCellEnd] = QColor(0xee, 0x44, 0);
    m_fillColorMap[CellItem::kCellOpened] = QColor(0x98, 0xfb, 0x98);
    m_fillColorMap[CellItem::kCellClosed] = QColor(0xaf, 0xee, 0xee);
}

CellItemOption::~CellItemOption()
{
}
