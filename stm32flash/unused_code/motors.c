
int lateral_left(void* args){

    motor_tcb = current_tcb;
    uint8_t target = *((uint8_t*)args);
    motor_timeout_counter = 0;
    
    // turn_off_motors();
    // zero_counters();

    GPIOC->BSRR |= INL2|INL4;
    GPIOB->BSRR |= INR1|INR4;    
    while(1){

        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return 0;
        }

        curr2 = TIM2->CNT;
        curr3 = abs(TIM3->CNT);
        curr4 = TIM4->CNT;
        curr8 = abs(TIM8->CNT);
        
        error2+= (target-curr2)/lateral_Ki;
        error3+= (target-curr3)/lateral_Ki;
        error4+= (target-curr4)/lateral_Ki;
        error8+= (target-curr8)/lateral_Ki;

        int16_t output2 = lateral_Kd*(curr2-prev2) + lateral_Kp*(target - curr2) + (error2);
        int16_t output3 = lateral_Kd*(curr3-prev3) + lateral_Kp*(target - curr3) + (error3);
        int16_t output4 = lateral_Kd*(curr4-prev4) + lateral_Kp*(target - curr4) + (error4);
        int16_t output8 = lateral_Kd*(curr8-prev8) + lateral_Kp*(target - curr8) + (error8);

        if(target - curr2 < 0){
            zero_CCR();
            turn_off_motors();
            return 1;
        } else {
            TIM1->CCR1 = output2;
            TIM1->CCR2 = output3;
            TIM1->CCR3 = output4;
            TIM1->CCR4 = output8;
        }
        
        prev2 = curr2;
        prev3 = curr3;
        prev4 = curr4;
        prev8 = curr8;
        block();
        motor_timeout_counter++;
    }   
}

int lateral_right(void* args){

    motor_tcb = current_tcb;
    uint8_t data = *((uint8_t*)args);
    
    int16_t target = data * 48;

    GPIOA->BSRR |= INR2;
    GPIOC->BSRR |= INL1|INL3|INR3;    
    
    while(1){

        if(motor_timeout_counter > MAXTIMEOUT){
            zero_CCR();
            turn_off_motors();
            return 0;
        }
        curr2 = abs(TIM2->CNT);
        curr3 = TIM3->CNT;
        curr4 = abs(TIM4->CNT);
        curr8 = TIM8->CNT;
        
        error2+= (target-curr2)/lateral_Ki;
        error3+= (target-curr3)/lateral_Ki;
        error4+= (target-curr4)/lateral_Ki;
        error8+= (target-curr8)/lateral_Ki;

        int16_t output2 = lateral_Kd*(curr2-prev2) + lateral_Kp*(target - curr2) + (error2);
        int16_t output3 = lateral_Kd*(curr3-prev3) + lateral_Kp*(target - curr3) + (error3);
        int16_t output4 = lateral_Kd*(curr4-prev4) + lateral_Kp*(target - curr4) + (error4);
        int16_t output8 = lateral_Kd*(curr8-prev8) + lateral_Kp*(target - curr8) + (error8);

        if(target - curr2 < 0){
            TIM1->CCR1 = 0;
            TIM1->CCR2 = 0;
            TIM1->CCR3 = 0;
            TIM1->CCR4 = 0;
            zero_CCR();
            turn_off_motors();
            return 1;
        }else {
            TIM1->CCR1 = output2;
            TIM1->CCR2 = output3;
            TIM1->CCR3 = output4;
            TIM1->CCR4 = output8;
        }
        
        prev2 = curr2;
        prev3 = curr3;
        prev4 = curr4;
        prev8 = curr8;
        block();
        motor_timeout_counter++;
    }

}
