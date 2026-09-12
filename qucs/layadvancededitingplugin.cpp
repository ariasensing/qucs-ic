// layAdvancedEditingPlugin.cc
#include "layadvancededitingplugin.h"
#include "layLayoutViewBase.h"
#include "tlLog.h"   // optional, for tl::info / tl::warn
#include "mouseselectiondrawer.h"
#include "shapedrawer.h"

QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*> layAdvancedEditingPlugin::m_mapped_plugins = QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*> ();

layAdvancedEditingPlugin::layAdvancedEditingPlugin(db::Manager * /*manager*/, lay::Dispatcher * /*dispatcher*/, lay::LayoutViewBase *view)
    : lay::Plugin(view),   // important: base constructor takes the view
      lay::ViewService(view ? view->canvas() : nullptr),
      mp_view(view),
      m_is_idle(true),
      m_shapeDrawer(nullptr)
{
  if (mp_view!=nullptr)
    m_mapped_plugins[mp_view] = this;

  if ((mp_view!=nullptr)&&(mp_view->widget()!=nullptr))
    mp_view->widget()->setCursor(Qt::BlankCursor);

  // Build a default (selection) shapeDrawer

  if ((mp_view!=nullptr)&&(mp_view->widget()!=nullptr))
    m_shapeDrawer = new MouseSelectionDrawer(mp_view,mp_view->widget());

}

/**
 * @brief layAdvancedEditingPlugin::get_plugin_from_view
 * @param view
 * @return
 */
layAdvancedEditingPlugin* layAdvancedEditingPlugin::get_plugin_from_view(lay::LayoutViewBase* view)
{
  QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*>::iterator it = m_mapped_plugins.find(view);
  if (it == m_mapped_plugins.end()) return nullptr;
  return it.value();
}
/**
 * @brief layAdvancedEditingPlugin::~layAdvancedEditingPlugin
 */
layAdvancedEditingPlugin::~layAdvancedEditingPlugin()
{
  m_mapped_plugins.remove(mp_view);
}
/**
 * @brief layAdvancedEditingPlugin::activated
 */
void layAdvancedEditingPlugin::activated()
{
  // Example: change cursor when the tool is selected
  if (mp_view) {
    set_cursor(lay::Cursor::cross);
  }
  tl::info << "layAdvancedEditingPlugin activated";
}
/**
 * @brief layAdvancedEditingPlugin::deactivated
 */
void layAdvancedEditingPlugin::deactivated()
{
  if (mp_view) {
    set_cursor(lay::Cursor::arrow);
  }
  tl::info << "layAdvancedEditingPlugin deactivated";
}

void layAdvancedEditingPlugin::update()
{
  // Called when the view (layers, cell, etc.) changes
}
/**
 * @brief layAdvancedEditingPlugin::mouse_move_event
 * @param p
 * @param prio
 * @return
 */

bool layAdvancedEditingPlugin::mouse_move_event(const db::DPoint &p, unsigned int /*buttons*/, bool /*prio*/)
{
  if (m_shapeDrawer==nullptr) return false;

  m_shapeDrawer->set_new_mouse_position(p);

  db::DPoint snapped = m_shapeDrawer->current_snapped_pos();

  QPointF ppx = micron_to_pixel(snapped);

  emit  update_mouse_position(snapped.x(), snapped.y(), static_cast<int>(std::round(ppx.x())), static_cast<int>(std::round(ppx.y())));


  return false;     // or true if you consume the event
}

/**
 * @brief layAdvancedEditingPlugin::mouse_press_event
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool layAdvancedEditingPlugin::mouse_press_event(const db::DPoint &p, unsigned int buttons, bool prio)
{
  if (!prio) {
    return false;
  }
  // Example reaction
  if ((buttons & lay::LeftButton) != 0) {
    tl::info << "Left click at " << p.to_string();
    // grab_mouse();  // if you want exclusive mouse capture
    return true;
  }
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_click_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_click_event(const db::DPoint & /*p*/, unsigned int /*buttons*/, bool /*prio*/)
{
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_double_click_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_double_click_event(const db::DPoint & /*p*/, unsigned int /*buttons*/, bool /*prio*/)
{
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::mouse_release_event
 * @return
 */
bool layAdvancedEditingPlugin::mouse_release_event(const db::DPoint & /*p*/, unsigned int /*buttons*/, bool /*prio*/)
{
  // ungrab_mouse();
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::wheel_event
 * @return
 */
bool layAdvancedEditingPlugin::wheel_event(int /*delta*/, bool /*horizontal*/,
                           const db::DPoint & /*p*/, unsigned int /*buttons*/, bool /*prio*/)
{
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::configure
 * @param name
 * @param value
 * @return
 */
bool layAdvancedEditingPlugin::configure(const std::string &name, const std::string &value)
{
  // Handle configuration keys you registered in the factory
  // return true if you consumed the key
  (void)name;
  (void)value;
  return false;
}
/**
 * @brief layAdvancedEditingPlugin::config_finalize
 */
void layAdvancedEditingPlugin::config_finalize()
{
  // Called after a batch of configure() calls
}
/**
 * @brief layAdvancedEditingPlugin::menu_activated
 * @param symbol
 */
void layAdvancedEditingPlugin::menu_activated(const std::string &symbol)
{
  // Handle menu symbols that target this plugin instance
  (void)symbol;
}

/**
 * @brief layAdvancedEditingPlugin::micron_to_pixel
 * @param micron_pos
 * @return
 */
QPointF layAdvancedEditingPlugin::micron_to_pixel(const db::DPoint& micron_pos)
{
  return micron_to_pixel(mp_view, micron_pos);
}
/**
 * @brief layAdvancedEditingPlugin::terminate_action Terminate whatever action was on-going
 */
void        layAdvancedEditingPlugin::terminate_action()
{

}

/**
 * @brief layAdvancedEditingPlugin::zoom_on_new_position
 * @param x
 * @param y
 */
void   layAdvancedEditingPlugin::zoom_on_new_position(double , double )
{

}

/**
 * @brief layAdvancedEditingPlugin::set_new_coords
 * @param x
 * @param y
 */
void   layAdvancedEditingPlugin::set_new_coords(double , double )
{

}
/**
 * @brief layAdvancedEditingPlugin::set_new_value
 * @param x
 */
void   layAdvancedEditingPlugin::set_new_value(double )
{

}


// Helper function
QPointF layAdvancedEditingPlugin::micron_to_pixel(lay::LayoutViewBase* view, const db::DPoint &micronPos)
{
  if (!view || !view->widget())
    return QPointF();

  db::DCplxTrans micron2pixel = view->viewport().trans();
  db::DPoint pixel = micron2pixel * micronPos;

  QWidget *canvas = view->widget();
  int x = static_cast<int>(std::round(pixel.x()));
  int y = canvas->height() - 1 - static_cast<int>(std::round(pixel.y()));

  return QPointF(x, y);
}

