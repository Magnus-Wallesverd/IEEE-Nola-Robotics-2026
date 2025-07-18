/* struct for motor data */
typedef struct{
    uint32_t encoder_data;
    uint32_t pwm_data;
    uint32_t error_data;
}lcd_motor_data_t;

typedef struct {
    uint32_t tag;
    void*    data_ptr;
    uint32_t timestamp;
}lcd_item_t;
