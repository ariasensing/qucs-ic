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
      // Single-tone setup — extract scalar values from the first index in the list
      const QString pVal  = powers.isEmpty() ? QString() : powers.first();
      const QString f     = freqs.isEmpty()  ? QString() : spicecompat::normalize_value(freqs.first());
      const QString phase = phases.isEmpty() ? QStringLiteral("0") : phases.first();

      const QString vamp = resolveVamp(pVal, z0, spicecompat::SPICEDefault);

      if (isTermination) {
        s += QStringLiteral(" dc 0 ac 0");
      } else {
        s += QStringLiteral(" dc 0 ac %1").arg(vamp);
        if (en_tran)
          s += QStringLiteral(" SIN(0 %1 %2 0 0 %3)").arg(vamp, f, phase);
    }

      s += QStringLiteral(" portnum %1").arg(getProperty("Num")->Value);
      s += QStringLiteral(" z0 %1").arg(z0);
      s += "\n";
      return s;

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
    QString pVal = getProperty("P")->Value.trimmed();
    QString f = spicecompat::normalize_value(getProperty("f")->Value);
    QString phase = getProperty("Phase")->Value.trimmed();

    bool en_tran = true;
    if (getProperty("EnableTran")->Value == "true") {
      en_tran = true;
    } else {
      en_tran = false;
    }

    // Calculate the power
    // The power may come explicitly in the "Power" field of the component (literal value) or be part of a sweep simulation (symbolic variable).
    // Check if P is a symbolic parameter (not a numeric dBm literal)
    bool isNumeric = false;
    double p = spicecompat::normalize_value(pVal).toDouble(&isNumeric);

    // Check if phase is numeric
    bool isPhaseNumeric = false;
    phase.toDouble(&isPhaseNumeric);

    // if user has explicitly set LoadOnly OR
    // if P is empty (unset), the port acts as a terminated port (a passive load).
    bool isTermination = (getProperty("LoadOnly")->Value == "true") || pVal.isEmpty();

    // Calculate voltage amplitude
    QString vamp;
    if (isTermination) {
      // Terminated port: set Vamp to 0
      vamp = QString::number(0);
    } else if (isNumeric) {
      // Fixed value (not part of a parametric simulation)
      double vrms = sqrt(z0 / 1000.0) * pow(10.0, p / 20.0);
      double vamp_val = 2.0 * vrms * sqrt(2.0);
      vamp = QString::number(vamp_val, 'g', 8);
    } else {
      // P is a symbolic variable (.PARAM)
      // The dBm to V is embedded directly in the netlist line of the AC power source
      // This is evaluated at each step
      vamp = QStringLiteral("{2*sqrt(2)*sqrt(%1/1000)*pow(10,(%2)/20)}")
                      .arg(z0).arg(pVal);
    }

    // Build netlist line
    s += QStringLiteral(" z0=%1 ").arg(s_z0);

    // AC phase (must be numeric for Xyce)
    const QString acPhase = isPhaseNumeric ? phase : "0";
    s += QStringLiteral(" AC %1 %2 ").arg(vamp, acPhase);

    // Transient analysis SIN source
    if (en_tran && !isTermination) {
      const QString transPhase = isPhaseNumeric ? phase : ("{" + phase + "}");
      s += QStringLiteral(" SIN 0 %1 %2 0 0 %3").arg(vamp, f, transPhase);
    }

    s += "\n";
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
    QString s = Model+":"+Name;

    // output all node names
    for (Port *p1 : std::as_const(Ports))
      s += " "+p1->Connection->Name;   // node names

    // output all properties
    for(int i=0; i <= Props.count()-2; i++)
      if(Props.at(i)->Name != "EnableTran")
        s += " "+Props.at(i)->Name+"=\""+Props.at(i)->Value+"\"";

    return s + '\n';
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
