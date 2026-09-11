// layAdvancedEditingPluginDeclaration.cc
#include "layPlugin.h"
#include "tlClassRegistry.h"
#include "layadvancededitingplugin.h".h"

class layAdvancedEditingPluginDeclaration : public lay::PluginDeclaration
{
public:
  layAdvancedEditingPluginDeclaration()
  {
    // Optional: you can also put registration logic here,
    // but the usual pattern is the static RegisteredClass below.
  }

         // Optional: configuration keys (name, default value)
  virtual void get_options(std::vector<std::pair<std::string, std::string>> &options) const override
  {
    // options.push_back(std::make_pair("my_plugin.some_option", "default"));
  }

         // Optional: menu entries
  virtual void get_menu_entries(std::vector<lay::MenuEntry> &menu_entries) const override
  {
    // Example:
    // menu_entries.push_back(
    //   lay::menu_item("my_plugin::do_something", "my_plugin_do_something",
    //                  "tools_menu.end", "My Plugin Action"));
  }

         // Tell KLayout this is a mouse mode (appears in the toolbar / mode list)
  virtual bool implements_mouse_mode(std::string &title) const override
  {
    title = "adv_layout";          // text shown in the UI
    return true;
  }

         // Optional: also expose it as an editable mode
         // virtual bool implements_editable(std::string &title) const override { ... }

         // The actual factory method
  virtual lay::Plugin *create_plugin(db::Manager *manager,
                                     lay::Dispatcher *dispatcher,
                                     lay::LayoutViewBase *view) const override
  {
    return new layAdvancedEditingPlugin(manager, dispatcher, view);
  }
};

// Register the declaration. The integer is the priority/order
// (lower numbers appear earlier). The string is a unique name.
static tl::RegisteredClass<lay::PluginDeclaration> s_decl(
    new layAdvancedEditingPluginDeclaration(),
    20000,                    // position among other plugins
    "adv_layout"              // unique internal name
    );