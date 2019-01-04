#include "m_pd.h"  

static t_class *clocker_class;  
 
typedef struct _clocker {  
    t_object  x_obj; 
    t_int length;
    t_int tick_count;
    t_int start_flag;
    t_int start_sync_flag;
    t_int loop;
    t_int output_enable;
} t_clocker;  

void clocker_stop(t_clocker *x) {  
    x->output_enable = 0;	
}

void clocker_start_sync(t_clocker *x) {  
    x->start_sync_flag = 1;
}

void clocker_start(t_clocker *x) {  
    x->start_flag = 1;
}

void clocker_float(t_clocker *x, t_floatarg input_phase) {  
    
    t_int i_input_phase = input_phase;
    
    // start now flag 
    if (x->start_flag) {
        x->start_flag = 0;
        x->tick_count = 0; 
        x->output_enable = 1;
    }
    // start on top of beat
    if (x->start_sync_flag) {
        if (i_input_phase == 0) {
            x->start_sync_flag = 0;
            x->tick_count = 0;
            x->output_enable = 1;
        }
    }
    // loop it
    if (x->output_enable) outlet_float(x->x_obj.ob_outlet, x->tick_count);
    x->tick_count++;
    if (x->tick_count >= x->length) {
        x->tick_count = 0;
        if (!x->loop) x->output_enable = 0;
    }
}

void clocker_length(t_clocker *x, t_floatarg len)
{
    x->length = len;
}

void clocker_loop(t_clocker *x, t_floatarg loop)
{
    x->loop = loop;
}

void *clocker_new(void) {  
    t_clocker *x = (t_clocker *)pd_new(clocker_class);  

    x->start_flag = 0;
    x->start_sync_flag = 0;
    x->length = 360;
    x->tick_count = 0;
    x->output_enable = 0;
    x->loop = 1;

    outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;  
}  

void clocker_setup(void) {  
    clocker_class = class_new(gensym("clocker"), (t_newmethod)clocker_new, 0, sizeof(t_clocker), CLASS_DEFAULT, 0);  
	class_addfloat(clocker_class, clocker_float);  
	class_addmethod(clocker_class, (t_method)clocker_stop, gensym("stop"), 0);  
	class_addmethod(clocker_class, (t_method)clocker_start, gensym("start"), 0);  
	class_addmethod(clocker_class, (t_method)clocker_start_sync, gensym("start-sync"), 0);  
    class_addmethod(clocker_class, (t_method)clocker_length, gensym("length"), A_DEFFLOAT, 0);
    class_addmethod(clocker_class, (t_method)clocker_loop, gensym("loop"), A_DEFFLOAT, 0);
}
