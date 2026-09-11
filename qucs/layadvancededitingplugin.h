#ifndef LAYADVANCEDEDITINGPLUGIN_H
#define LAYADVANCEDEDITINGPLUGIN_H

#include "layPlugin.h"
#include "layLayoutViewBase.h"
#include "dbManager.h"
#include "shapedrawer.h"
#include "dbShape.h"
#include <QObject>
#include <QMap>
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

  void        terminate_action() {}
private:
  lay::LayoutViewBase *mp_view;
  // Helper for shape editing

  bool         m_is_idle;        //
  ShapeDrawer *m_shapeDrawer;

signals:
  void  update_mouse_position(double xdb, double ydb,  int pxx,  int pxy);
  void  insert_started();
  void  insert_done(bool added, const db::Shape& shape);
private:
  QPointF micron_to_pixel(const db::DPoint& micron_pos);


};

#endif // LAYADVANCEDEDITINGPLUGIN_H
