// layAdvancedEditingPlugin.cc
#include "layadvancededitingplugin.h"
#include "layLayoutViewBase.h"
#include "edtShapeService.h"
#include "tlLog.h"   // optional, for tl::info / tl::warn
#include "shapedrawer.h"


/**
 * @brief layAdvancedEditingPlugin::selection_start
 */
void        layAdvancedEditingPlugin::selection_start()
{
  if (m_current_operation==Selection)
    return;

  terminate_action();

  m_current_operation = Selection;
  mode_start                    = &selection_start;
  mode_complete                 = &selection_complete;
  mode_abort                    = &selection_abort;
  mode_mouse_move_event         = &selection_mouse_move;
  mode_mouse_click_event        = &selection_mouse_click;
  mode_mouse_double_click_event = &selection_mouse_double_click;
  mode_mouse_press_event        = &selection_mouse_press;
  mode_drag_cancel              = &selection_drag_cancel;
  mode_key_event                = &selection_key_event;
  mode_mouse_release_event      = &selection_mouse_release;
}

/**
 * @brief layAdvancedEditingPlugin::selection_terminate
 */

void        layAdvancedEditingPlugin::selection_terminate()
{
  // Keep the selection. Don't do anything since we stay in the same mode
}

/**
 * @brief layAdvancedEditingPlugin::selection_abort
 */
void        layAdvancedEditingPlugin::selection_abort()
{
  clear_partial_selection();
}
/**
 * @brief layAdvancedEditingPlugin::selection_mouse_click
 * @param p
 * @param buttons
 */
void        layAdvancedEditingPlugin::selection_mouse_click(const db::DPoint &p, unsigned int buttons)
{

}
/**
 * @brief layAdvancedEditingPlugin::selection_mouse_double_click
 * @param p
 * @param buttons
 */
void        layAdvancedEditingPlugin::selection_mouse_double_click(const db::DPoint &p, unsigned int buttons)
{

}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_press
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool        layAdvancedEditingPlugin::selection_mouse_press(const db::DPoint &p, unsigned int buttons, bool prio)
{
  if (buttons & lay::LeftButton) {
    m_dragging   = true;
    m_drag_start = p;
    m_drag_box   = db::DBox(p, p);
    return true;
  }
  return false;
}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_release
 * @param p
 * @param buttons
 * @param prio
 * @return
 */
bool        layAdvancedEditingPlugin::selection_mouse_release(const db::DPoint &p, unsigned int buttons, bool prio)
{

}

/**
 * @brief layAdvancedEditingPlugin::selection_drag_cancel
 */
void        layAdvancedEditingPlugin::selection_drag_cancel()
{

}

/**
 * @brief layAdvancedEditingPlugin::selection_mouse_move
 * @param p
 * @param buttons
 * @return
 */

bool        layAdvancedEditingPlugin::selection_mouse_move(const db::DPoint &p, unsigned int buttons, bool )
{
  // Check if left button is kept down
  if ((buttons & lay::LeftButton))
  {
    if (!m_dragging)
    {
      m_dragging   = true;
      m_drag_start = m_last_snapped;
      m_drag_box   = db::DBox(m_last_snapped, m_last_snapped);
    }
  }

  update_cursor_markers();

  return false;
}


/**
 * @brief layAdvancedEditingPlugin::selection_key_event
 * @param key
 * @param buttons
 * @return
 */
bool        layAdvancedEditingPlugin::selection_key_event(unsigned int key, unsigned int buttons)
{

}