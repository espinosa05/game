#ifndef __SAMPLE_H__
#define __SAMPLE_H__

#include <be/be_engine.h>
#include <be/be_layer.h>
#include <core/vulkan.h>

typedef struct {
} SampleData;

SampleData *sample_on_attach(BeEngine *be);
void sample_on_detach(BeEngine *be, SampleData *sd);
void sample_on_suspend(BeEngine *be, SampleData *sd);
void sample_on_activate(BeEngine *be, SampleData *sd);
void sample_on_event(BeEngine *be, SampleData *sd);

#endif /* __SAMPLE_H__ */
