#include "gridscene.h"

#include <math.h>
#include <QGraphicsSceneMouseEvent>
#include <QMessageBox>
#include <QMutexLocker>
#include "scene/cellitem.h"

#include <QDebug>

#define CENTER_OFFSET_MAX 4

GridScene::GridScene(QObject *parent) :
    QGraphicsScene(parent),
    m_cellSize(QSizeF(40, 40)),
    m_linePen(QPen(Qt::yellow, 4)),
    m_state(kStateIdle),
    m_gridItem(0),
    m_startCell(0),
    m_endCell(0),
    m_selectedCell(0)
{
}

GridScene::~GridScene()
{
}

CellItem *GridScene::cellItemAt(int x, int y)
{
    return m_cellGrid[y][x];
}

void GridScene::callbackCellItemChanged(CellItem *cellItem)
{
    CellItem::CellType type = cellItem->cellType();
    if (type != CellItem::kCellStart && type != CellItem::kCellEnd) {
        type = cellItem->data(kCellClosed).toBool() ? CellItem::kCellClosed
                                                    : CellItem::kCellOpened;
        cellItem->setCellType(type);
        cellItem->setTopLeftText(cellItem->data(kCellF).toString());
        cellItem->setBottomLeftText(cellItem->data(kCellG).toString());
        cellItem->setBottomRightText(cellItem->data(kCellH).toString());
        cellItem->setTextDrawn(true);
        cellItem->update();
        m_ChangedCells.push_back(cellItem);
    }
}

void GridScene::callbackShortestPath(const CellItemVector &cells)
{
    if (cells.size() > 0) {
        addLines(cells);
    } else {
        QMessageBox::warning(NULL, "Warning",
                             "Cannot find the shortest path.",
                             QMessageBox::Ok);
    }
}

void GridScene::prepare(const QRectF &rect)
{
    if (sceneRect() == rect) {
        return;
    }
    setSceneRect(rect);

    qreal cellWidth = m_cellSize.width(),
          cellHeight = m_cellSize.height();
    int column = rect.width() / cellWidth,
        row = rect.height() / cellHeight;
    if (column < 2 || row < 1) {
        QMessageBox::warning(NULL, "Warning",
                             "Too few cells! "
                             "Confirm the cell size is not too large.",
                             QMessageBox::Ok);
        return;
    }
    qreal gridWidth = column * cellWidth,
          gridHeight = row * cellHeight;
    qreal left = (rect.width() - gridWidth) / 2;
    qreal top = (rect.height() - gridHeight) / 2;
    QRectF gridRect(left, top, gridWidth, gridHeight);

    // if already prepared
    if (m_gridItem) {
        // remove items
        removeItem(m_gridItem);
        // reset memebers
        m_lineVector.clear();
        m_ChangedCells.clear();
        m_state = kStateIdle;
    }
    m_gridItem = new QGraphicsRectItem(gridRect);
    m_gridItem->setPen(Qt::NoPen);
    setupCells(row, column);
    addItem(m_gridItem);
}

void GridScene::startSearch()
{
    clearLines();
    resetChangedCells();
    if (m_delegate)
        m_delegate->onSearchStarted(m_startCell, m_endCell);
}

void GridScene::setupCells(int row, int column)
{
    Q_ASSERT(m_gridItem);
    if (m_delegate)
        m_delegate->onPrepared(row, column);

    // shared cellitem option
    CellItem::SharedOption option = CellItem::SharedOption(new CellItemOption);

    // start and end cell initial position
    int rowMaxIndex = row - 1;
    int colMaxIndex = column - 1;
    qreal columnHalf = colMaxIndex / 2.0;
    qreal centerOffset = columnHalf;
    if (centerOffset > CENTER_OFFSET_MAX)
        centerOffset = CENTER_OFFSET_MAX;
    int centerLeft = ceil(columnHalf - centerOffset),
        centerRight = floor(columnHalf + centerOffset);
    centerLeft = qBound(0, centerLeft, colMaxIndex);
    centerRight = qBound(0, centerRight, colMaxIndex);
    int middle = qBound(0, row / 2, rowMaxIndex);

    qreal left = m_gridItem->rect().x(),
          top = m_gridItem->rect().y();
    qreal cellWidth = m_cellSize.width(),
          cellHeight = m_cellSize.height();
    qreal x = 0, y = top;
    m_cellGrid.resize(row);
    for (int r = 0; r < row; ++r) {
        x = left;
        m_cellGrid[r].resize(column);
        for (int c = 0; c < column; ++c) {
            CellItem *item = new CellItem(QRectF(QPointF(x, y), m_cellSize),
                                          m_gridItem, option);
            item->setData(kCellAxis, QPoint(c, r));
            // set start and end cell
            if (middle == r) {
                if (c == centerLeft) {
                    item->setCellType(CellItem::kCellStart);
                    m_startCell = item;
                }
                else if (c == centerRight) {
                    item->setCellType(CellItem::kCellEnd);
                    m_endCell = item;
                }
            }
            m_cellGrid[r][c] = item;
            if (m_delegate)
                m_delegate->onCellItemCreated(item);
            x += cellWidth;
        }
        y += cellHeight;
    }
}

