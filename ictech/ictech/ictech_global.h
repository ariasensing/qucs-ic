#ifndef ICTECH_GLOBAL_H
#define ICTECH_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(ICTECH_LIBRARY)
#define ICTECH_EXPORT Q_DECL_EXPORT
#else
#define ICTECH_EXPORT Q_DECL_IMPORT
#endif

#endif // ICTECH_GLOBAL_H
