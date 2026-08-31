/**
 * ARIA Sensing srl
 * Handling of connection and nodes.
 * This is done
 **/
#include <QMap>
#include <QVector>
#include "schematic.h"
#include "settings.h"
#include "textdoc.h"

#include "misc.h"
#include "iclayout.h"
#include "wire.h"
#include "node.h"
/**
 * @brief Schematic::removeConductor. Remove a conductor from the connection set. This function does not
 * check for actual connections. It simply take care of the connection set and the maxId
 * @param cond
 */
void                      Schematic::removeConductor(Conductor* cond)
{

  if (a_symbolMode) return;
  if (cond == nullptr) return;

  unsigned int id = cond->getNetID();

  if (id==0) return;

  connected_iterator it = m_connection_set.find(id);
  if (it==m_connection_set.end()) return;
  it->removeAll(cond);
  if (it->isEmpty())
    m_connection_set.remove(id);

  // The conductor is still living in memory space. Let's detach it from the m_connection set
  cond->setNetID(0);

}
/**
 * @brief Schematic::addConductor Add a conductor to the connection set. This function does not
 * check for actual connections. If conductor id is 0, a new id is assigned
 * @param cond
 */
void                      Schematic::addConductor(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond == nullptr) return;
  unsigned int id = cond->getNetID();

  if (id==0)
    return;
  /*
  {
    id = get_available_id();
    cond->setNetID(id);
    setNameId(id, cond->hasLabel()? cond->label()->Name : "");
  }*/

  QVector<Conductor*>& current_set = m_connection_set[id];
  if (std::find( current_set.begin() , current_set.end(), cond)!=current_set.end()) return;
  current_set.emplace_back(cond);
}

/**
 * @brief Schematic::addConductorWithConnectionCheck. Add a conductor to the connection set. This function
 * check current connections and update the connectionSet accordingly
 * @param cond
 */
void                      Schematic::addConductorWithConnectionCheck(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond==nullptr) return;

  QVector<Conductor*> current_set;

  cond->getConnectedConductors(current_set);

  unsigned int target_id = 0;
  // If the given conductor does not have a valid id,
  // let's search in the connection
  if (cond->getNetID()==0)
  {
    for(const auto& c:std::as_const(current_set))
      if (c!=nullptr)
      {
        unsigned int connected_id = c->getNetID();
        if (connected_id > 0)
        { target_id = connected_id; break;}
      }
  }
  else
        target_id = cond->getNetID();

  // If overall we still don't have a valid id, let's assign a new one
  if (target_id == 0)
  {
    target_id = get_available_id();
    setNameId(target_id, cond->hasLabel()? cond->label()->Name : "");
  }

  // We have found a suitable index: let's assign it to all the elements of the set
  for(const auto& c:std::as_const(current_set))
  {
    // Update also the id, in case we have a mismatch
    if (c==nullptr) continue;

    unsigned int current_id = c->getNetID();
    if (current_id == target_id) continue;

    // Remove the one from the previous one
    if (current_id)
        removeConductor(c);

    // Set the new id and update
    c->setNetID(target_id);

    addConductor(c);
  }
}

/**
 * @brief Schematic::searchConnectionSets Given a list of conductors, it splits it into a list
 * of connected ones. Note that the output set may contain MORE conductors since all connected
 * are inserted.
 */
QList<QVector<Conductor*>>  Schematic::splitSetAccordingToConnections(QVector<Conductor*> cond_set)
{

  QList<QVector<Conductor*>>  out_list;
  if (a_symbolMode) return out_list;
  while (!cond_set.isEmpty())
  {
    Conductor* test = cond_set.last();
    cond_set.removeLast();

    if (test==nullptr) continue;

    QVector<Conductor*> current_set;
    test->getConnectedConductors(current_set);

    // Put the current set into the output
    assert(!current_set.isEmpty());
    out_list.emplace_back(current_set);
    // Remove the current set from the inquiry elements
    for(const auto& c:std::as_const(current_set))
      if (c)
        cond_set.removeAll(c);
  }
  return out_list;
}
/**
 * @brief Schematic::rebuildConnectionAfterDelete
 * @param cond Is the conductor that has been deleted
 */