void GridScene::itemSelectedAtScenePos(const QPointF &pos)
{
    QGraphicsItem *item = itemAt(pos, QTransform());
    if (!item) return;
    switch (item->type()) {
    case CellItem::Type:
        CellItem *cellItem = qgraphicsitem_cast<CellItem *>(item);
        if (cellItem) {
            itemSelected(cellItem);
        }
        break;
    }
}

void GridScene::itemSelected(CellItem *cellItem)
{
    QMutexLocker locker(&m_mutex);
    if (m_state == kStateSwap) {
        if (m_selectedCell != cellItem) {
            swapCellItem(m_selectedCell, cellItem);
            m_state = kStateIdle;
        }
        return;
    }
    switch (cellItem->cellType()) {
    case CellItem::kCellNormal:
    case CellItem::kCellUnwalkable:
    case CellItem::kCellOpened:
    case CellItem::kCellClosed:
        toggleCellWalkable(cellItem);
        break;
    case CellItem::kCellStart:
    case CellItem::kCellEnd:
        m_selectedCell = cellItem;
        m_state = kStateSwap;
        break;
    default:
        Q_ASSERT(false);
        break;
    }
}

void GridScene::toggleCellWalkable(CellItem *cellItem)
{
    if (m_state != kStateIdle)
        return;
    m_state = kStateToggle;
    if (cellItem->cellType() == CellItem::kCellUnwalkable) {
        cellItem->setCellType(CellItem::kCellNormal);
    } else {  // otherwise unwalkable
        cellItem->setCellType(CellItem::kCellUnwalkable);
    }
    if (m_delegate)
        m_delegate->onCellItemChanged(cellItem);
    cellItem->setTextDrawn(false);
    cellItem->update();
    // remove the changed cell
    int i;
    if (isChangedCell(cellItem, i)) {
        m_ChangedCells.removeAt(i);
    }
}

void GridScene::swapCellItem(CellItem *prev, CellItem *now)
{
    CellItem *temp = new CellItem(QRectF());
    *temp = *prev; *prev = *now; *now = *temp;  // swap their properties
    delete temp;
    // reset start and end cells
    if (prev->cellType() == CellItem::kCellStart) {
        m_startCell = prev;
    } else if (prev->cellType() == CellItem::kCellEnd) {
        m_endCell = prev;
    }
    if (now->cellType() == CellItem::kCellStart) {
        m_startCell = now;
    } else if (now->cellType() == CellItem::kCellEnd) {
        m_endCell = now;
    }
    if (m_delegate) {
        m_delegate->onCellItemChanged(prev);
        m_delegate->onCellItemChanged(now);
    }
    prev->update();
    now->update();
    // swap the changed cell
    // note: prev must be start or end cell,
    //       while m_ChangedCells wont contain it.
    int i;
    if (isChangedCell(now, i)) {
        m_ChangedCells.removeAt(i);
        m_ChangedCells.push_back(prev);
    }
}

void GridScene::addLines(const CellItemVector &cells)
{
    Q_ASSERT(cells.size() > 0);
    CellItemVector::const_iterator beg = cells.begin(),
                                   end = cells.end();
    CellItem *prev = *(beg++), *next = 0;
    while (beg != end) {
        next = *(beg++);
        addLine4TwoCells(prev, next);
        prev = next;
    }
}

void GridScene::addLine4TwoCells(CellItem *prev, CellItem *next)
{
    QGraphicsLineItem *line = new QGraphicsLineItem(m_gridItem);
    line->setZValue(10);  // make sure that the lines are drawn on top of others
    line->setPen(m_linePen);
    line->setLine(QLineF(prev->rect().center(), next->rect().center()));
    m_lineVector.push_back(line);
}

void GridScene::clearLines()
{
    foreach (QGraphicsLineItem *line, m_lineVector) {
        removeItem(line);
    }
    m_lineVector.clear();
}

void GridScene::resetChangedCells()
{
    foreach (CellItem *cell, m_ChangedCells) {
        cell->setCellType(CellItem::kCellNormal);
        cell->setTextDrawn(false);
        cell->update();
    }
    m_ChangedCells.clear();
}

bool GridScene::isChangedCell(CellItem *cellItem, int &index)
{
    int i = 0;
    foreach (CellItem *cell, m_ChangedCells) {
        if (cellItem == cell) {
            index = i;
            return true;
        }
        ++i;
    }
    return false;
}

void GridScene::mousePressEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    itemSelectedAtScenePos(mouseEvent->lastScenePos());
}

void GridScene::mouseReleaseEvent(QGraphicsSceneMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent)
    if (m_state == kStateToggle)
        m_state = kStateIdle;
}
