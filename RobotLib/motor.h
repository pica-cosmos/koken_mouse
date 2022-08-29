#ifndef MOTOR_H_
#define MOTOR_H_

    void motor_init(void);
    void motor_on(int R_dir, int L_dir, int R_duty, int L_duty);
    void motor_off(void);
    
#endif