void                      Schematic::rebuildConnectionAfterDelete(Conductor* cond)
{
  if (a_symbolMode) return;
  // Cond is the conductor we have delete. Let's find previous connection
  if (cond == nullptr) return;
  unsigned int cond_id = cond->getNetID();

  if (cond_id==0) return;
  removeConductor(cond);
  QVector<Conductor*> connections = m_connection_set[cond_id];
  if (m_connection_set[cond_id].isEmpty()) m_connection_set.remove(cond_id);

  if (connections.isEmpty()) return;

  // Check for previous connection. Deleting a conductor in between may result in two different subsets.
  // In case, we need to assign a new set (unless they have same label!)
  QList<QVector<Conductor*>> connections_sets = splitSetAccordingToConnections(connections);
  assert(connections_sets.size() > 0 );

  if (connections_sets.size()<2)
    return;

  // Since we are deleting just ONE conductor, we cannot have more than two sets
  //assert(connections_sets.size() == 2 );

  QVector<Conductor*> & set1 = connections_sets[0];
  QVector<Conductor*> & set2 = connections_sets[1];

  if ((set1.isEmpty())||(set2.isEmpty())) return;

  Conductor* cond1 = set1[0];
  Conductor* cond2 = set2[0];
  WireLabel* l1 = cond1->has_global_label();
  WireLabel* l2 = cond2->has_global_label();
  if ((l1!=nullptr)&&(l2!=nullptr)) return;
  bool cond1_is_gnd = cond1->isGnd();
  bool cond2_is_gnd = cond2->isGnd();

  if (cond1_is_gnd && cond2_is_gnd) return;

  if ((l1!=nullptr)||(cond1_is_gnd))
  {
    // Modify only cond2_set
    unsigned int new_id = get_available_id();
    setNameId(new_id, "");
    // Remove all conductors of set2
    for(const auto& c:std::as_const(set2))
      if (c!=nullptr)
        removeConductor(c);

     for(const auto& c:std::as_const(set2))
      if (c!=nullptr)
        c->setNetID(new_id);

    m_connection_set[new_id] = set2;

    return;
  }

  // Modify only cond1_set
  unsigned int new_id = get_available_id();
  setNameId(new_id, "");
  // Remove all conductors of set2
  for(const auto& c:std::as_const(set1))
    if (c!=nullptr)
      removeConductor(c);

   for(const auto& c:std::as_const(set1))
    if (c!=nullptr)
      c->setNetID(new_id);

  m_connection_set[new_id] = set1;
  return;
}

void                      Schematic::rebuildConnectionAfterInsertion(Conductor* cond)
{
  if (a_symbolMode) return;
  // When inserting a new conductor, we may join two different subsets.
  // 1. Retrieve the list of connected conductors. If they have different IDs, the two subsets must be joined
  if (cond == nullptr) return;


  //if (cond_id==0) return;

  QVector<Conductor*> current_set;
  cond->getConnectedConductors(current_set);
  QSet<unsigned int> id_sets;

  QString label;
  bool    b_label_given = false;
   for(const auto& c:std::as_const(current_set))
    if (c!=nullptr)
    {
      id_sets.insert(c->getNetID());
      if ((c->isNodeConnectedToGnd())&&(!b_label_given)) {label = "GND"; b_label_given = true;}
      if ((c->hasLabel())&&(!b_label_given)) {label = c->label()->Name; b_label_given = true;}
    }
  // Find the first non-zero id
  unsigned int target_id = 0;
  for (auto id : id_sets)
    if (id > 0)
    {
      target_id = id; break;
    }

  if (target_id==0)
  {
    target_id = get_available_id();
    if (!b_label_given) setNameId(target_id);
  }

  // Join all previous
   for(const auto& c:std::as_const(current_set))
    if (c!=nullptr)
    {
      if (c->getNetID() == target_id) continue;

      removeConductor(c);

      c->setNetID(target_id);

      addConductor(c);
    }
}


/**
 * @brief Schematic::rebuildConnectionAfterSplitting
 * @param cond is the new wire which splits a previous one
 */
void  Schematic::rebuildConnectionAfterSplitting(Conductor* cond)
{
  if (a_symbolMode) return;
  // We just need to normalize the id and the connection set
  if (cond==nullptr) return;

  QVector<Conductor*> cond_connected;
  cond->getConnectedConductors(cond_connected);

  unsigned int target_id = 0;
   for(const auto& c:std::as_const(cond_connected))
  {

    if (c==nullptr) continue;
    unsigned int current_id = c->getNetID();

    if (current_id > 0)
    {
      target_id = current_id;
      break;
    }
  }

  // Assign a new one if we don't have yet a valid id
  if (target_id==0)
  {
    target_id = get_available_id();
    setNameId(target_id);
  }

   for(const auto& c:std::as_const(cond_connected))
  {
    if (c== nullptr) continue;

    if (c->getNetID()==target_id) continue;

    removeConductor(c);

    c->setNetID(target_id);

    addConductor(c);
  }


}

