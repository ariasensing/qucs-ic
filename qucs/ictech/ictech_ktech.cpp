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
  QDir    layoutDir(baseDir.absoluteFilePath(relative_layout_folder));
  return layoutDir.absolutePath();

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
  m_ktech = db::Technologies::instance()->add(laytech);
  assert(m_ktech!=nullptr);

  // Set the base path as
  QString layout_folder;

  if (m_fileName.isEmpty())
    layout_folder = "";
  else
    layout_folder = getLayoutFolder();

  m_ktech->set_explicit_base_path(layout_folder.toStdString());

  // Set the layout tech file (lyt)
  // If not file given, assign a default name
  if (m_layout_tech_file.isEmpty())
    m_layout_tech_file = getLayoutFilepath();

  // Create dummy layout view
  m_layoutView = new lay::LayoutView(nullptr, true, nullptr,
                                     lay::LayoutViewBase::LV_NoHierarchyPanel +
                                         lay::LayoutViewBase::LV_NoEditorOptionsPanel +
                                         lay::LayoutViewBase::LV_NoBookmarksView +
                                         lay::LayoutViewBase::LV_NoZoom +
                                         lay::LayoutViewBase::LV_NoGrid +
                                         lay::LayoutViewBase::LV_NoPropertiesPopup +
                                         lay::LayoutViewBase::LV_NoServices);

  assert(m_layoutView!=nullptr);
  m_layoutView->create_layout(m_techName.toStdString(),true,true);
  m_layout =  &(m_layoutView->cellview(0)->layout());

  assert(m_layout!=nullptr);


}

/**
 * @brief tech::remove_klayout_tech
 */
void    tech::remove_klayout_tech()
{

  if (m_ktech==nullptr) return;
  db::Technologies::instance()->remove(getTechname().toStdString());
  //db::Technologies::unregister_ptr(m_ktech->);
  delete m_ktech;
  m_ktech = nullptr;
}
/**
 * @brief tech::import_klayout_tech_file
 * @return
 */

bool    tech::import_klayout_tech_file()
{
  if ((m_ktech==nullptr)||(m_layout_tech_file.isEmpty()))
    return true;
  m_ktech->load(m_layout_tech_file.toStdString());
  m_layout_lyp_file =  QString::fromStdString(m_ktech->layer_properties_file());

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
  if (m_ktech==nullptr) return true;
  m_ktech->set_layer_properties_file(m_layout_lyp_file.toStdString());

  return true;
}

/**
 * @brief tech::saveLayoutData
 * @param rootLayout
 */

void    tech::saveLayoutData(XMLElement* rootLayout, XMLDocument* )
{
  QDir    layoutBasePath(getLayoutFolder());

  QString defaultLytFile = QFileInfo(m_fileName).baseName()+".lyt";
  QString defaultLypFile = QFileInfo(m_fileName).baseName()+".lyp";

  QString fullLytFile = QFileInfo(layoutBasePath,defaultLytFile).absoluteFilePath();
  QString fullLypFile = QFileInfo(layoutBasePath,defaultLypFile).absoluteFilePath();

  m_ktech->set_name(getTechname().toStdString());
  m_ktech->set_default_base_path(layoutBasePath);

  m_ktech->set_layer_properties_file(fullLypFile.toStdString());

  m_layoutView->save_layer_props(fullLypFile.toStdString());

  m_ktech->save(fullLytFile.toStdString());

  if (rootLayout==nullptr) return;

  rootLayout->SetAttribute("layout_tech_file", fullLytFile.toLocal8Bit().data());
  rootLayout->SetAttribute("layout_properties_file",fullLypFile.toLocal8Bit().data());

  m_layout_lyp_file = fullLypFile;
  m_layout_tech_file = fullLytFile;

}
