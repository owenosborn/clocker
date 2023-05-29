#include "m_pd.h"  

static t_class *clockfill_class;  
 
typedef struct _clockfill {  
    t_object  x_obj; 
    t_int last;         // previous tick
    t_int first_input;  // for the very frist input
} t_clockfill;  

void clockfill_float(t_clockfill *x, t_floatarg input_phase) {   
    
    t_int in = input_phase;
    t_int diff = 0;
    t_int diff2 = 0;
    t_int i = 0;
    
    // only do anything for positive numbers
    if (in < 0) return;

    if (x->first_input){
        x->first_input = 0;
        // if first input a 0, let it through 
        if (in == 0) outlet_float(x->x_obj.ob_outlet, in);
    }

    if (in != x->last) {
        diff = in - x->last;
        // if a number was skipped, fill in missing
        if (diff > 1) {
            // don't send out more than 8 ticks at once (this would be faster than like 500 bpm),
            if (diff < 8) {
                for (i = 1; i < (diff + 1); i++) {
                    outlet_float(x->x_obj.ob_outlet, x->last + i);
                }
            }
        }
        // rolled over, fill in missing before and after 359
        else if (diff < 0 && diff != -359){ 
            diff2 = (360 - x->last) + in;
            // don't send out more than 8 ticks at once (this would be faster than 500 bpm)
            if (diff2 < 8) {
                for (i = 1; i < (diff2 + 1); i++) {
                    outlet_float(x->x_obj.ob_outlet, (x->last + i) % 360);
                }
            }
            // if it is 0, and more than 8 away from last, we assume phase was reset, so  output 0
            else if (in == 0){
                outlet_float(x->x_obj.ob_outlet, 0);
            }
        }
        // otherwise it was consecutive
        else {
            outlet_float(x->x_obj.ob_outlet, in);
        }
        // update for next time
        x->last = in;
    }
}

void *clockfill_new(void) {  
    t_clockfill *x = (t_clockfill *)pd_new(clockfill_class);  
    
    x->last = 0;
    x->first_input = 1;
    outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;  
}  

void clockfill_setup(void) {  
    clockfill_class = class_new(gensym("clockfill"), (t_newmethod)clockfill_new, 0, sizeof(t_clockfill), CLASS_DEFAULT, 0);  
	class_addfloat(clockfill_class, clockfill_float);  
}
