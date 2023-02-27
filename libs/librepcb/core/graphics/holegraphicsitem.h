/*
 * LibrePCB - Professional EDA for everyone!
 * Copyright (C) 2013 LibrePCB Developers, see AUTHORS.md for contributors.
 * https://librepcb.org/
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef LIBREPCB_CORE_HOLEGRAPHICSITEM_H
#define LIBREPCB_CORE_HOLEGRAPHICSITEM_H

/*******************************************************************************
 *  Includes
 ******************************************************************************/
#include "../geometry/hole.h"
#include "primitivepathgraphicsitem.h"

#include <QtCore>
#include <QtWidgets>

/*******************************************************************************
 *  Namespace / Forward Declarations
 ******************************************************************************/
namespace librepcb {

class IF_GraphicsLayerProvider;
class OriginCrossGraphicsItem;

/*******************************************************************************
 *  Class HoleGraphicsItem
 ******************************************************************************/

/**
 * @brief The HoleGraphicsItem class is the graphical representation of a
 *        librepcb::Hole
 */
class HoleGraphicsItem final : public QGraphicsItem {
public:
  // Constructors / Destructor
  HoleGraphicsItem() = delete;
  HoleGraphicsItem(const HoleGraphicsItem& other) = delete;
  HoleGraphicsItem(const Hole& hole, const IF_GraphicsLayerProvider& lp,
                   bool originCrossesVisible,
                   QGraphicsItem* parent = nullptr) noexcept;
  ~HoleGraphicsItem() noexcept;

  // Getters
  const Hole& getHole() noexcept { return mHole; }

  // Setters
  void setAutoStopMaskOffset(const Length& offset) noexcept;

  // Inherited from QGraphicsItem
  QRectF boundingRect() const noexcept override {
    return mHoleGraphicsItem->boundingRect();
  }
  QPainterPath shape() const noexcept override {
    return mHoleGraphicsItem->shape();
  }
  void paint(QPainter* painter, const QStyleOptionGraphicsItem* option,
             QWidget* widget = 0) noexcept override;

  // Operator Overloadings
  HoleGraphicsItem& operator=(const HoleGraphicsItem& rhs) = delete;

private:  // Methods
  void holeEdited(const Hole& hole, Hole::Event event) noexcept;
  void updateGeometry() noexcept;
  void updateMasksVisibility() noexcept;
  QVariant itemChange(GraphicsItemChange change,
                      const QVariant& value) noexcept override;

private:  // Data
  const Hole& mHole;
  const IF_GraphicsLayerProvider& mLayerProvider;
  Length mAutoStopMaskOffset;
  QScopedPointer<PrimitivePathGraphicsItem> mHoleGraphicsItem;
  QScopedPointer<PrimitivePathGraphicsItem> mStopMaskGraphicsItemBot;
  QScopedPointer<PrimitivePathGraphicsItem> mStopMaskGraphicsItemTop;
  QScopedPointer<OriginCrossGraphicsItem> mOriginCrossGraphicsItemStart;
  QScopedPointer<OriginCrossGraphicsItem> mOriginCrossGraphicsItemEnd;

  // Slots
  Hole::OnEditedSlot mOnEditedSlot;
};

/*******************************************************************************
 *  End of File
 ******************************************************************************/

}  // namespace librepcb

#endif
