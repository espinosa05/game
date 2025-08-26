#ifndef __XML_XML_H__
#define __XML_XML_H__

#include <core/types.h>
#include <core/os.h>

typedef struct {
} XML_Context;

typedef struct {
    usz inputType;
    OS_Stream inputStream;
} XML_OpenInfo;

XML_Status XML_Init(XML_Context *xml);
XML_Status XML_Open(XML_Context *xml, XML_OpenInfo *info);
XML_Status XML_Destroy(XML_Context *xml);

#endif /* __XML_XML_H__ */
