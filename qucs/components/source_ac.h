/***************************************************************************
                          source_ac.h  -  description
                             -------------------
    begin                : Sat Aug 23 2003
    copyright            : (C) 2003 by Michael Margraf
    email                : michael.margraf@alumni.tu-berlin.de
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 ***************************************************************************/

#ifndef SOURCE_AC_H
#define SOURCE_AC_H

#include "component.h"
#include "misc.h"
#include <QRegularExpression>
#include "schematic.h" // Needed to detect the type of simulation and adjust the qucsator-rf netlist accordingly

class Source_ac : public Component  {
private:
  /// @brief Build the ngspice netlist line for this component.
  QString ngspice_netlist();

  /// @brief Build the ngspice netlist line for this component.
  QString xyce_netlist();

  /// @brief Prepare the ngspice netline for the multitone setup (a series R (Z0) + as many series voltage sources as tones)
  /// @param z0     Characteristic impedance
  /// @param freqs  List of the tone frequencies
  /// @param powers List of powers per carrier in dBm
  /// @param phases List of phases per carrier
  /// @param enTran Flag for the transient simulation.
  ///               When true, append a SIN(...) transient specification to each voltage source
  QString multitone_ngspice(double z0,
                            const QStringList &freqs,
                            const QStringList &powers,
                            const QStringList &phases, bool enTran);

  /// @brief Prepare the Xyce netline for the multitone setup (a series R (Z0) + as many series voltage sources as tones)
  /// @param z0     Characteristic impedance
  /// @param freqs  List of the tone frequencies
  /// @param powers List of powers per carrier in dBm (to be converter in Vpeak inside)
  /// @param phases List of phases per carrier
  /// @param enTran Flag for the transient simulation.
  ///               When true, append a SIN(...) transient specification to each voltage source
  QString multitone_xyce (double z0,
                         const QStringList &freqs,
                         const QStringList &powers,
                         const QStringList &phases, bool enTran);

  /// @brief Prepare the ngspice netline for the single-tone setup
  /// @param z0 Characteristic impedance
  /// @param freq Frequency of the tone
  /// @param power Available power (dBm)
  /// @param phases Initial phase
  /// @param enTran Flag for the transient simulation
  ///               When true, append a SIN(...) transient specification.
  /// @param isTermination Flag for passive port (Vamp = 0)
  QString singletone_ngspice(const QString &nodeString, double z0,
                             const QString &freq, const QString &pVal,
                             const QString &phase,
                             bool enTran, bool isTermination);

  /// @brief Prepare the Xyce netline for the single-tone setup
  /// @param z0 Characteristic impedance
  /// @param freq Frequency of the tone
  /// @param power Available power (dBm)
  /// @param phases Initial phase
  /// @param enTran Flag for the transient simulation
  ///               When true, append a SIN(...) transient specification.
  /// @param isTermination Flag for passive port (Vamp = 0)
  QString singletone_xyce(const QString &nodeString, double z0,
                          const QString &freq,
                          const QString &pVal,
                          const QString &phase,
                          bool enTran, bool isTermination);

  /// @brief Prepare the ngspice netline for the single-tone setup
  /// @param z0 Characteristic impedance
  /// @param freq Frequency of the tone
  /// @param power Available power (dBm)
  /// @param phases Initial phase
  /// @param temp Temperature
  QString multitone_qucsator(double z0,
                             const QStringList &freqs,
                             const QStringList &powers,
                             const QStringList &phases,
                             const QString &temp);



  /// @brief Split a comma-separated property string into a trimmed list.
  /// @param raw String input from a component property field
  /// @details Surrounding brackets or braces (e.g. "{1 GHz, 2 GHz}" or "[1 GHz; 2 GHz]")
  /// are stripped before splitting.
  QStringList parseList(const QString &raw) const;

  /// @brief Convert the RMS power into a Xyce/ngspice netlist
  /// @param pVal Power in dBm
  /// @param z0 Characteristic impedance
  /// @param dialect Spice dialect: Xyce of ngspice
  /// @details When @p pVal is a numeric dBm literal the result is pre-computed:
  /// Vamp = 2 * sqrt(2) * sqrt(z0/1000) * 10^(pVal/20)
  /// When @p pVal is a .PARAM name the conversion is embedded as an
  /// expression in the dialect-appropriate quoting style (single quotes for ngspice, curly braces for Xyce).
  QString resolveVamp(const QString &pVal, double z0, spicecompat::SpiceDialect dialect) const;

public:
  /// @brief Class constructor
  Source_ac();

  /// @brief Class destructor
  ~Source_ac();

  /// @brief Create a fresh instance of this component.
  /// @return Heap-allocated Source_ac; ownership passes to the caller.
  Component* newOne();

  /// @brief Return display metadata and optionally create a new instance.
  /// @param Name        Name of the component
  /// @param BitmapFile  Bitmap file with the symbol
  /// @param getNewOne   When true, allocate and return a new instance.
  /// @return            New Source_ac instance if @p getNewOne is true, else 0.
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:

  /// @brief Dispatcher: route to the correct dialect-specific netlist builder.
  /// @param dialect  SPICEXyce selects the Xyce builder
  ///                 Otherwise, it selects the ngspice builder.
  /// @return         The netlist fragment for this component.
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);

  /// @brief Build the qucsator netlist line for this component.
  QString netlist();
};

#endif
