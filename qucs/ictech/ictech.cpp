#include "ictech.h"
#include <tinyxml2.h>
#include <QDateTime>
#include <QDir>

QSet<tech*>           tech::m_availableTechs = QSet<tech*>();

using namespace tinyxml2;
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
                               m_ktech(nullptr),
                               m_layoutView(nullptr),
                               m_layout(nullptr),
                               m_layout_tech_file(),
                               m_model_files(),
                               m_substrate_files(),
                               m_symbol_files(),
                               m_subcktSymbols()
{

  // Check if we already have a tech with the same filename.
  // If so copy data from the existing (faster than loading)

  tech* prev = tech::getTechFromFilename(filename);

  create_klayout_tech();

  if (prev!=nullptr)
  {
    copyFrom(prev);
    return;
  }

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
  if (m_layoutView!=nullptr) delete m_layoutView;
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
  m_layout_tech_file      = t2->m_layout_tech_file;
  m_layout_lyp_file       = t2->m_layout_lyp_file;
  m_model_files           = t2->m_model_files;
  m_symbol_files          = t2->m_symbol_files;
  m_subcktSymbols         = t2->m_subcktSymbols;
  create_klayout_tech();
  // Copy all layer props
  m_layoutView->clear_layers();
  for (lay::LayerPropertiesConstIterator it =  t2->m_layoutView->begin_layers();
       it != t2->m_layoutView->end_layers(); ++it)
    m_layoutView->insert_layer(m_layoutView->begin_layers(), *it);
  // dbu / gris
  m_ktech->set_dbu(t2->m_ktech->dbu());
  m_ktech->set_default_grids(t2->m_ktech->default_grids());
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
  if (m_fileName.isEmpty()) return false;
  if (m_techName.isEmpty()) return false;

  if (make_available)
  {
    tech* prev = tech::getTechFromFilename(m_fileName);
    if ((prev!=nullptr)&&(prev!=this))
      prev->removeFromProject();

    makeAvailableForTheProject();
  }

  // Create folders, if missing
  QDir base = getTechnologyBaseFolder();
  if (!base.exists())
    QDir().mkdir(base.absolutePath());

  QString layoutFolder = getLayoutFolder();
  if (!QDir(layoutFolder).exists())
    QDir().mkdir(layoutFolder);

  QString emFolder = getEMFolder();
  if (!QDir(emFolder).exists())
    QDir().mkdir(emFolder);

  QString spiceModelsFolder = getSpiceModelsFolder();
  if (!QDir(spiceModelsFolder).exists())
    QDir().mkdir(spiceModelsFolder);

  QString librariesFolder = getLibrariesFolder();
  if (!QDir(librariesFolder).exists())
    QDir().mkdir(librariesFolder);

  QString stdcellsFolder = getStdCellsFolder();
  if (!QDir(stdcellsFolder).exists())
    QDir().mkdir(stdcellsFolder);


  // Proceed with saving

  XMLDocument xmlTechDoc;

  XMLElement * pTechRoot = xmlTechDoc.NewElement("technology");
  pTechRoot->SetAttribute("name", m_techName.toLocal8Bit().data());
  pTechRoot->SetAttribute("save_time",QDateTime::currentDateTime().toString().toLocal8Bit().data());

  assert(pTechRoot!=nullptr);
  // Properties
  XMLElement* pTechDescription = xmlTechDoc.NewElement("properties");
  pTechRoot->InsertFirstChild(pTechDescription);

  // Layout
  XMLElement* pLayoutDescription = xmlTechDoc.NewElement("layout");
  pTechDescription->InsertEndChild(pLayoutDescription);

  // Spice models
  XMLElement* pModelDescription = xmlTechDoc.NewElement("models");
  pTechDescription->InsertEndChild(pModelDescription);

  // Libraries
  XMLElement* pLibrariesDescription = xmlTechDoc.NewElement("libraries");
  pTechDescription->InsertEndChild(pLibrariesDescription);

  //
  XMLElement* pStdCells = xmlTechDoc.NewElement("stdcells");
  pTechRoot->InsertEndChild(pStdCells);

  xmlTechDoc.InsertFirstChild(pTechRoot);

  // Save all relevant data in the folder
  saveLayoutData(pLayoutDescription , &xmlTechDoc);


  XMLError eResult = xmlTechDoc.SaveFile(m_fileName.toLocal8Bit().data());



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

/**
 * @brief tech::getTechnologyBaseFolder
 * @return
 */

QDir    tech::getTechnologyBaseFolder()
{
  if (m_fileName.isEmpty()) return QDir();

  QDir    baseDir         = QFileInfo(m_fileName).dir();
  QString baseName        = QFileInfo(m_fileName).baseName()+QDir::separator();
  QDir    result(baseDir.absoluteFilePath(baseName));

  return result;//QDir(result.canonicalPath());
}

/**
 * @brief tech::setDescription
 * @param descr
 */
void    tech::setDescription(const QString& descr)
{
  m_ktech->set_description(descr.toStdString());
}

/**
 * @brief tech::getDescription
 * @return
 */
QString tech::getDescription()
{
  return m_ktech->description();
}
