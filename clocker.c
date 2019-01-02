#include "m_pd.h"  

static t_class *clocker_class;  
 
typedef struct _clocker {  
    t_object  x_obj; 
    t_int length;
    t_int divisor;
    t_int start_flag;
    t_int start_sync_flag;
    t_int cycle_count;
    t_float f_length;
    
    t_float offset;
    t_float offset_on_start;
    t_float beat_count;
    t_float phase_prev;
    t_float input_phase_prev;
} t_clocker;  

void clocker_stop(t_clocker *x) {  
	
}

void clocker_start_sync(t_clocker *x) {  
    x->start_sync_flag = 1;
}

void clocker_start(t_clocker *x) {  
    x->start_flag = 1;
}

void clocker_float(t_clocker *x, t_floatarg input_phase) {  
    
    // start now flag 
    if (x->start_flag) {
        x->start_flag = 0;
        x->offset = input_phase;
        x->offset_on_start = input_phase;
        x->beat_count = 0;
        x->phase_prev = 0;
        x->cycle_count = 0;
    }

    // start on top of beat
    if (x->start_sync_flag) {
        if (x->input_phase_prev > input_phase) {
            x->start_sync_flag = 0;
            x->offset = input_phase;
            x->offset_on_start = input_phase;
            x->beat_count = 0;
            x->phase_prev = 0;
            x->cycle_count = 0;
        }
    }

    t_float phase = input_phase - x->offset;
    phase = phase - floor(phase);
    
    // check for rollover
    if (x->phase_prev > phase) {
        x->beat_count += 1;
    }

    // check length
    if ((phase + x->beat_count) >= (x->f_length - .001)) {
        x->offset = input_phase;
        //post("rolled with offest: %f", x->offset);
        phase = 0;
        x->beat_count = 0;
        // after divisor number of loops, the clock should be in phase with input,
        // so force it to be so
        x->cycle_count++;
        if (x->cycle_count == x->divisor){
           x->offset = x->offset_on_start;
           x->cycle_count = 0;
        }
    }
    
    x->phase_prev = phase;
    x->input_phase_prev = input_phase; 
    
    outlet_float(x->x_obj.ob_outlet, phase + x->beat_count);
}

void clocker_length(t_clocker *x, t_floatarg f1, t_floatarg f2)
{
    x->length = f1;
    x->divisor = f2;
    x->f_length = f1 / f2;
    post("length %f / %f = %f", f1, f2, x->f_length);
}

void *clocker_new(void) {  
    t_clocker *x = (t_clocker *)pd_new(clocker_class);  
    

    x->offset = 0;
    x->offset_on_start = 0;
    x->cycle_count = 0;
    x->beat_count = 0;
    x->phase_prev = 0;
    x->input_phase_prev = 0;

    x->start_flag = 0;
    x->start_sync_flag = 0;
    x->length = 1;
    x->divisor = 1;
    x->f_length = 1;
    
    outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;  
}  

void clocker_setup(void) {  
    clocker_class = class_new(gensym("clocker"), (t_newmethod)clocker_new, 0, sizeof(t_clocker), CLASS_DEFAULT, 0);  
	class_addfloat(clocker_class, clocker_float);  
	class_addmethod(clocker_class, (t_method)clocker_stop, gensym("stop"), 0);  
	class_addmethod(clocker_class, (t_method)clocker_start, gensym("start"), 0);  
	class_addmethod(clocker_class, (t_method)clocker_start_sync, gensym("start-sync"), 0);  
    class_addmethod(clocker_class, (t_method)clocker_length, gensym("length"), A_DEFFLOAT, A_DEFFLOAT, 0);
}
