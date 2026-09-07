#include "ictech.h"
#include <QFileInfo>
#include <QDir>
#include <tinyxml2.h>

using namespace tinyxml2;
/**
 * All procedures related to the klayout technologies inside ictech are here
 * */

/**
 * @brief tech::create_klayout_tech
 */

const QString relative_layout_folder="./layout/";
/**
 * @brief tech::getLayoutFolder
 * @return
 */
QString   tech::getLayoutFolder()
{
  if (m_fileName.isEmpty()) return QString("");

  QDir    baseDir         = getTechnologyBaseFolder();
  QDir    layoutDir(baseDir.absoluteFilePath(relative_models_folder));
  return layoutDir.canonicalPath();

}
/**
 * @brief tech::getLayoutFilepath
 * @return A default name for the lyt file in the technology
 */
QString   tech::getLayoutFilepath()
{
  QString folderPath = getLayoutFolder();

  QString fileName = m_fileName;

  if (fileName.isEmpty()) return QString("");
  // Change the extension to lyt
  QFileInfo fi(fileName);
  fileName = fi.baseName()+".lyt";

  // Combine them safely
  return  QDir(folderPath).filePath(fileName);
}

/**
 * @brief tech::create_klayout_tech: used when creating an empty tech or when loading one
 */
void  tech::create_klayout_tech()
{
  remove_klayout_tech();

  // Create an empty klayout technology
  db::Technology laytech;
  laytech.set_name(getTechname().toStdString());
  m_laydefs = db::Technologies::instance()->add(laytech);
  assert(m_laydefs!=nullptr);

  // Set the base path as
  QString layout_folder;

  if (m_fileName.isEmpty())
    layout_folder = "";
  else
    layout_folder = getLayoutFolder();

  m_laydefs->set_explicit_base_path(layout_folder.toStdString());

  // Set the layout tech file (lyt)
  // If not file given, assign a default name
  if (m_layout_tech_file.isEmpty())
    m_layout_tech_file = getLayoutFilepath();
}

/**
 * @brief tech::remove_klayout_tech
 */
void    tech::remove_klayout_tech()
{

  if (m_laydefs==nullptr) return;
  db::Technologies::instance()->remove(getTechname().toStdString());
  //db::Technologies::unregister_ptr(m_laydefs->);
  delete m_laydefs;
  m_laydefs = nullptr;
}
/**
 * @brief tech::import_klayout_tech_file
 * @return
 */

bool    tech::import_klayout_tech_file()
{
  if ((m_laydefs==nullptr)||(m_layout_tech_file.isEmpty()))
    return true;
  m_laydefs->load(m_layout_tech_file.toStdString());
  m_layout_lyp_file =  QString::fromStdString(m_laydefs->layer_properties_file());

  import_klayout_layerdefs();
  return true;

}
/**
 * @brief tech::import_klayout_layerdefs
 * @return
 */
bool    tech::import_klayout_layerdefs(const QString& newLypFile)
{
  if (newLypFile!=m_layout_lyp_file)
    m_layout_lyp_file = newLypFile;

  if (m_layout_lyp_file.isEmpty()) return true;
  if (m_laydefs==nullptr) return true;
  m_laydefs->set_layer_properties_file(m_layout_lyp_file.toStdString());

  // Clear prev layers
  m_layoutView->clear_layers();
  m_layoutView->load_layer_props(newlypfile.toStdString());

  return true;
}

/**
 * @brief tech::saveLayoutData
 * @param rootLayout
 */

void    tech::saveLayoutData(XMLNode* rootLayout, XMLDocument* doc)
{
  QDir    layoutBasePath(getLayoutFilepath());

  QString defaultLytFile = getTechname()+".lyt";
  QString defaultLypFile = getTechname()+".lyp";

  QString fullLytFile = QFileInfo(layoutBasePath,defaultLytFile).absoluteFilePath();
  QString fullLypFile = QFileInfo(layoutBasePath,defaultLypFile).absoluteFilePath();

  m_laydefs->set_name(getTechname().toStdString());

  m_laydefs->set_layer_properties_file(fullLytFile.toStdString());

  m_laydefs->save(fullLypFile.toStdString());

  if (doc==nullptr) return;
  if (rootLayout==nullptr) return;

  XMLElement* layoutFiles = doc->NewElement("layout_files");
  assert(layoutFiles!=nullptr);
  layoutFiles->SetAttribute("layout_tech_file", fullLytFile);
  layoutFiles->SetAttribute("layout_properties_file",fullLypFile);

  rootLayout->InsertEndChild(layoutFiles);
}
