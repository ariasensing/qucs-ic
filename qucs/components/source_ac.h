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
#include <QRegularExpression>

class Source_ac : public Component  {
private:
  QString ngspice_netlist();
  QString xyce_netlist();

  /// @brief Prepare the ngspice netline for the multitone setup
  /// @param z0 Characteristic impedance
  /// @param freqs List of the tone frequencies
  /// @param powers List of powers per carrier in dBm (to be converter in Vpeak inside)
  /// @param phases List of phases per carrier
  /// @param enTran Flag for the transient simulation
  QString multitone_ngspice(double z0,
                            const QStringList &freqs,
                            const QStringList &powers,
                            const QStringList &phases, bool enTran);

  /// @brief Prepare the Xyce netline for the multitone setup
  /// @param z0 Characteristic impedance
  /// @param freqs List of the tone frequencies
  /// @param powers List of powers per carrier in dBm (to be converter in Vpeak inside)
  /// @param phases List of phases per carrier
  /// @param enTran Flag for the transient simulation
  QString multitone_xyce (double z0,
                         const QStringList &freqs,
                         const QStringList &powers,
                         const QStringList &phases, bool enTran);

  /// @brief Prepare the ngspice netline for the single-tone setup
  /// @param z0 Characteristic impedance
  /// @param freq Frequency of the tone
  /// @param power RMS power
  /// @param phases Phase
  /// @param enTran Flag for the transient simulation
  /// @param isTermination Flag for passive port
  QString singletone_ngspice(const QString &nodeString, double z0,
                             const QString &freq, const QString &pVal,
                             const QString &phase,
                             bool enTran, bool isTermination);

  /// @brief Prepare the Xyce netline for the single-tone setup
  /// @param z0 Characteristic impedance
  /// @param freq Frequency of the tone
  /// @param power RMS power
  /// @param phases Phase
  /// @param enTran Flag for the transient simulation
  /// @param isTermination Flag for passive port
  QString singletone_xyce(const QString &nodeString, double z0,
                          const QString &freq,
                          const QString &pVal,
                          const QString &phase,
                          bool enTran, bool isTermination);



  /// @brief Split a comma-separated property string into a trimmed list.
  /// @param raw String input from a component property field
  QStringList parseList(const QString &raw) const;

  /// @brief Convert the RMS power into a Xyce/ngspice netlist
  /// @param pVal Power in dBm
  /// @param z0 Characteristic impedance
  /// @param dialect Spice dialect: Xyce of ngspice
  QString resolveVamp(const QString &pVal, double z0, spicecompat::SpiceDialect dialect) const;

public:
  Source_ac();
  ~Source_ac();
  Component* newOne();
  static Element* info(QString&, char* &, bool getNewOne=false);
protected:
  QString spice_netlist(spicecompat::SpiceDialect dialect = spicecompat::SPICEDefault);
  QString netlist();
};

#endif
