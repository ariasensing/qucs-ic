#ifndef LAYADVANCEDEDITINGPLUGIN_H
#define LAYADVANCEDEDITINGPLUGIN_H

#include "layPlugin.h"
#include "layLayoutViewBase.h"
#include "dbManager.h"
#include "dbShape.h"
#include <QObject>
#include <QMap>

#include "shapedrawer.h"
namespace lay {
class Dispatcher;
}


class layAdvancedEditingPlugin : public QObject,  public lay::Plugin ,  public lay::ViewService
{
  Q_OBJECT
private:
  // Let's keep a copy of the instantiated plugins
  static QMap<lay::LayoutViewBase*, layAdvancedEditingPlugin*> m_mapped_plugins;
public:
  layAdvancedEditingPlugin(db::Manager *manager, lay::Dispatcher *dispatcher, lay::LayoutViewBase *view);
  virtual ~layAdvancedEditingPlugin();

         // Called when the tool becomes the active mouse mode
  virtual void activated() override;

         // Called when another tool becomes active
  virtual void deactivated() override;

         // Optional: react to view changes
  virtual void update() override;

         // Mouse events (return true if the event was consumed)
  virtual bool mouse_move_event(const db::DPoint &p, unsigned int buttons, bool prio) override;
  virtual bool mouse_press_event(const db::DPoint &p, unsigned int buttons, bool prio) override;
  virtual bool mouse_click_event(const db::DPoint &p, unsigned int buttons, bool prio) override;
  virtual bool mouse_double_click_event(const db::DPoint &p, unsigned int buttons, bool prio) override;
  virtual bool mouse_release_event(const db::DPoint &p, unsigned int buttons, bool prio) override;
  virtual bool wheel_event(int delta, bool horizontal, const db::DPoint &p, unsigned int buttons, bool prio) override;

         // Optional configuration
  virtual bool configure(const std::string &name, const std::string &value) override;
  virtual void config_finalize() override;

         // Optional menu handling for this instance
  virtual void menu_activated(const std::string &symbol) override;
//-----------------------------------------------
// Plugins instance
  static layAdvancedEditingPlugin* get_plugin_from_view(lay::LayoutViewBase* view);

//-----------------------------------------------
// Actions
  bool        is_idle() {return m_is_idle;}
// rects
  void        insert_rect_start();
  void        insert_rect_coord_given_p1(double x, double y);
  void        insert_rect_coord_given_p2(double x, double y);
  void        insert_rect_terminate();
// paths
  void        insert_path_start() {}
  void        insert_path_coord_given(double x, double y) {}
  void        insert_path_terminate() {}
// circles
  void        insert_circle_start() {}
  void        insert_circle_terminate() {}

  void        terminate_action();
private:
  lay::LayoutViewBase *mp_view;
  // Helper for shape editing

  bool         m_is_idle;        //
  class ShapeDrawer *m_shapeDrawer;

signals:
  void  update_mouse_position(double xdb, double ydb,  int pxx,  int pxy);
  void  insert_started();
  void  insert_done(bool added, const db::Shape& shape);
  void  set_query_name(const QString& query_name);
private:
  QPointF micron_to_pixel(const db::DPoint& micron_pos);
public slots:
  void   zoom_on_new_position(double x, double y);
  void   get_new_coords(double x, double y);
  void   get_new_value(double x);


//----------------------------------------------------
// Snapping

public:
  void set_magnetic(bool on);
  void set_catch_distance(double microns);   // e.g. 0.5 µm
  void set_grid(double grid_um);             // 0 = use view’s editor grid

  db::DPoint current_snapped_pos() const { return m_snapped; }

private:
  void update_cursor(const db::DPoint &raw);
  db::DPoint snap_to_grid(const db::DPoint &p) const;
  db::DPoint snap_magnetic(const db::DPoint &p) const;

  lay::LayoutView *mp_view;
  lay::Marker     *mp_cursor = nullptr;   // the visible cross-hair

  bool     m_magnetic     = false;
  double   m_catch_dist   = 0.5;          // µm
  double   m_grid         = 0.0;          // 0 → use view grid
  db::DPoint m_snapped;

};

#endif // LAYADVANCEDEDITINGPLUGIN_H
