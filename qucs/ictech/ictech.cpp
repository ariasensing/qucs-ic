#include "ictech.h"

QHash<QString, tech*>           tech::m_availableTechs = QHash<QString,tech*>();
QHash<QString, QString>         tech::m_mapNameToFiles = QHash<QString,QString>();
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
                               m_fileName(),
                               m_modelCorners(),
                               m_substrateCorners(),
                               m_Substrates(),
                               m_lastError()
{
  if (filename.isEmpty()) return;
  // Check if we already have a tech with the same filename. If so copy data from the existing
  QHash<QString,tech*>::iterator it = m_availableTechs.find(filename);
  if (it!=m_availableTechs.end())
  {
    copyFrom(*it);
    return;
  }

  m_fileName = filename;
  // Should we keep a copy? For now, just clean
  if (!load())
    clean();

  appendThisToAvailable();
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
  removeThisFromAvailable();
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
}

/**
 * @brief tech::appendThisToAvailable
 */
void   tech::appendThisToAvailable()
{
  // Add a new holder in the available list
  m_mapNameToFiles[m_techName] = m_fileName;
  m_availableTechs[m_fileName] = this;

}
/**
 * @brief tech::removeThisFromAvailable
 */
void   tech::removeThisFromAvailable()
{
  m_mapNameToFiles.remove(m_techName);
  m_availableTechs.remove(m_fileName);

}
/**
 * @brief tech::rename
 * @param newname
 */
void   tech::rename(const QString& newname)
{
  if (newname==m_techName) return;
  m_mapNameToFiles[newname] = m_mapNameToFiles[m_techName];
  m_mapNameToFiles.remove(m_techName);
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
bool  tech::save()
{
  appendThisToAvailable();
  return true;
}

/**
 * @brief tech::saveToFile
 * @param filename
 * @return
 */
bool tech::saveToFile(const QString& filename)
{
  if (filename.isEmpty())
    return false;

  if (filename==m_fileName)
    return save();
  // We cannot save to a filename already associated with another tech
  removeThisFromAvailable();
  m_fileName = filename;
  return save();
}
/**
 * @brief tech::load
 * @return
 */
bool  tech::load()
{
  appendThisToAvailable();
  return true;
}

/**
 * @brief tech::load
 * @param filename
 * @return
 */
bool tech::load(const QString& filename)
{

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
 * @brief tech::getTechFromFilename
 * @param filename
 * @return The set of tech object attached to
 */
tech*  tech::getTechFromFilename(const QString& filename)
{
  QHash<QString, tech*>::iterator it = m_availableTechs.find(filename);
  if (it == m_availableTechs.end()) return nullptr;
  return (*it);

}
/**
 * @brief tech::getTechFromName
 * @param techname
 * @return
 */
tech*  tech::getTechFromName(const QString& techname)
{
  QHash<QString, QString>::iterator file_it = m_mapNameToFiles.find(techname);
  if (file_it==m_mapNameToFiles.end()) return nullptr;
  QHash<QString, tech*>::iterator it = m_availableTechs.find(*file_it);
  if (it == m_availableTechs.end()) return nullptr;
  return (*it);

}
/**
 * @brief tech::getAvailableTechs
 * @return The list of available techs
 */
QStringList tech::getAvailableTechs()
{
  QStringList out;
  QHash<QString, QString>::iterator file_it;
  for (file_it = m_mapNameToFiles.begin(); file_it != m_mapNameToFiles.end(); file_it++)
      out.append(file_it.key());
  return out;
}

/**
 * @brief tech::getFilenameFromTech
 * @param tech
 * @return
 */
QString tech::getFilenameFromTech(const QString &tech)
{
  QHash<QString, QString>::iterator file_it = m_mapNameToFiles.find(tech);
  if (file_it==m_mapNameToFiles.end()) return QString("");

  return *file_it;
}

/**
 * @brief tech::getTechFromFilename
 * @param filname
 * @return
 */
QString tech::getTechnameFromFilename(const QString &filename)
{
  QHash<QString, QString>::iterator file_it;
  for (file_it = m_mapNameToFiles.begin(); file_it != m_mapNameToFiles.end(); file_it++)
    if ((file_it.value())==filename) return file_it.key();

  return "";
}


