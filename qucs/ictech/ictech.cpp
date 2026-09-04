#include "ictech.h"

QSet<tech*>           tech::m_availableTechs = QSet<tech*>();
/*
 * We have a 1:1 correspondance in between files and tech.
 * When creating a tech, it may be an empty file (new tech or void tech)
 * When creating a tech with a filename, if another tech was present, "this" is
 * appended to the list of tech objects with same filename
*/
/**
 * @brief tech::tech
 * @param name
 */
tech::tech(QString filename) :
                               m_isEmpty(true),
                               m_techName("[NONAME]"),
                               m_fileName(filename),
                               m_modelCorners(),
                               m_substrateCorners(),
                               m_Substrates(),
                               m_lastError(),
                               m_laydefs(nullptr),
                               m_layout_tech_file(),
                               m_model_files(),
                               m_substrate_files(),
                               m_symbol_files(),
                               m_subcktSymbols()
{

  // Check if we already have a tech with the same filename.
  // If so copy data from the existing (faster than loading)

  tech* prev = tech::getTechFromFilename(filename);

  if (prev!=nullptr)
  {
    copyFrom(prev);
    return;
  }

  create_klayout_tech();

  if (!load())
    clean();  
}
/**
 * @brief tech::clean. Clean only internal data (retain filename)
 */
void tech::clean()
{
  m_isEmpty = true;
  m_modelCorners.clear();
  m_substrateCorners.clear();
  m_Substrates.clear();

}

/**
 * @brief tech::~tech
 */
tech::~tech()
{
  removeFromProject();
}
/**
 * @brief tech::copyFrom
 * @param t2
 */
void    tech::copyFrom(tech* t2)
{
  if (t2==nullptr) return;
  m_isEmpty               = t2->m_isEmpty;
  m_techName              = t2->m_techName;
  m_fileName              = t2->m_fileName;
  m_modelCorners          = t2->m_modelCorners;
  m_substrateCorners      = t2->m_substrateCorners;
  m_Substrates            = t2->m_Substrates;

  create_klayout_tech();
}

/**
 * @brief tech::appendThisToAvailable
 */
void   tech::makeAvailableForTheProject()
{
  // Add a new holder in the available list
    m_availableTechs.insert(this);

}
/**
 * @brief tech::removeThisFromAvailable
 */
void   tech::removeFromProject()
{
  m_availableTechs.remove(this);

}
/**
 * @brief tech::rename
 * @param newname
 */
void   tech::rename(const QString& newname)
{
  if (newname==m_techName) return;
  m_techName = newname;
}


/**
 * @brief tech::addModelCorner
 * @param corner
 */
void  tech::addModelCorner(const QString& corner)
{

}

/**
 * @brief tech::addSubstrateCorner
 * @param corner
 */
void  tech::addSubstrateCorner(const QString& corner)
{

}
/**
 * @brief tech::removeModelCorner
 * @param corner
 */
void  tech::removeModelCorner(const QString& corner)
{

}
/**
 * @brief tech::removeSubstrateCorner
 * @param corner
 */
void  tech::removeSubstrateCorner(const QString& corner)
{

}
/**
 * @brief tech::getModelCorners
 * @return
 */
QStringList tech::getModelCorners()
{
  return m_modelCorners;
}
/**
 * @brief tech::getSubstrateCorners
 * @return
 */
QStringList tech::getSubstrateCorners()
{
  return m_substrateCorners;
}
/**
 * @brief tech::assignSubstrateCornerToModelCorner
 * @param modelCorner
 * @param subCorner
 * @return
 */
bool  tech::assignSubstrateCornerToModelCorner(QString modelCorner, QString subCorner)
{
  return true;
}
/**
 * @brief tech::hasModelCorner
 * @param modelCorner
 * @return
 */
bool  tech::hasModelCorner(const QString& modelCorner)
{
  return std::find(m_modelCorners.begin(), m_modelCorners.end(), modelCorner)!=m_modelCorners.end();
}
/**
 * @brief hasSubstrateCorner
 * @param substrateCorner
 * @return
 */
bool  tech::hasSubstrateCorner(const QString& substrateCorner)
{
  return std::find(m_substrateCorners.begin(), m_substrateCorners.end(), substrateCorner)!=m_substrateCorners.end();
}

/**
 * @brief tech::loadSubstrateData
 * @param corner
 * @param filename
 * @return
 */
bool  tech::loadSubstrateData(const QString& corner, const QString& filename)
{
  return true;
}
/**
 * @brief tech::save
 * @return
 */
bool  tech::save(bool make_available)
{
  if (make_available)
  {
    tech* prev = tech::getTechFromFilename(m_fileName);
    if ((prev!=nullptr)&&(prev!=this))
    {
      // Remove previous one.
      prev->removeFromProject();
      makeAvailableForTheProject();
    }
  }

  return true;
}

/**
 * @brief tech::saveToFile
 * @param filename
 * @return
 */
bool tech::saveToFile(const QString& filename,bool make_available)
{
  if (filename.isEmpty())
    return false;


  m_fileName = filename;

  return save(make_available);
}
/**
 * @brief tech::load
 * @return
 */
bool  tech::load()
{
  // load technology
  import_klayout_tech_file();

  return true;
}

/**
 * @brief tech::load
 * @param filename
 * @return
 */
bool tech::load(const QString& filename)
{
  m_fileName = filename;
  return load();
}

/**
 * @brief tech::getFilename
 * @return
 */
QString tech::getFilename()
{
  return m_fileName;
}


/**
 * @brief tech::getTechFromName
 * @param techname
 * @return
 */
tech*  tech::getTechFromName(const QString& techname)
{
  for (const auto& avtech : std::as_const(m_availableTechs))
  if (avtech!=nullptr)
  {
    if (avtech->getTechname() == techname)
      return avtech;
  }
  return nullptr;
}
/**
 * @brief tech::getAvailableTechs
 * @return The list of available techs
 */
QStringList tech::getAvailableTechs()
{
  QStringList out;
  for (const auto& avtech : std::as_const(m_availableTechs))
    out.append(avtech==nullptr?"":avtech->getTechname());
  return out;
}

/**
 * @brief tech::getFilenameFromTech
 * @param tech
 * @return
 */
tech* tech::getTechFromFilename(const QString &techfile)
{
  for (const auto& avtech : std::as_const(m_availableTechs))
    if (avtech!=nullptr)
  {
    if (avtech->getFilename() == techfile)
      return avtech;
  }
  return nullptr;
}

/**
 * @brief tech::createDefaultFileNames
 */
void   tech::createDefaultFileNames()
{

}
