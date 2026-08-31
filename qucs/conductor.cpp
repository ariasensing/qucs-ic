#include "conductor.h"
#include "schematic.h"

/**
 * @brief Conductor::~Conductor. This removes the conductor from the connection set.
 * NB Connectivity must be updated before hand.
 */
Conductor::~Conductor()
{
  if (m_owner!=nullptr) m_owner->removeConductor(this);
}
/**
 * @brief Conductor::setSchematicOwner
 * @param schem. If null, this conductor is detached from previous conductor. Note: the conductor may
 * still be present in the previous Schematic db so it should be removed.
 */
void  Conductor::setSchematicOwner(Schematic* const schem)
{
  Schematic* prev_owner = m_owner;

  m_owner = nullptr; // This way we may call removeConductor without looping
  if (prev_owner!=nullptr)
    prev_owner->removeConductor(this);

  m_owner = schem;
  if (m_owner)
    m_owner->addConductor(this);
}