/**
 * @brief Schematic::rebuildConnectionAfterLabelInsertion
 * @param cond
 */
void  Schematic::rebuildConnectionAfterLabelInsertion(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond==nullptr) return;
  if (!cond->hasLabel()) return;
  QString label_cond = cond->label()->Name;
  // We need to look for any other conductor which share the same label text

  unsigned int target_id = 0;
  unsigned int cond_id = cond->getNetID();

  // Search for other wires with the same label
  for (std::list<Wire*>::iterator wit = a_DocWires.begin(); wit != a_DocWires.end(); wit++)
  {
    Wire* w = *wit;
    if (w==nullptr) continue;
    if (!w->hasLabel()) continue;
    if (w==cond) continue;
    QString other_label = w->label()->Name;

    if (other_label!=label_cond) continue;

    unsigned int wid = w->getNetID();
    // They are already in the same set. Check: we assume that no wires are created AFTER the label
    if (wid == cond_id)
      return;

    target_id = wid;
    // Here we have found the other set with the same label
    break;
  }
  // There's no other conductor with the same label
  if (target_id == 0) return;

  // Get all the conductors connected to cond
  QVector<Conductor*> set = m_connection_set[cond_id];
  for (auto c : set)
    if (c!=nullptr)
      c->setNetID(target_id);

  m_connection_set.remove(cond_id);
  m_connection_set[target_id].append(set);
}

/**
 * @brief rebuildConnectionAfterLabelRemoval
 */
void  Schematic::rebuildConnectionAfterLabelRemoval(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond==nullptr) return;
  if (cond->hasLabel()) return;
  // We removed the label from cond. We need to detach only the set of conductors physically attached to cond
  unsigned int current_id = cond->getNetID();

  // get the current set
  QVector<Conductor*> current_set = m_connection_set[current_id];

  QVector<Conductor*> cond_connected;
  cond->getConnectedConductors(cond_connected);

  if (cond_connected.size()==current_set.size()) return; // There's no left conductor, so don't need to assign a new id
  unsigned int target_id = get_available_id();

  setNameId(target_id);

   for(const auto& c:std::as_const(cond_connected))
  {
    if (c!=nullptr)
      removeConductor(c);
    c->setNetID(target_id);
  }

  m_connection_set[target_id] = cond_connected;
}

/**
 * @brief Schematic::get_available_id
 * @return
 */

unsigned int  Schematic::get_available_id()
{
  if (a_symbolMode) return 0;
  unsigned int  id = 0;
  unsigned int  nmax = m_connection_set.size();
  for (unsigned int n=1; n<= nmax; n++)
  {
    if (m_connection_set.find(n)==m_connection_set.end())
    {
      id = n;
      break;
    }
  }

  if (id == 0) id = nmax+1;

  return id;
}

/**
 * @brief Schematic::rebuildConnectionAfterGndInsertion
 */
void    Schematic::rebuildConnectionAfterGndInsertion(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond==nullptr) return;
  // We have inserted GND to a certain node.
  // Loop thru each set to verify if it is GND.
  // If so, we need to join the different sets

  // Cond and its connection tree may not be joined to previous ground.
  // We need to look for another ground
  for (auto node : a_DocNodes)
  {
    if (node==nullptr) continue;
    if (!node->isNodeConnectedToGnd()) continue;
    if (node->getNetID()!=cond->getNetID())
    {
      unsigned int prev_gnd_id = node->getNetID();
      QVector<Conductor*> connected_to_new_gnd;
      cond->getConnectedConductors(connected_to_new_gnd);
      // Set the same id as previous gnd
      for(const auto& c:std::as_const(connected_to_new_gnd))
      {
        if (c->getNetID()!=prev_gnd_id)
        {
          removeConductor(c);
          c->setNetID(prev_gnd_id);
        }
      }

      m_connection_set[prev_gnd_id].append(connected_to_new_gnd);
    }
  }
}
/**
 * @brief Schematic::rebuildConnectionAfterGndLabelRemoval
 */
void  Schematic::rebuildConnectionAfterGndRemoval(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond==nullptr) return;
  // Differently from labels, we may have two gnd components attached. In this case, skip
  if (cond->isGnd()) return;
  // to all practical purposes, it's equivalent to "removeLabel"
  rebuildConnectionAfterLabelRemoval(cond);

}
/**
 * @brief Schematic::rebuildAll
 */
