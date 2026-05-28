/***************************************************************************
                          hb_sim.cpp  -  description
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
#include "hb_sim.h"
#include "extsimkernels/spicecompat.h"
#include <QRegularExpression>


HB_Sim::HB_Sim()
{
  Description = QObject::tr("Harmonic balance simulation");
  Simulator = spicecompat::simXyce | spicecompat::simQucsator;

  QString  s = Description;
  initSymbol(Description);
  Model = ".HB";
  Name  = "HB";
  SpiceModel = ".HB";
  isSimulation = true;

  Props.append(new Property("f", "1 GHz", false,
		QObject::tr("frequency in Hertz")));
  Props.append(new Property("n", "4", true,
		QObject::tr("number of harmonics")));
  Props.append(new Property("iabstol", "1 pA", false,
		QObject::tr("absolute tolerance for currents")));
  Props.append(new Property("vabstol", "1 uV", false,
		QObject::tr("absolute tolerance for voltages")));
  Props.append(new Property("reltol", "0.001", false,
		QObject::tr("relative tolerance for convergence")));
  Props.append(new Property("MaxIter", "150", false,
		QObject::tr("maximum number of iterations until error")));
}

HB_Sim::~HB_Sim()
{
}

Component* HB_Sim::newOne()
{
  return new HB_Sim();
}

Element* HB_Sim::info(QString& Name, char* &BitmapFile, bool getNewOne)
{
  Name = QObject::tr("Harmonic balance");
  BitmapFile = (char *) "hb";

  if(getNewOne)  return new HB_Sim();
  return 0;
}

QString HB_Sim::spice_netlist(spicecompat::SpiceDialect dialect /* = spicecompat::SPICEDefault */)
{
    QString s="";
    if (dialect == spicecompat::SPICEXyce) {  // Only in Xyce
        // Get frequency list from the properties
        QStringList freqs = Props.at(0)->Value.split(QRegularExpression("\\s*[,;]\\s*|\\s+(?=[0-9])"), Qt::SkipEmptyParts);

        // Build the NUMFREQ QString for N fundamental frequencies
        QString NUMFREQ = Props.at(1)->Value; // Number of harmonics to be calculated for each tone
        QString numfreqs = NUMFREQ;
        int N = freqs.count(); // Number of fundamental frequencies
        if (N > 1){
          // Multitone input
          for (int i = 0; i < N-1; ++i) {
            numfreqs.append(QString(",%1").arg(NUMFREQ));
          }
        }

        // split frequencyes list by space before digit
        for (QStringList::iterator it = freqs.begin();it != freqs.end(); it++) {
            (*it) = spicecompat::normalize_value(*it);
        }
        s += QStringLiteral(".HB %1\n").arg(freqs.join(" "));
        s += QStringLiteral(".options hbint numfreq=%1 STARTUPPERIODS=2\n").arg(numfreqs);
    }
    return s;
}

QString HB_Sim::netlist()
{
  QString fVal = Props.at(0)->Value.trimmed();

  // qucsator reads tone frequencies directly from PAC excitation components.
  // Its f property is PROP_REAL and cannot parse a comma-separated list —
  // omit it when multiple tones are specified; the PAC chain handles it.
  bool isMultitone = fVal.contains(',') || fVal.contains(';');

  QString s = Model + ":" + Name;
  for (int i = 0; i < Props.count() - 1; i++) {
    const Property *p = Props.at(i);
    if (p->Name == "f" && isMultitone)
      continue;
    s += " " + p->Name + "=\"" + p->Value + "\"";
  }
  return s + '\n';
}
