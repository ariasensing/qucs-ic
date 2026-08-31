/***************************************************************************
                          node.cpp  -  description
                             -------------------
    begin                : Sat Sep 20 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/
#include "node.h"

#include "component.h"
#include "wire.h"
#include "schematic.h"
#include <QPainter>

Node::Node(int x, int y, Schematic* owner)
  : DType("")
  , State(0)
{
  Type  = isNode;

  cx = x;
  cy = y;

  setSchematicOwner(owner);
}

void Node::paint(QPainter* painter) const {
  painter->save();

  if (isSelected) {
      painter->setPen(QPen(Qt::darkGray, 5));
      painter->drawEllipse(cx-5, cy-5, 10, 10);
  }
  else if (conn_count() == 1) {
      if (hasLabel()) {
        painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue); // open but labeled
      } else {
        painter->setPen(QPen(Qt::red,1));  // node is open
        painter->drawEllipse(cx-4, cy-4, 8, 8);
      }
  }
  else if (conn_count() > 2) {
      painter->setBrush(Qt::darkBlue);  // more than 2 connections
      painter->setPen(QPen(Qt::darkBlue,1));
      painter->drawEllipse(cx-3, cy-3, 6, 6);
  }
  else if (m_wires.size() != 2) {
      painter->fillRect(cx-2, cy-2, 4, 4, Qt::darkBlue);
  }

  painter->restore();
}

bool Node::getSelected(int x, int y)
{
  return cx - 3 <= x && x <= cx + 3 && cy - 3 <= y && y <= cy + 3;
}

void Node::setName(const QString& name, const QString& value, int x, int y)
{
  // Passing two empty strings acted like a signal to remove the label
  // and later was superseded by dropLabel() method. This assertion is
  // just merely a guard against legacy usage, it may be freely removed
  // after some time.
  // Added on 2025-06-12.
  assert(!(name.isEmpty() && value.isEmpty()));

  if (!hasLabel()) {
    acquireLabel(std::make_unique<WireLabel>(name, cx, cy, x, y));
  }
  else {
    label()->setName(name);
  }
  label()->initValue = value;
}

bool Node::moveCenter(int dx, int dy) noexcept
{
  Element::moveCenter(dx, dy);
  if (hasLabel()) {
    label()->moveRoot(dx, dy);
  }
  return dx != 0 || dy != 0;
}

  Node* Node::merge(Node* donor)
  {
    std::ranges::for_each(donor->wires(), [this,donor](auto* w) { w->Port1 == donor ? w->Port1 = this : w->Port2 = this; });
    std::ranges::copy(donor->wires(), std::back_inserter(m_wires));
    donor->m_wires.clear();

    for (auto* c : donor->components()) {
        for (auto* p : std::as_const(c->Ports)) {
            if (p->Connection == donor) {
                p->Connection = this;
            }
        }
    }

    std::ranges::copy(donor->components(), std::back_inserter(m_components));
    donor->m_components.clear();

    if (!this->hasLabel() && donor->hasLabel()) {
        this->acquireLabel(donor->releaseLabel());
    }

    this->isSelected = this->isSelected || donor->isSelected;

    return donor;
}

bool Node::isOverlapping(int otherX, int otherY) const {
  return (otherX == x() && otherY == y());
}

bool Node::isOverlapping(const Node* other) const {
  // Comparison of self is false
  if (this == other) {
    return false;
  }

  return isOverlapping(other->x(), other->y());
}


void Node::connect(Wire* wire)
{
  if (!is_connected(wire))
  {
    m_wires.emplace_front(wire);
  }
}


void Node::connect(Component* comp)
{
  if (!is_connected(comp)) m_components.emplace_front(comp);

  if ((comp->Model=="GND")&&(getSchematicOwner()!=nullptr))
    getSchematicOwner()->rebuildConnectionAfterGndInsertion(this);


}


void Node::disconnect(Component* comp)
{
  m_components.remove(comp);
  if ((comp->Model=="GND")&&(getSchematicOwner()!=nullptr))
    getSchematicOwner()->rebuildConnectionAfterGndRemoval(this);

}

void  Node::disconnect(Wire* wire)
{
  m_wires.remove(wire);
  // We may have other wires that are still connected to this Node thru other routes
}



/**
 * @brief propagateVisitFlag
 * @param nv
 */
void    Node::propagateVisitFlag(NodeVisit nv)
{
  if (getVisitFlag()==nv) return;
  setVisitFlag(nv);
  for (auto w: m_wires)
    if (w)
      w->propagateVisitFlag(nv);
}

/**
 * @brief Node::propagateNetId
 * @param id
 */
