
// static void setup_8_bit(GPIO_TypeDef* dp,GPIO_TypeDef* cp, uint32_t data){
//
//     PinWrite(dp, data);
//     PinWrite(cp, E_PIN);
//     for(volatile int i = 0; i < 6; i++);
//     ResetPins(cp, E_PIN);
//     ResetPins(dp, 0xFF);
//     for(volatile int i = 0; i < 6; i++);
// }

// void lcd_init(GPIO_TypeDef* dp, GPIO_TypeDef* cp, uint8_t pins, uint8_t bit_mode, uint8_t offset){
//     switch(bit_mode){
//         case BIT_MODE_4:
//             SetPinOutput(dp, pins << offset);
//             SetOutputType(cp, RW_PIN, 1);
//             SetPinPD(dp, pins << offset);
//             SetPinPU(cp,RW_PIN);
//             setup_4_bit(dp, cp, pins << offset);
//             break;
//
//         case BIT_MODE_8:
//             // port init
//             SetPinOutput(dp, pins << offset);
//             SetOutputType(cp,RW_PIN,1);
//             SetPinPD(dp, pins << offset);
//             SetPinPU(cp,RW_PIN);
//
//             // initialization
//             setup_8_bit(dp, cp, FUNC_SET << offset);
//             setup_8_bit(dp, cp, DISP_SET << offset);
//             setup_8_bit(dp, cp, CLR_LCD << offset);
//             for(volatile int i = 0; i < 1818; i++);
//             break;
//     }
// }
