#ifndef KPF5_GLOBAL_H
#define KPF5_GLOBAL_H

#include <QtCore/qglobal.h>

#if defined(KPF5_LIBRARY)
#  define KPF5SHARED_EXPORT Q_DECL_EXPORT
#else
#  define KPF5SHARED_EXPORT Q_DECL_IMPORT
#endif

#endif // KPF5_GLOBAL_H
