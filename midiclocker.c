#include "m_pd.h"  

// takes midi clock in 0 - 23 and outputs 0 - 359 beat clock (15 clocks per midi clock)

static t_class *midiclocker_class;  

typedef struct _midiclocker {  
    t_object  x_obj; 
} t_midiclocker;  

void midiclocker_float(t_midiclocker *x, t_floatarg in) {   

    int i;

    for (i = 0; i< 15; i++) {
        outlet_float(x->x_obj.ob_outlet, i + (int)in * 15);
    }
}

void *midiclocker_new(void) {  
    t_midiclocker *x = (t_midiclocker *)pd_new(midiclocker_class);  
    
    outlet_new(&x->x_obj, &s_float);
    
    return (void *)x;  
}  

void midiclocker_setup(void) {  
    midiclocker_class = class_new(gensym("midiclocker"), (t_newmethod)midiclocker_new, 0, sizeof(t_midiclocker), CLASS_DEFAULT, 0);  
	class_addfloat(midiclocker_class, midiclocker_float);  
}
