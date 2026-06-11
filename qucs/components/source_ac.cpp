/***************************************************************************
                               source_ac.cpp
                              ---------------
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

#include "source_ac.h"
#include "node.h"
#include "extsimkernels/spicecompat.h"

#include <cmath>

Source_ac::Source_ac()
{
  Description = QObject::tr("ac power source");

  Lines.append(new qucs::Line(-22,-11, 22,-11,QPen(Qt::darkGray,0)));
  Lines.append(new qucs::Line(-22, 11, 22, 11,QPen(Qt::darkGray,0)));
  Lines.append(new qucs::Line(-22,-11,-22, 11,QPen(Qt::darkGray,0)));
  Lines.append(new qucs::Line( 22,-11, 22, 11,QPen(Qt::darkGray,0)));

  Arcs.append(new qucs::Arc(-19, -9, 18, 18,     0, 16*360,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-13, -6,  6,  6,16*270, 16*180,QPen(Qt::darkBlue,2)));
  Arcs.append(new qucs::Arc(-13,  0,  6,  6, 16*90, 16*180,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(-30,  0,-19,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 30,  0, 19,  0,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( -1,  0,  3,  0,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line(  3, -5, 19, -5,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  3,  5, 19,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line(  3, -5,  3,  5,QPen(Qt::darkBlue,2)));
  Lines.append(new qucs::Line( 19, -5, 19,  5,QPen(Qt::darkBlue,2)));

  Lines.append(new qucs::Line( 25,  5, 25, 11,QPen(Qt::red,1)));
  Lines.append(new qucs::Line( 28,  8, 22,  8,QPen(Qt::red,1)));
  Lines.append(new qucs::Line(-25,  5,-25, 11,QPen(Qt::black,1)));

  Ports.append(new Port( 30,  0));
  Ports.append(new Port(-30,  0));

  x1 = -30; y1 = -14;
  x2 =  30; y2 =  14;

  tx = x1+4;
  ty = y2+4;
  Model = "Pac";
  SpiceModel = "P";
  Name  = "P";

  // This property must be the first one !
  Props.append(new Property("Num", "1", true,
                QObject::tr("number of the port")));
  Props.append(new Property("Z", "50 Ohm", true,
                QObject::tr("port impedance")));
  Props.append(new Property("P", "0 dBm", false,
                QObject::tr("(available) ac power in dBm")));
  Props.append(new Property("f", "1 MHz", false,
                QObject::tr("frequency in Hertz")));
  Props.append(new Property("Phase", "0", false,
                            QObject::tr("initial phase in degrees")));

  Props.append(new Property("Temp", "26.85", false,
        QObject::tr("simulation temperature in degree Celsius")));
  Props.append(new Property("EnableTran", "true", false,
    QObject::tr("enable transient model as sine source [true,false]")));
  Props.append(new Property("LoadOnly", "false", false,
    QObject::tr("disable as a source (AC and transient), passive termination only [true,false]")));

  rotate();  // fix historical flaw
}

Source_ac::~Source_ac()
{
}

Component* Source_ac::newOne()
{
  return new Source_ac();
}

Element* Source_ac::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Power Source");
  BitmapFile = (char *) "source";

  if(getNewOne)  return new Source_ac();
  return 0;
}


QString Source_ac::ngspice_netlist()
{
    QString s = QStringLiteral("V%1").arg(Name);
    for (Port *p1 : std::as_const(Ports)) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }

    // Get source parameters (as lists)
    double z0 = spicecompat::normalize_value(getProperty("Z")->Value).toDouble();
    const QStringList freqs   = parseList(getProperty("f")->Value);
    const QStringList powers  = parseList(getProperty("P")->Value);
    const QStringList phases  = parseList(getProperty("Phase")->Value);


    bool en_tran = true;
    if (getProperty("EnableTran")->Value == "true") {
        en_tran = true;
    } else {
        en_tran = false;
    }

    // if user has explicitly set LoadOnly OR if P is empty (unset), the port acts as a terminated port (a passive load).
    const bool isTermination = (getProperty("LoadOnly")->Value == "true")
                               || (powers.isEmpty() || powers.first().isEmpty());


    // Multi-tone setup
    if (freqs.size() > 1 && !isTermination) {
      return multitone_ngspice(z0, freqs, powers, phases, en_tran);
    } else {
      // Single-tone setup
      const QString pVal  = powers.isEmpty() ? QString() : powers.first();
      const QString f     = freqs.isEmpty()  ? QString() : spicecompat::normalize_value(freqs.first());
      const QString phase = phases.isEmpty() ? QStringLiteral("0") : phases.first();
      return singletone_ngspice(s, z0, f, pVal, phase, en_tran, isTermination);
    }
}

QString Source_ac::multitone_ngspice(double z0,
                                     const QStringList &freqs,
                                     const QStringList &powers,
                                     const QStringList &phases,
                                     bool enTran)
{
  // Resolve external node names
  auto toSpiceNode = [](Port *p) {
    const QString n = p->Connection->Name;
    return (n == "gnd") ? QStringLiteral("0") : n;
  };
  const QString nodePos = toSpiceNode(Ports.at(0));
  const QString nodeNeg = toSpiceNode(Ports.at(1));

  // Nodes between the series sources: "P1_n0", "P1_n1", ...
  auto junctionNode = [&](int k) {
    return QStringLiteral("%1_n%2").arg(Name).arg(k);
  };

  // Broadcast a single-entry list to all tones
  // Falls back to a default if the list is empty or shorter than the tone index.
  auto pick = [](const QStringList &list, int i, const QString &fallback) -> QString {
    if (list.isEmpty())    return fallback;
    if (list.size() == 1)  return list.at(0);
    return (i < list.size()) ? list.at(i) : fallback;
  };

  const int N = freqs.size();
  QString s;

  // Series impedance, Z0
  s += QStringLiteral("R_%1 %2 %3 %4\n")
           .arg(Name, nodePos, junctionNode(0))
           .arg(z0, 0, 'g', 8);

  // Series AC voltage sources
  for (int i = 0; i < N; ++i) {
    const QString pVal      = pick(powers, i, QStringLiteral("0"));
    const QString freq      = spicecompat::normalize_value(freqs.at(i));
    const QString phase     = pick(phases, i, QStringLiteral("0"));
    const QString vamp      = resolveVamp(pVal, z0, spicecompat::SPICEDefault);
    const QString nodeAbove = junctionNode(i);
    const QString nodeBelow = (i == N - 1) ? nodeNeg : junctionNode(i + 1);

    s += QStringLiteral("V_%1_t%2 %3 %4 DC 0 AC %5")
             .arg(Name).arg(i + 1)
             .arg(nodeAbove, nodeBelow, vamp);

    if (enTran){
      s += QStringLiteral(" SIN(0 %1 %2 0 0 %3)").arg(vamp, freq, phase);
    }
    s += '\n';
  }

  return s;
}


QString Source_ac::singletone_ngspice(const QString &nodeString, double z0,
                                      const QString &freq, const QString &pVal,
                                      const QString &phase,
                                      bool enTran, bool isTermination)
{
  QString s = nodeString;
  const QString vamp = resolveVamp(pVal, z0, spicecompat::SPICEDefault);

  if (isTermination) {
    s += QStringLiteral(" dc 0 ac 0");
  } else {
    s += QStringLiteral(" dc 0 ac %1").arg(vamp);
    if (enTran)
      s += QStringLiteral(" SIN(0 %1 %2 0 0 %3)").arg(vamp, freq, phase);
  }

  s += QStringLiteral(" portnum %1").arg(getProperty("Num")->Value);
  s += QStringLiteral(" z0 %1").arg(z0);
  s += "\n";
  return s;
}


QString Source_ac::xyce_netlist()
{
    QString s = spicecompat::check_refdes(Name,SpiceModel);
    for (Port *p1 : std::as_const(Ports)) {
        QString nam = p1->Connection->Name;
        if (nam=="gnd") nam = "0";
        s += " "+ nam;   // node names
    }
    s += QStringLiteral(" port=%1 ").arg(getProperty("Num")->Value);

    // Get source parameters
    QString s_z0 = spicecompat::normalize_value(getProperty("Z")->Value);
    double z0 = s_z0.toDouble();

    const QStringList freqs  = parseList(getProperty("f")->Value);
    const QStringList powers = parseList(getProperty("P")->Value);
    const QStringList phases = parseList(getProperty("Phase")->Value);

    const bool en_tran       = (getProperty("EnableTran")->Value == "true");

    // if user has explicitly set LoadOnly OR
    // if P is empty (unset), the port acts as a terminated port (a passive load).
    const bool isTermination = (getProperty("LoadOnly")->Value == "true")
                               || (powers.isEmpty() || powers.first().isEmpty());

    if (freqs.size() > 1 && !isTermination) {
      // Multi-tone
      return multitone_xyce(z0, freqs, powers, phases, en_tran);
    } else {
      // Single-tone
      const QString pVal  = powers.isEmpty() ? QString() : powers.first();
      const QString freq  = freqs.isEmpty()  ? QString() : spicecompat::normalize_value(freqs.first());
      const QString phase = phases.isEmpty() ? QStringLiteral("0") : phases.first();
      return singletone_xyce(s, z0, freq, pVal, phase, en_tran, isTermination);
    }

}

QString Source_ac::singletone_xyce(const QString &nodeString, double z0,
                                   const QString &freq, const QString &pVal,
                                   const QString &phase,
                                   bool enTran, bool isTermination)
{
  const QString s_z0 = QString::number(z0, 'g', 8);
  QString s = nodeString;
  const QString vamp = resolveVamp(pVal, z0, spicecompat::SPICEXyce);

  // ACPHASE on the Xyce PORT device must be a numeric literal;
  // a parametric phase is wrapped in {...} only for the SIN transient source.
  bool isPhaseNumeric = false;
  phase.toDouble(&isPhaseNumeric);
  const QString acPhase    = isPhaseNumeric ? phase : QStringLiteral("0");
  const QString transPhase = isPhaseNumeric ? phase : ('{' + phase + '}');

  s += QStringLiteral(" z0=%1 ").arg(s_z0);
  s += QStringLiteral(" AC %1 %2 ").arg(vamp, acPhase);
  if (enTran && !isTermination)
    s += QStringLiteral(" SIN 0 %1 %2 0 0 %3").arg(vamp, freq, transPhase);

  s += "\n";
  return s;
}


QString Source_ac::multitone_xyce(double z0,
                                  const QStringList &freqs,
                                  const QStringList &powers,
                                  const QStringList &phases,
                                  bool enTran){
  const QString s_z0 = QString::number(z0, 'g', 8);
  const int N = freqs.size();

  auto nodeName = [](Port *p) {
    QString n = p->Connection->Name;
    return (n == "gnd") ? QStringLiteral("0") : n;
  };
  const QString nodePos = nodeName(Ports.at(0));
  const QString nodeNeg = nodeName(Ports.at(1));
  auto intNode = [&](int k) {
    return QStringLiteral("%1_n%2").arg(Name).arg(k);
  };

  QString s;

  // Series impedance resistor
  s += QStringLiteral("R_%1 %2 %3 %4\n")
           .arg(Name, nodePos, intNode(0), s_z0);

  // One voltage source per tone
  // Helper: pick list[i] if available, broadcast list[0] if list has one entry, else fallback
  auto pick = [](const QStringList &list, int i, const QString &fallback) -> QString {
    if (list.isEmpty())       return fallback;
    if (list.size() == 1)     return list.at(0);   // broadcast single entry to all tones
    if (i < list.size())      return list.at(i);
    return fallback;
  };

  for (int i = 0; i < N; ++i) {
    const QString pVal  = pick(powers, i, QStringLiteral("0"));
    const QString freq  = spicecompat::normalize_value(freqs.at(i));
    const QString phase = pick(phases, i, QStringLiteral("0"));
    const QString vamp  = resolveVamp(pVal, z0, spicecompat::SPICEXyce);

    const QString nodeAbove = intNode(i);
    const QString nodeBelow = (i == N - 1) ? nodeNeg : intNode(i + 1);

    // ACPHASE must be a numeric literal on Xyce PORT device;
    // for SIN, a parametric phase is wrapped in {...} to force expression evaluation.
    bool isPhaseNumeric = false;
    phase.toDouble(&isPhaseNumeric);
    const QString acPhase    = isPhaseNumeric ? phase : QStringLiteral("0");
    const QString transPhase = isPhaseNumeric ? phase : ('{' + phase + '}');

    s += QStringLiteral("V_%1_t%2 %3 %4 DC 0 AC %5 %6")
             .arg(Name).arg(i + 1)
             .arg(nodeAbove, nodeBelow, vamp, acPhase);
    if (enTran) {
      s += QStringLiteral(" SIN 0 %1 %2 0 0 %3").arg(vamp, freq, transPhase);
    }
    s += '\n';
  }

  s += '\n';

  return s;
}



QString Source_ac::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    if (dialect == spicecompat::SPICEXyce) {
        return xyce_netlist();
    } else {
        return ngspice_netlist();
    }
}

QString Source_ac::netlist()
{
  // Detect the type of simulation.
  // The AC power source definition is different for SP and TRAN. HB and AC (VAC + R)
  QString simType;
  if (Schematic *sch = dynamic_cast<Schematic*>(getSchematic())) {
    simType = sch->getSimType();
  }

  // In case of a SP simulation, use the Pac component
  if (simType == "SP") {
    QString s = Model + ":" + Name;

    for (Port *p1 : std::as_const(Ports)){
      s += " " + p1->Connection->Name;
    }

    for (int i = 0; i < Props.count(); i++) {
      const QString &name = Props.at(i)->Name;
      if (name == "EnableTran" || name == "Phase" || name == "LoadOnly")
        continue;
      s += " " + name + "=\"" + Props.at(i)->Value + "\"";
    }
    return s + '\n';
  }


  // For HB, TR: Thévenin equivalent
  const QStringList freqs  = parseList(getProperty("f")->Value);
  const QStringList powers = parseList(getProperty("P")->Value);
  const QStringList phases = parseList(getProperty("Phase")->Value);
  const QString z    = getProperty("Z")->Value;
  const QString temp = getProperty("Temp")->Value;
  const bool isTermination =
      (getProperty("LoadOnly")->Value == "true")
      || (powers.isEmpty() || powers.first().isEmpty());

  // Parse Z0
  double Z0, scale;
  QString unit;
  misc::str2num(z, Z0, unit, scale);
  Z0 *= scale;

  if (isTermination) {
    const QString nodePos = Ports.at(0)->Connection->Name;
    const QString nodeNeg = Ports.at(1)->Connection->Name;
    return QStringLiteral(
               "R:R_%1 %2 %3 "
               "R=\"%4 Ohm\" "
               "Temp=\"%5\"\n")
        .arg(Name, nodePos, nodeNeg,
             QString::number(Z0, 'g', 12),
             temp);
  }

  return multitone_qucsator(Z0, freqs, powers, phases, temp);
}

QString Source_ac::multitone_qucsator(double z0,
                                      const QStringList &freqs,
                                      const QStringList &powers,
                                      const QStringList &phases,
                                      const QString &temp)
{
  // Helper: pick list[i] if available, broadcast list[0] if single entry, else fallback.
  // This allows a single power/phase value to apply to all tones in a multitone setup.
  auto pick = [](const QStringList &list, int i, const QString &fallback) -> QString {
    if (list.isEmpty())   return fallback;
    if (list.size() == 1) return list.first();
    return (i < list.size()) ? list.at(i) : fallback;
  };

  const int N = freqs.size();
  const QString nodePos = Ports.at(0)->Connection->Name;
  const QString nodeNeg = Ports.at(1)->Connection->Name;

  // Internal junction nodes between the series voltage sources:
  // nodePos -- R -- n0 -- Vac_t1 -- n1 -- Vac_t2 -- ... -- nodeNeg
  auto intNode = [&](int k) {
    return QStringLiteral("%1_n%2").arg(Name).arg(k);
  };

  QString s;

  // Thévenin source impedance — one resistor shared by all tones.
  // Placed between the external positive node and the first internal junction.
  s += QStringLiteral("R:R_%1 %2 %3 R=\"%4 Ohm\" Temp=\"%5\"\n")
           .arg(Name, nodePos, intNode(0),
                QString::number(z0, 'g', 12), temp);

  // One ideal sinusoidal voltage source per tone
  // Adjust the AC voltage amplitude according to the power
  for (int i = 0; i < N; ++i) {
    const QString freq  = freqs.at(i);
    const QString power = pick(powers, i, QStringLiteral("0"));
    const QString phase = pick(phases, i, QStringLiteral("0"));

    // Series chain: tone i sits between intNode(i) and intNode(i+1),
    // except the last tone which connects directly to the negative external node.
    const QString nodeAbove = intNode(i);
    const QString nodeBelow = (i == N - 1) ? nodeNeg : intNode(i + 1);

    QString vampStr;
    bool ok = false;
    double p_dbm = spicecompat::normalize_value(power).toDouble(&ok);

    if (ok) {
      // Numeric power: compute vamp directly
      const double vrms = std::sqrt(z0 / 1000.0) * std::pow(10.0, p_dbm / 20.0);
      const double vamp = 2.0 * vrms * std::sqrt(2.0);
      vampStr = QString::number(vamp, 'g', 12) + " V";
    } else {
      // Non-numeric power: the value is a sweep parameter variable (e.g. "Pin").
      // QucsatorRF cannot evaluate inline expressions inside component property
      // strings, so it needs to emit a Eqn: block that computes the amplitude
      // symbolically. The equation is evaluated at each sweep iteration.
      //
      // Important: QucsatorRF passes sweep parameter values to the equation
      // evaluator already converted in W, despite the sweep is in dBm.
      // The amplitude formula is thus derived from P_watts directly:
      //   Vrms  = sqrt(Z0 * P_watts)
      //   Vpeak = sqrt(2) * Vrms
      //   Vamp  = 2 * Vpeak = 2*sqrt(2)*sqrt(Z0 * P_watts)
      const QString eqnBlockName = QStringLiteral("Eqn_vamp_%1_t%2").arg(Name).arg(i + 1);
      const QString eqnVarName   = QStringLiteral("vamp_%1_t%2").arg(Name).arg(i + 1);
      s += QStringLiteral("Eqn:%1 %2=\"2*sqrt(2)*sqrt(%3*%4)\" Export=\"yes\"\n")
               .arg(eqnBlockName, eqnVarName,
                    QString::number(z0, 'g', 12),
                    power);
      // Reference the variable name directly — no unit suffix when using a variable
      vampStr = eqnVarName;
    }

    s += QStringLiteral(
             "Vac:V_%1_t%2 %3 %4 "
             "U=\"%5\" "
             "f=\"%6\" "
             "Phase=\"%7\" "
             "Theta=\"0\" "
             "T=\"0\"\n")
             .arg(Name).arg(i + 1)
             .arg(nodeAbove, nodeBelow, vampStr, freq, phase);
  }

  return s;
}

QStringList Source_ac::parseList(const QString &raw) const
{
  // Strip surrounding brackets or braces if any, e.g. "{1 MHz, 2 MHz}" or "[1 MHz; 2 MHz]"
  QString cleaned = raw.trimmed();
  if ((cleaned.startsWith('{') && cleaned.endsWith('}')) ||
      (cleaned.startsWith('[') && cleaned.endsWith(']'))) {
    cleaned = cleaned.mid(1, cleaned.length() - 2).trimmed();
  }

  QStringList parts = cleaned.split(QRegularExpression("[,;]"));
  for (QString &s : parts){
    s = s.trimmed();
  }

  // Drop any trailing empty entry produced by a trailing delimiter,
   // e.g. "980 MHz, 990 MHz," would otherwise produce a spurious third empty entry
  while (!parts.isEmpty() && parts.last().isEmpty()) {
    parts.removeLast();
  }

  return parts;
}

QString Source_ac::resolveVamp(const QString &pVal, double z0,
                               spicecompat::SpiceDialect dialect) const
{
  if (pVal.isEmpty())
    return QStringLiteral("0");

  bool isNumeric = false;
  double p = spicecompat::normalize_value(pVal).toDouble(&isNumeric);
  if (isNumeric) {
    double vrms = sqrt(z0 / 1000.0) * pow(10.0, p / 20.0);
    return QString::number(2.0 * vrms * sqrt(2.0), 'g', 8);
  }

  if (dialect == spicecompat::SPICEXyce) {
    // Xyce
    return QStringLiteral("{2*sqrt(2)*sqrt(%1/1000)*pow(10,(%2)/20)}")
        .arg(z0).arg(pVal);
  } else {
    // ngspice
    return QStringLiteral("'2*sqrt(2)*sqrt(%1/1000)*pow(10,(%2)/20)'")
        .arg(z0).arg(pVal);
  }
}