void  Schematic::rebuildAll()
{
  if (a_symbolMode) return;
  m_connection_set.clear();
  QVector<Conductor*> wires;
  for (auto w: a_DocWires) wires.emplace_back(w);
  for (auto n: a_DocNodes) wires.emplace_back(n);
  QVector<Conductor*> gnd_wires;
  QHash<QString, QVector<Conductor*>> labeled_wires;
  // First: sort wires according to their set
  for (auto w : wires)
  {
    if (w==nullptr) continue;
    if (w->isNodeConnectedToGnd()) {gnd_wires.emplace_back(w); continue;}
    // Relying on local label should be enough
    WireLabel* label = w->hasLabel() ? w->label() : nullptr;
    if (label==nullptr) continue;
    if (label->Name.isEmpty()) continue;

    labeled_wires[label->Name].emplace_back(w);
  }

  // Ground nodes
  QVector<Conductor*> connected_gnd;
  while (!gnd_wires.isEmpty())
  {
    Conductor* w = gnd_wires.last();

    gnd_wires.removeLast();

    if (w==nullptr) continue;

    w->getConnectedConductors(connected_gnd);

    for(const auto& wtodel:std::as_const(connected_gnd))
    {
      wtodel->setNetID(1);
      wires.removeAll(wtodel);
      gnd_wires.removeAll(wtodel);
    }
  }
  // Store the ground set
  if (!connected_gnd.isEmpty())
    m_connection_set[1] = connected_gnd;

  // Labeled nodes
  unsigned int setid = 2;
  for (auto set = labeled_wires.cbegin(); set != labeled_wires.cend();  set++)
  {
    QVector<Conductor*> current_label_set = set.value();
    for (Conductor* w : current_label_set)
    {
      if (w==nullptr) continue;
      QVector<Conductor*> connected_to_labeled_wire;
      w->getConnectedConductors(connected_to_labeled_wire);
      for (const auto& connw : std::as_const(connected_to_labeled_wire))
      {
        if (connw==nullptr) continue;

        connw->setNetID(setid);

        wires.removeAll(connw);
      }

      m_connection_set[setid].append(connected_to_labeled_wire);
    }

    setid++;
  }


  // Remaining wires (they are not labeled and not connected to ground so each group is independent
  while (!wires.isEmpty())
  {
    Conductor* w = wires.last();
    wires.removeLast();

    if (w==nullptr) continue;
    QVector<Conductor*> connected;

    w->getConnectedConductors(connected);
    // Remove all wires already found
     for(const auto& wtodel:std::as_const(connected))
     {
      wtodel->setNetID(setid);
      wires.removeAll(wtodel);
     }

    m_connection_set[setid] = connected;



    setid++;
  }
}
/**
 * @brief Schematic::setNameId Associate an id to a string name. If empty, an automatic name is given
 * @param id
 * @param name
 */
void Schematic::setNameId(unsigned int id, QString name)
{
  if (name.isEmpty())
    name = QString("__NET")+QString::number(id);
  m_mapped_names[name] = id;
  m_mapped_ids[id] = name;
}
/**
 * @brief Schematic::rebuildConnectionAfterLabelEdit
 * @param cond MUST be the owner the label
 */
void  Schematic::rebuildConnectionAfterLabelEdit(Conductor* cond)
{
  if (a_symbolMode) return;
  if (cond == nullptr)  return;
  if (cond->getNetID()==0) return;
  if (!cond->hasLabel()) return;
  QString new_name = cond->label()->Name;
  // The following things can happen
  // 1. We are detaching the input cond with a new label
  // 2. We are detaching the input cond from the previous set and attaching to an existing one
  QVector<Conductor*> connection_set;
  cond->getConnectedConductors(connection_set);

  assert(m_connection_set.find(cond->getNetID())!=m_connection_set.end());

  // From prev_set, we need to remove all conductors
  for(const auto& wtodel:std::as_const(connection_set))
    removeConductor(wtodel);

  // Loop thru existing wires to
  unsigned int other_id = 0;
  for (auto w:a_DocWires)
    if ((w!=cond)&&(w!=nullptr)&&(w->hasLabel()))
      if (w->label()->Name == new_name)
      {
        other_id = w->getNetID();
        break;
      }

  if (other_id==0)
  {
    // Create a new set
    other_id = get_available_id();
    for (const auto&c : std::as_const(connection_set))
      if (c!=nullptr) c->setNetID(other_id);

    m_connection_set[other_id] = connection_set;
    return;
  }

  // We must join the current set to the previous one
  for (const auto& c : std::as_const(connection_set))
    if (c!=nullptr)
      c->setNetID(other_id);
  m_connection_set[other_id].append(connection_set);

}