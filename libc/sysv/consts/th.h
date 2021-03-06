#ifndef COSMOPOLITAN_LIBC_SYSV_CONSTS_TH_H_
#define COSMOPOLITAN_LIBC_SYSV_CONSTS_TH_H_
#include "libc/runtime/symbolic.h"

#define TH_ACK SYMBOLIC(TH_ACK)
#define TH_FIN SYMBOLIC(TH_FIN)
#define TH_PUSH SYMBOLIC(TH_PUSH)
#define TH_RST SYMBOLIC(TH_RST)
#define TH_SYN SYMBOLIC(TH_SYN)
#define TH_URG SYMBOLIC(TH_URG)

#if !(__ASSEMBLER__ + __LINKER__ + 0)
COSMOPOLITAN_C_START_

extern const long TH_ACK;
extern const long TH_FIN;
extern const long TH_PUSH;
extern const long TH_RST;
extern const long TH_SYN;
extern const long TH_URG;

COSMOPOLITAN_C_END_
#endif /* !(__ASSEMBLER__ + __LINKER__ + 0) */
#endif /* COSMOPOLITAN_LIBC_SYSV_CONSTS_TH_H_ */
