#ifndef LAYADVANCEDEDITINGPLUGIN_H
#define LAYADVANCEDEDITINGPLUGIN_H

#include "layPlugin.h"
#include "layLayoutViewBase.h"
#include "edtShapeService.h"
#include "dbManager.h"
#include "dbShape.h"
#include <QObject>
#include <QMap>

#include "shapedrawer.h"
namespace lay {
class Dispatcher;
}

/*
 * This the main controller for the advanced editing function. It is, as a matter of fact, the glue
 * logic in between the layLayoutView and the shapeDrawer subclasses
 * */

class layAdvancedEditingPlugin : public QObject,  public edt::Service
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

  static QPointF micron_to_pixel(lay::LayoutViewBase* view, const db::DPoint& micron_pos);
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
  void        insert_path_coord_given(double , double ) {}
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
  void  update_mouse_position(const db::DPoint& pt);
  void  insert_started();
  void  insert_done(bool added, const db::Shape& shape);
  void  set_query_name(const QString& query_name);
private:
  QPointF micron_to_pixel(const db::DPoint& micron_pos);
public slots:
  void   zoom_on_new_position(double x, double y);
  void   set_new_coords(double x, double y);
  void   set_new_value(double x);


//----------------------------------------------------
// Snapping
private:
  bool            m_magnetic = true;
  db::DPoint      m_last_snapped;
  double          grid_micron() const;
  db::DPoint      snap_to_grid(const db::DPoint &p) const;
public:

  void            set_snap_mode(bool enable) {m_magnetic = enable;}
  bool            is_snap_mode() {return m_magnetic;}
  db::DPoint&     get_snapped_pos();

};

#endif // LAYADVANCEDEDITINGPLUGIN_H
