
void set_speed(uint16_t target){
    
    int16_t curr2 = TIM2->CNT;
    int16_t curr3 = TIM3->CNT;
    int16_t curr4 = TIM4->CNT;
    int16_t curr8 = TIM8->CNT;

    measure2 = curr2 - prev2;
    measure3 = curr3 - prev3;
    //measure4 = curr4 - prev4;
    //measure8 = curr8 - prev8;

    prev2  = curr2; 
    prev3  = curr3;
    //prev4  = curr4;
    //prev8  = curr8;
    
    error2 = target - measure2;
    error3 = target - measure3;
    //error4 = target - measure4;
    //error8 = target - measure8;
    
    TIM1->CCR1 += (Kp*error2) * !(measure2 < -200 ||measure2 > 400);
    //TIM1->CCR2 += (Kp*error8) * !(measure8 < -200 ||measure8 > 400);
    //TIM1->CCR3 += Kp*error4   * !(measure4 < -200 ||measure4 > 200);
    //TIM1->CCR4 += Kp*error3   * !(measure3 < -200 ||measure3 > 200);
}

void correct_heading(int16_t target_h){
    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = curr_h - prev_h;
    if(measure_h <= HEADING_MAX_VALUE/2) measure_h+=HEADING_MAX_VALUE;
    if(measure_h > HEADING_MAX_VALUE/2) measure_h-=HEADING_MAX_VALUE;
    
    error_h = measure_h;

    if(abs(error_h) < 16){
        return;
    } else if(error_h < 0){
        TIM1->CCR2 += Kp_h*error_h;
        TIM1->CCR3 += Kp_h*error_h;
    } else {
        TIM1->CCR1 += Kp_h*error_h;
        TIM1->CCR4 += Kp_h*error_h;
    }
    prev_h = curr_h;
}

// R = ch1,4 & L = ch 2,3 
void heading_correction(int16_t target_h){
    curr_h = (i2c_rx_buffer[HEADING_MSB] << 8 | i2c_rx_buffer[HEADING_LSB]);
    measure_h = curr_h - prev_h;
    if((!measure_h) & bno_flag){
        return;
    }
    if(curr_h > HEADING_MAX_VALUE - 160 && prev_h < 160 ){
        //undf
        // decrement right side wheels
        error_h = target_h - (HEADING_MAX_VALUE - measure_h);
        TIM1->CCR1 += Kp_h*error_h;
        TIM1->CCR4 += Kp_h*error_h;
        
    } else if(curr_h < 160 && prev_h > HEADING_MAX_VALUE - 160){
        //ovf
        // decrement left side wheels
        error_h = target_h - twos_compl16(HEADING_MAX_VALUE + measure_h);
        TIM1->CCR2 += Kp_h*error_h;
        TIM1->CCR3 += Kp_h*error_h;
    } else {
        error_h = target_h - measure_h;
        if(measure_h < 0){
            // error_h is positive
            // decrement right side wheels
            error_h*=-1;
            TIM1->CCR1 += Kp_h*error_h;
            TIM1->CCR4 += Kp_h*error_h;
        } else {
            // decrement left side wheels
            // error_h is negative
            Kp_h*error_h;
            TIM1->CCR2 += Kp_h*error_h;
            TIM1->CCR3 += Kp_h*error_h;
        }
    }
    prev_h = curr_h;
}

void servo(void* args){
    (void) args;
    while(1){
        // TIM16->CCR1 |= 14000; 
        // TIM16->CCR1 = TIM16->ARR - get_global_tick() % TIM16->ARR;
    }
}
