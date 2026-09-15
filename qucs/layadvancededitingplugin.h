#ifndef LAYADVANCEDEDITINGPLUGIN_H
#define LAYADVANCEDEDITINGPLUGIN_H

#include "layPlugin.h"
#include "layLayoutViewBase.h"
#include "edtShapeService.h"
#include "dbManager.h"
#include "layObjectInstPath.h"
#include "dbShape.h"
#include "dbEdge.h"
#include "dbPoint.h"
#include "dbBox.h"
#include "dbPolygon.h"
#include "dbPath.h"
#include "dbRecursiveShapeIterator.h"
#include "dbInstElement.h"

#include <functional>

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

  enum operation {
    Selection             = 0,
    InsertRectangle ,
    InsertPath,
    InsertVias,
    InsertInstance,
    Move,
    Stretch,
    MirrorX,
    MirrorY,
    Chop,
    Copy,
    Paste,
    Unknown
  };

  operation     m_current_operation = Unknown;
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

  virtual void drag_cancel() override;

  virtual bool leave_event(bool /*prio*/) override;

  static QPointF micron_to_pixel(lay::LayoutViewBase* view, const db::DPoint& micron_pos);

  virtual bool key_event (unsigned int /*key*/, unsigned int /*buttons*/) override;

//-----------------------------------------------
// All functios are virtualized so that we have a single entry point

  bool (layAdvancedEditingPlugin::*mode_mouse_move_event)(const db::DPoint &p, unsigned int buttons, bool prio) = 0;
  bool (layAdvancedEditingPlugin::*mode_mouse_press_event)(const db::DPoint &p, unsigned int buttons, bool prio) = 0;
  bool (layAdvancedEditingPlugin::*mode_mouse_click_event)(const db::DPoint &p, unsigned int buttons, bool prio) = 0;
  bool (layAdvancedEditingPlugin::*mode_mouse_double_click_event)(const db::DPoint &p, unsigned int buttons, bool prio) = 0;
  bool (layAdvancedEditingPlugin::*mode_mouse_release_event)(const db::DPoint &p, unsigned int buttons, bool prio) = 0;
  void (layAdvancedEditingPlugin::*mode_start)() =0;
  void (layAdvancedEditingPlugin::*mode_complete)()=0;
  void (layAdvancedEditingPlugin::*mode_abort)()=0;
  void (layAdvancedEditingPlugin::*mode_drag_cancel)() = 0;
  bool (layAdvancedEditingPlugin::*mode_key_event)(unsigned int /*key*/, unsigned int /*buttons*/) = 0;

//-----------------------------------------------
// Plugins instance
  static layAdvancedEditingPlugin* get_plugin_from_view(lay::LayoutViewBase* view);

//-----------------------------------------------
// Actions
// Each mode has a "start" procedure, a "terminate" procedure (successful) and an
// "abort" procedure. Also they handle mouse events
  bool        is_idle() {return m_is_idle;}

// selection
  void        selection_start();
  void        selection_complete();
  void        selection_abort();
  bool        selection_mouse_move(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        selection_mouse_click(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        selection_mouse_press(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        selection_mouse_release(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        selection_mouse_double_click(const db::DPoint &p, unsigned int buttons, bool prio);
  void        selection_drag_cancel();
  bool        selection_key_event(unsigned int key, unsigned int buttons);


// rects.
  void        insert_rect_start();
  void        insert_rect_coord_given_p1(double x, double y);
  void        insert_rect_coord_given_p2(double x, double y);
  void        insert_rect_terminate();
  void        insert_rect_abort();
  bool        insert_rect_mouse_click(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        insert_rect_mouse_double_click(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        insert_rect_mouse_move(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        insert_rect_mouse_press(const db::DPoint &p, unsigned int buttons, bool prio);
  bool        insert_rect_mouse_release(const db::DPoint &p, unsigned int buttons, bool prio);

  void        terminate_action();
private:
  lay::LayoutViewBase *mp_view;
  // Helper for shape editing

  bool               m_is_idle;
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
  void            init_mode();
  bool            m_magnetic = true;
  db::DPoint      m_last_snapped;
  double          grid_micron() const;
  db::DPoint      snap_to_grid(const db::DPoint &p) const;
public:

  void            set_snap_mode(bool enable) {m_magnetic = enable;}
  bool            is_snap_mode() {return m_magnetic;}
  db::DPoint&     get_snapped_pos();

//------------------------------------------------------
// Partial selection
private:

  struct PartialSelection {
    lay::ObjectInstPath path;          // the shape (or instance path)
    int                 edge_index;    // >=0 → edge, -1 → not an edge
    int                 vertex_index;  // >=0 → vertex, -1 → not a vertex
    db::DEdge           edge;          // valid when edge_index >= 0
    db::DPoint          vertex;        // valid when vertex_index >= 0

    bool is_edge()   const { return edge_index   >= 0; }
    bool is_vertex() const { return vertex_index >= 0; }
  };
  std::vector<PartialSelection> m_hovering; // List of objects we are hovering over
  std::vector<PartialSelection> m_partial_selection;

  // Rubber-box selection
  bool       m_dragging;
  db::DPoint m_drag_start;
  db::DBox   m_drag_box;
  // Configuration
  double m_pick_tolerance_um = 0.005;   // how close the mouse must be (µm)

  void clear_partial_selection();
  void select_at_point(const db::DPoint &p, bool add);
  void select_in_box  (const db::DBox   &box, bool add);
  void collect_partials_from_shape(const db::Shape &shape,
                                   unsigned int layer,
                                   const db::CplxTrans &to_micron,
                                   const db::DBox &search_box,
                                   double pick_tol_um,
                                   int cv_index,
                                   db::cell_index_type cell_index,
                                   std::vector<PartialSelection> &out);

  void visualize_partial_selection();

         // Helpers
  db::DPolygon shape_to_dpolygon(const db::Shape &shape,
                                  const db::CplxTrans &to_micron);
  void extract_edges_and_vertices(const db::DPolygon &poly,
                                  std::vector<db::DEdge>  &edges,
                                  std::vector<db::DPoint> &vertices);

  void  update_cursor_markers();

  void  clear_hovering_list();
  void  update_hovering_list(const db::DPoint& pt);

};

#endif // LAYADVANCEDEDITINGPLUGIN_H
