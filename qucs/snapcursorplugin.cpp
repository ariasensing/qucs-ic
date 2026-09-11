#include "snapcursorplugin.h"
#include "tlString.h"
#include <cmath>
#include <algorithm>


QMap<lay::LayoutViewBase*, SnapCursorPlugin*> SnapCursorPlugin::m_mapPlugins;

SnapCursorPluginFactory* SnapCursorPluginFactory::m_decl = nullptr;

SnapCursorPlugin::SnapCursorPlugin(lay::LayoutViewBase *view)
    : lay::Plugin(view), mp_view(view), m_layout(nullptr)
{
  if (mp_view!=nullptr) m_mapPlugins[view] = this;
}

SnapCursorPlugin::~SnapCursorPlugin()
{
  delete mp_cursor;
  mp_cursor = nullptr;

  if (mp_view!=nullptr)
    m_mapPlugins.remove(mp_view);
}

void SnapCursorPlugin::activated()
{
  if (!mp_cursor) {
    mp_cursor = new lay::Marker(mp_view, mp_view->active_cellview_index());
    mp_cursor->set_frame_color(0x00ff00);
    mp_cursor->set_line_width(1);
    mp_cursor->set_vertex_size(9);
    mp_cursor->set_dither_pattern(1);   // hollow
  }
  mp_view->message("Snap cursor active (Esc to cancel)");
}

void SnapCursorPlugin::deactivated()
{
  delete mp_cursor;
  mp_cursor = nullptr;
  mp_view->message(std::string());
}

void SnapCursorPlugin::drag_cancel()
{
  deactivated();
}


void SnapCursorPlugin::set_magnetic(bool on)               { m_magnetic = on; }
void SnapCursorPlugin::set_catch_distance_px(double px)    { m_catch_dist_px = px; }
void SnapCursorPlugin::set_grid(double g)                  { m_grid = g; }

// ----------------------------------------------------------------------
// Convert pixels → micron using current zoom
double SnapCursorPlugin::pixels_to_micron(double pixels) const
{
  if (!mp_view)
    return 0.0;

         // viewport_trans: micron → pixel
  db::DCplxTrans micron2pixel = mp_view->viewport().trans();
  // scale factor (x component is enough for isotropic view)
  double scale = micron2pixel.mag();   // pixels per micron

  if (scale <= 0.0)
    return 0.0;

  return pixels / scale;               // micron
}

bool SnapCursorPlugin::mouse_click_event(int x, int y, unsigned int buttons, unsigned int keys)
{

}

// ----------------------------------------------------------------------
db::DPoint SnapCursorPlugin::snap_to_grid(const db::DPoint &p) const
{
  double g = grid_micron();
  if (g <= 0.0)
    return p;
  return db::DPoint(std::round(p.x() / g) * g,
                    std::round(p.y() / g) * g);
}
//-------------------------------------------------------------------------
// Use: 1. set grid, 2. default grid , 3. dbu
double SnapCursorPlugin::grid_micron() const
{
  if (m_grid > 0.0)
    return m_grid;

  double g = m_layout->technology()->default_grid();

  return (g > 0.0) ? g : m_layout->dbu();
}

// ------------------------------------------------------------------
// High-performance magnetic snap
db::DPoint SnapCursorPlugin::snap_magnetic(const db::DPoint &raw) const
{
  // Cache
  if (m_has_cache) {
    double dx = raw.x() - m_last_raw.x();
    double dy = raw.y() - m_last_raw.y();
    if (dx*dx + dy*dy < m_cache_radius2)
      return m_last_snapped;
  }
  return db::DPoint();
}

// ------------------------------------------------------------------
void SnapCursorPlugin::update_cursor(const db::DPoint &raw)
{
  db::DPoint s = snap_to_grid(raw);
  if (m_magnetic)
    s = snap_magnetic(s);

  m_snapped = s;

  if (!mp_cursor)
    return;

  const double size = 0.25; // µm (visual size of the target)
  db::DBox box(s.x() - size, s.y() - size, s.x() + size, s.y() + size);
  //mp_cursor->set(box, db::VCplxTrans());
}

// ------------------------------------------------------------------
bool SnapCursorPlugin::mouse_moved_event(const db::DPoint &p, unsigned int /*buttons*/, bool prio)
{
  if (!prio)
    return false;   // only when we are the active mode

  update_cursor(p);
  return false;     // do not steal the event from other tools
}
// ------------------------------------------------------------------
bool SnapCursorPlugin::mouse_button_pressed_event(const db::DPoint &p, unsigned int buttons, bool prio)
{
  if (!prio || (buttons & lay::LeftButton) == 0)
    return false;

  update_cursor(p);
  // Here you can emit a signal / call a callback with m_snapped
  return false;
}

// ------------------------------------------------------------------
SnapCursorPlugin* SnapCursorPlugin::get_plugin_from_view(lay::LayoutViewBase* view)
{
  QMap<lay::LayoutViewBase*, SnapCursorPlugin*>::iterator it = m_mapPlugins.find(view);
  if (it==m_mapPlugins.end()) return nullptr;
  return it.value();
}
// ------------------------------------------------------------------

static tl::RegisteredClass<lay::PluginDeclaration> snap_plugin_registration (
    new SnapCursorPluginFactory(),
    3000,
    "snap_cursor_mode"
    );
