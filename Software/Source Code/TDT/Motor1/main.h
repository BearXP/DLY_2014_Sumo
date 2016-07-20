volatile uint8_t previous_port_j;  //records previous state to find which pin changed
volatile uint8_t interrupt_edge_port_j;  //defines the mask to indicate which edge to trigger on. 0=falling, 1=rising

