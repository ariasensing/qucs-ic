
#ifndef CONDUCTOR_H
#define CONDUCTOR_H

#include "element.h"
#include "wirelabel.h"
#include <QMap>
#include <QVector>
/** \class Conductor
  * \brief label for Node and Wire classes
  *
  */

enum NodeVisit{NEED_VISIT, VISITED, DEFAULT_STATE};

class Conductor : public Element {


  std::unique_ptr<WireLabel> m_label;

public:
  Conductor() : m_owner(nullptr), m_need_visit{DEFAULT_STATE}, m_id(0) {}
  ~Conductor();

  bool hasLabel() const
  {
    return m_label != nullptr;
  }

  virtual void dropLabel()
  {    
    m_label.reset();
  }

  std::unique_ptr<WireLabel> releaseLabel()
  {
    if (hasLabel()) {
      m_label->setOwner(nullptr);
      return std::move(m_label);
    }
    return nullptr;
  }

  void acquireLabel(std::unique_ptr<WireLabel>&& new_label)
  {
    if (new_label != nullptr) {
      assert(new_label->owner() == nullptr);
      new_label->setOwner(this);
    }
    m_label = std::move(new_label);

  }

  void acquireLabel(WireLabel* new_label)
  {
    if (new_label != nullptr) {
      assert(new_label->owner() == nullptr);
      new_label->setOwner(this);
    }

    m_label.reset(new_label);
  }

  WireLabel* label() const
  {
    return m_label.get();
  }


private:
  class Schematic*            m_owner;

  NodeVisit                   m_need_visit;

  unsigned int                m_id;

  /*
   *  Automatic net & connection handling.
   * */

public:
  /**
   * @brief is_connected_globally Check if wire is connected
   * @param wire
   * @param init
   * @return
   */
  virtual bool        is_connected_physically(class Wire* , bool ) {return false;}
  /**
   * @brief is_connected_physically
   * @return
   */
  virtual bool        is_connected_physically(Node* , bool ) {return false;}
  /**
   * @brief has_global_label
   * @return
   */
  virtual WireLabel*  has_global_label(bool )                {return nullptr;}
  /**
   * @brief propagateNetId : propagate the net id to all physically connected conductors
   * @param id
   */

  virtual void    propagateNetId(unsigned int , bool) {}
  /**
   * @brief setVisitFlag
   * @param nv
   */

  virtual void getConnectedConductors(QVector<Conductor*>& , bool ) { }

  /**
   * @brief setSchematicOwner
   * @param schem. If null, this conductor is detached from previous conductor. Note: the conductor may
   * still be present in the previous Schematic db so it should be removed.
   */
  void          setSchematicOwner(class Schematic* const schem=nullptr);
  /**
   * @brief getSchematicOwner
   * @return  Current owner
   */
  class Schematic* getSchematicOwner() {return m_owner;}
  /**
   * @brief isGnd
   * @return True if this node is connected to ground OR if any its connected conds is ground
   */
  virtual bool          isGnd(bool) {return false;}
  /**
   * @brief isGnd
   * @return True if this node is connected to ground OR if any its connected conds is ground
   */
  virtual bool          isGnd() {return false;}
  /**
   * @brief getConnectedConductors. Return the list of connected conductors, as stored by the owner.
   * @return
   */
  virtual void getConnectedConductors(QVector<Conductor*>& ) {}
  /**
   * @brief setVisitFlag Update the "visit" flag
   * @param nv
   */
  void              setVisitFlag(NodeVisit nv)  {m_need_visit = nv;}
  /**
   * @brief getVisitFlag
   * @return
   */
  NodeVisit         getVisitFlag()              {return m_need_visit;}
  /**
   * @brief has_global_label
   * @return
   */
  virtual WireLabel*  has_global_label()                {return nullptr;}
  /**
   * @brief is_connected_physically
   * @param wire
   * @return
   */
  virtual bool is_connected_physically(class Wire* ) {return false;}
  /**
   * @brief is_connected_physically
   * @param node
   * @return
   */
  virtual bool is_connected_physically(Node* ) {return false;}
  /**
   * @brief propagateNetId
   */
  virtual void propagateNetId(unsigned int )  {}
  /**
   * @brief propagateVisitFlag. Propagate the visit flag to this conductor
   * @param nv
   */

  virtual void    propagateVisitFlag(NodeVisit ) {}

  /**
   * @brief setNetID
   * @param id
   */
  void        setNetID(unsigned int id) {m_id = id;}
  /**
   * @brief getNetID
   */

  unsigned int        getNetID() const {return m_id;}
  /**
   * @brief isNodeConnectedToGnd
   * @return
   */

  virtual bool        isNodeConnectedToGnd() {return false;}

  virtual bool        isNodeConnectedToComp() {return false;}


};


#endif