void  Node::propagateNetId(unsigned int id)
{
  propagateNetId(id,true);
}
/**
 * @brief Node::propagateNetId
 * @param id
 * @param init
 */
void    Node::propagateNetId(unsigned int id,bool init)
{
  // If init, we set every cond as NEED_VISIT
  if (init)
    propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED) return;

  setVisitFlag(VISITED);

  setNetID(id);

  for (auto w: m_wires)
    if(w!=nullptr)
      w->propagateNetId(id, false);
}

/**
 * @brief Node::is_connected_physically
 * @param wire
 * @param init
 * @return
 */
bool Node::is_connected_physically(Wire* wire, bool init)
{
  if (init)
    propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED) return false;

  setVisitFlag(VISITED);

  for (auto w: m_wires)
    if (w)
    {
      // Beware: this way we may end up with some of the tree with VISITED (the initial part, til the wire
      // found) and the remaining with "NEED_VISIT".
      if (w == wire)
        return true;

      if (w->is_connected_physically(wire, false))
        return true;
    }
  return false;
}

/**
 * @brief Node::is_connected_physically
 * @param node
 * @param init
 * @return
 */
bool Node::is_connected_physically(Node* node, bool init)
{
  if (init)
    propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED) return false;

  setVisitFlag(VISITED);

  if (this==node) return true;

  for (auto w: m_wires)
    if (w)
    {
      // Beware: this way we may end up with some of the tree with VISITED (the initial part, til the wire
      // found) and the remaining with "NEED_VISIT".

      if (w->is_connected_physically(node, false))
        return true;
    }
  return false;
}
/**
 * @brief Node::is_connected_physically
 * @param wire
 * @return
 */
bool Node::is_connected_physically(Wire* wire)
{
  bool bres = is_connected_physically(wire, true);

  propagateVisitFlag(DEFAULT_STATE);

  return bres;
}
/**
 * @brief Node::is_connected_physically
 * @param node
 * @return
 */
bool Node::is_connected_physically(Node* node)
{
  bool bres = is_connected_physically(node, true);

  propagateVisitFlag(DEFAULT_STATE);

  return bres;

}
/**
 * @brief Node::has_global_label
 * @param init
 * @return
 */
WireLabel*  Node::has_global_label(bool init)
{
  if (init)
    propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED)
    return nullptr;

  setVisitFlag(VISITED);

  if (hasLabel())
    return label();

  for (auto w: m_wires)
    if (w)
    {
      WireLabel* wire_label = w->has_global_label(false);
      if (wire_label!=nullptr) return wire_label;
    }

  return nullptr;
}
/**
 * @brief Node::has_global_label
 * @return
 */
WireLabel* Node::has_global_label()
{
  WireLabel* res = has_global_label(true);

  propagateVisitFlag(DEFAULT_STATE);

  return res;
}

/**
 * @brief Node::getConnectedConductors
 * @param current
 * @param init
 * @return
 */

void Node::getConnectedConductors(QVector<Conductor*>& current, bool init)
{
  if (init)
      propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED)
    return;

  setVisitFlag(VISITED);

  current.emplace_back(this);

  for (auto w : m_wires)
    if (w)
      w->getConnectedConductors(current, false);
}

/**
 * @brief Node::getConnectedConductors
 * @param current
 */
void  Node::getConnectedConductors(QVector<Conductor*>& current)
{
  getConnectedConductors(current, true);
}

/**
 * @brief Node::isGnd
 * @param init
 * @return
 */
bool Node::isGnd(bool init)
{
  if (init)
    propagateVisitFlag(NEED_VISIT);

  if (getVisitFlag()==VISITED)
    return false;

  setVisitFlag(VISITED);

  for (auto c: m_components)
    if (c!=nullptr)
    {
      if (c->Model=="GND")
        return true;
    }

  for (auto w: m_wires)
    if (w!=nullptr)
      if (w->isGnd(false)) return true;

  return false;
}
/**
 * @brief Node::isGnd
 * @return
 */
bool        Node::isGnd()
{
  bool bres = isGnd(true);

  propagateVisitFlag(DEFAULT_STATE);
  return bres;
}

/**
 * @brief Node::dropLabel
 */
void  Node::dropLabel()
{
  Conductor::dropLabel();
  if (getSchematicOwner()!=nullptr) getSchematicOwner()->rebuildConnectionAfterLabelRemoval(this);
}
/**
 * @brief Node::isNodeConnectedToGnd
 * @return
 */
bool  Node::isNodeConnectedToGnd()
{
  for (auto c: m_components)
    if (c!=nullptr)
      if (c->Model=="GND") return true;

  return false;
}

bool  Node::isNodeConnectedToComp()
{
  return !(m_components.empty());
}