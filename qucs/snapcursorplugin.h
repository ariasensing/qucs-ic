#ifndef SNAPCURSORPLUGIN_H
#define SNAPCURSORPLUGIN_H

#include "layPlugin.h"

#include "layLayoutView.h"
#include "layLayoutView_qt.h"
#include "layMarker.h"
#include "dbBox.h"
#include "dbEdge.h"
#include "dbShape.h"
#include "dbLayerProperties.h"
#include <set>
#include <limits>
#include <QMap>

class SnapCursorPlugin : public lay::Plugin
{
public:
  SnapCursorPlugin(lay::LayoutViewBase *view);
  ~SnapCursorPlugin();

  void activated();
  void deactivated();
  void drag_cancel();

  virtual bool mouse_moved_event(const db::DPoint &p, unsigned int buttons, bool prio) ;
  virtual bool mouse_button_pressed_event(const db::DPoint &p, unsigned int buttons, bool prio);
  bool mouse_click_event(int x, int y, unsigned int buttons, unsigned int keys);
         // Public control API
  void set_magnetic(bool on);
  void set_catch_distance_px(double pixels);   // ← now in pixels
  void set_grid(double grid_um);               // 0 = use view grid

  db::DPoint current_snapped_pos() const { return m_snapped; }
  static SnapCursorPlugin* get_plugin_from_view(lay::LayoutViewBase* view);

  void   set_layout(db::Layout* layout = nullptr) {m_layout = layout;}
private:
  static QMap<lay::LayoutViewBase*, SnapCursorPlugin*> m_mapPlugins;

  void update_cursor(const db::DPoint &raw);
  db::DPoint snap_to_grid(const db::DPoint &p) const;
  db::DPoint snap_magnetic(const db::DPoint &p) const;
  double grid_micron() const;
  double pixels_to_micron(double pixels) const;

  lay::LayoutViewBase *mp_view = nullptr;
  lay::Marker     *mp_cursor = nullptr;
  db::Layout      *m_layout = nullptr;

  bool     m_magnetic        = false;
  double   m_catch_dist_px   = 12.0;   // pixels (default ~12 px)
  double   m_grid            = 0.0;    // 0 → use view grid
  db::DPoint m_snapped;

         // performance cache
  mutable db::DPoint m_last_raw;
  mutable db::DPoint m_last_snapped;
  mutable bool       m_has_cache = false;
  mutable double     m_cache_radius2 = 0.0;
};




class SnapCursorPluginFactory : public lay::PluginDeclaration
{
  static SnapCursorPluginFactory* m_decl;
public:
  SnapCursorPluginFactory(){
    // title, menu path, icon (empty), mode name
    //register_plugin("Snap Cursor", "view_menu.snap_cursor", std::string(), "snap_cursor_mode");
    m_decl = this;
  }

  static SnapCursorPluginFactory* get_declaration() {return m_decl;}

  virtual lay::Plugin *create_plugin(db::Manager * /*manager*/,
                                     lay::Dispatcher * /*dispatcher*/,
                                     lay::LayoutViewBase *view) const override{


    return new SnapCursorPlugin(view);
  }
};


#endif // SNAPCURSORPLUGIN_H
