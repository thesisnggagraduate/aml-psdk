#ifndef __AML_PSDK_RWOBJECT_H
#define __AML_PSDK_RWOBJECT_H

#include <aml-psdk/sdk_base.h>
#include "RwLinks.h"
#include "RwBase.h"

#define rpATOMIC 1
#define rpCLUMP 2
#define rpLIGHT 3
#define rpWORLD 7
#define rpGEOMETRY 8

struct RwFrame;
struct RwObjectHasFrame;

struct RwObject
{
    RwUInt8  type;
    RwUInt8  subType;
    RwUInt8  flags;
    RwUInt8  privateFlags;
    union
    {
        void    *parent;
        RwFrame *frame; // Mostly (or always) RwFrame*
    };
};

typedef RwObject *(*RwObjectCallBack)(RwObject *object, void *data);
typedef RwObjectHasFrame * (*RwObjectHasFrameSyncFunction)(RwObjectHasFrame *object);

struct RwObjectHasFrame
{
    RwObject                     object;
    RwLLLink                     lFrame;
    RwObjectHasFrameSyncFunction sync;
};

DECL_FASTCALL_SIMPLE_GLO(_rwObjectHasFrameSetFrame, _Z25_rwObjectHasFrameSetFramePvP7RwFrame, void, void* object, RwFrame* frame);
DECL_FASTCALL_SIMPLE_GLO(_rwObjectHasFrameReleaseFrame, _Z29_rwObjectHasFrameReleaseFramePv, void, RwFrame* object);

#endif // __AML_PSDK_RWOBJECT_H