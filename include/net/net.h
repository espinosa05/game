#ifndef __NET_H__
#define __NET_H__

#include <core/types.h>
#include <core/strings.h>
#include <core/error_report.h>

#define INET_PORT_STRLEN 5 + NULL_TERM_SIZE
#define INET_ADDR_STRLEN 16 + NULL_TERM_SIZE

u32 NET_StrToInetAddr(char *s);
u16 NET_StrToInetPort(char *s);

u32 NET_StrToInetAddrER(char *s, ErrorReport *er);
u16 NET_StrToInetPortER(char *s, ErrorReport *er);
char *NET_InetPortToStr(u16 port, char buff[INET_PORT_STRLEN]);
char *NET_InetAddrToStr(u32 addr, char buff[INET_ADDR_STRLEN]);

#endif /* __NET_H__ */
