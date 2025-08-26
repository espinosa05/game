
/* symbols for Grep */
#define CDSTRUCT_ARRAY_SIZE(a) sizeof(a)/sizeof(*a)
#define DECL_CSTRUCT(p) void p ()
#define DECL_DSTRUCT(p) void p ()
#define CSTRUCT_ENTRY(e)   e
#define DSTRUCT_ENTRY(e)   e

#define CSTRUCT()                                               \
    do {                                                        \
        for (int i = 0; i < CDSTRUCT_ARRAY_SIZE(cstruct); ++i)  \
            cstruct[i]();                                       \
    } while (0)

#define DSTRUCT()                                               \
    do {                                                        \
        for (int i = 0; i < CDSTRUCT_ARRAY_SIZE(cstruct); ++i)  \
            dstruct[i]();                                       \
    } while (0)


typedef void (*CStructFunc)(void);
typedef void (*DStructFunc)(void);


/* add your constructor function symbols here */
DECL_CSTRUCT(DummyConstructor);
DECL_CSTRUCT(InitGlobalArena);
/* add your destructor function symbols here */
DECL_DSTRUCT(DummyDestructor);

/* add your constructors to the list */
CStructFunc cstruct[] = {
    CSTRUCT_ENTRY(DummyConstructor),
    CSTRUCT_ENTRY(InitGlobalAerna),
};

/* add your destructors to the list */
DStructFunc dstruct[] = {
    DSTRUCT_ENTRY(DummyDestructor),
};

void DummyConstructor(void) { return; }
void DummyDestructor(void) { return; }
