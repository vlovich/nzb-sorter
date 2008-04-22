#ifndef QOUT_H_
#define QOUT_H_

#include <QTextStream>
#include <cstdio>

namespace nzb
{
static QTextStream qout(stdout);
static QTextStream qerr(stderr);
}


#undef EXTERN

#endif /* QOUT_H_ */
