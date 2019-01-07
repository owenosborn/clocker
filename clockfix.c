#include "m_pd.h"  

static t_class *clockfix_class;  
 
typedef struct _clockfix {  
    t_object  x_obj; 
    t_int last;         // previous tick
    t_int first_input;  // for the very frist input
} t_clockfix;  

void clockfix_float(t_clockfix *x, t_floatarg input_phase) {   
    
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
            // don't send out more than 20 ticks at once
            if (diff > 20) diff = 20;  
            for (i = 1; i < (diff + 1); i++) {
                outlet_float(x->x_obj.ob_outlet, x->last + i);
            }
        }
        // rolled over, fill in missing before and after 359
        else if (diff < 0 && diff != -359){ 
            diff2 = (360 - x->last) + in;
            // don't send out more than 20 ticks at once
            if (diff2 > 20) diff2 = 20;
            for (i = 1; i < (diff2 + 1); i++) {
                outlet_float(x->x_obj.ob_outlet, (x->last + i) % 360);
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

void *clockfix_new(void) {  
    t_clockfix *x = (t_clockfix *)pd_new(clockfix_class);  
    
    x->last = 0;
    x->first_input = 1;
    outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;  
}  

void clockfix_setup(void) {  
    clockfix_class = class_new(gensym("clockfix"), (t_newmethod)clockfix_new, 0, sizeof(t_clockfix), CLASS_DEFAULT, 0);  
	class_addfloat(clockfix_class, clockfix_float);  
}
