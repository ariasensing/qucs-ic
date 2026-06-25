/// @file spdeembed.h
/// @brief S-parameter network N-port de-embedding component (definition)
/// @author Qucs Team; Andrés Martínez Mera
/// @date 2017-2026
/// @copyright Copyright (C) 2017 Qucs Team,
///            Based on sparamfile.cpp (C) 2003 by Michael Margraf
///            2026 Andrés Martínez Mera
///            Code porting from Qucs PR#693; GPLv3-or-later
/// @license GPL-3.0-or-later

#ifndef SPDEEMBED_H
#define SPDEEMBED_H

#include "component.h"
#include "componentdialog.h"

/// @class SPDeEmbedDialog
/// @brief Custom dialog for the S-parameter de-embedding component
class SPDeEmbedDialog : public ComponentDialog {
  Q_OBJECT
public:
  /// @brief Constructor
  SPDeEmbedDialog(Component* c, Schematic* s);
};


/// @class SPDeEmbed
/// @details S-parameter de-embedding component
class SPDeEmbed : public MultiViewComponent  {
public:
  SPDeEmbed();
  ~SPDeEmbed() {};
  Component* newOne() override;
  static Element* info(QString&, char* &, bool getNewOne=false);

  ComponentDialog* createDialog(Schematic* s) override;
  static int portsFromFilename(const QString& filename);
  QString getSubcircuitFile() override;

protected:
  QString netlist() override;
  void createSymbol() override;
};

#endif /* SPDEEMBED_H */
