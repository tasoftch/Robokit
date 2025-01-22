/*
* MIT License
 *
 * Copyright (c) 2024 Th. Abplanalp, F. Romer
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 */

#ifndef PWM_MOTORS_H
#define PWM_MOTORS_H

#include <config.h>

#define PWM_FREQ       20000    // PWM-Frequency in Hz
#define PWM_RESOLUTION LEDC_TIMER_8_BIT // Resolution of timer
#define PWM_DUTY_CYCLE 64       // Default duty cycle

#define LEDC_TIMER      LEDC_TIMER_1
#define LEDC_MODE       LEDC_LOW_SPEED_MODE

// Motor 1 covers USB port.
// If used, no debug information will be sent to console.
// idf.py monitor will not work anymore.
#define ROBOKIT_USE_MOTOR_1 0

typedef enum {
#if ROBOKIT_USE_MOTOR_1
	E_DRIVE_MOTOR_1,
#endif
	E_DRIVE_MOTOR_2,
	E_DRIVE_MOTOR_3,
	E_DRIVE_MOTOR_4
} E_motor;

enum {
	E_DRIVE_MOTOR_FORWARD_FLAG			= 1<<0,
	E_DRIVE_MOTOR_BACKWARD_FLAG			= 1<<1,
	E_DRIVE_MOTOR_DR_HIGH_FLAG			= 1<<2,
	E_DRIVE_MOTOR_DR_LOW_FLAG			= 1<<3,
	E_DRIVE_MOTOR_SWITCH_CONNECTORS		= 1<<4,
};

/**
	* @brief Turns off all PWM motors.
	*
	* This function deactivates all PWM motor outputs, ensuring that no motors are
	* driven. It sequentially calls the motor off function for each motor
	* defined within the system.
	*
	* @return void
	*/
void robokit_pwm_motor_all_off(void);

/**
	* @brief Activates a specified PWM motor with full speed.
	*
	* This function turns on a specified motor by setting its speed to maximum. The direction and
	* behavior of the motor are controlled by the provided flags, which determine how the PWM
	* signals are configured.
	*
	* @param[in] motor E_motor The motor to turn on, specified by the motor enumeration.
	* @param[in] flags uint8_t Bitmask for specifying motor direction and behavior, using definitions
	*                          like E_DRIVE_MOTOR_FORWARD_FLAG, E_DRIVE_MOTOR_BACKWARD_FLAG, etc.
	*/
void robokit_pwm_motor_on(E_motor motor, uint8_t flags);

/**
	* @brief Disables the PWM output for a specified motor.
	*
	* This function turns off the PWM signals to a specified motor, ensuring that
	* the motor stops operation by setting its PWM duty cycle to zero.
	*
	* @param[in] motor E_motor The motor to be turned off. Selection from predefined motor enumeration.
	* @return void
	*/
void robokit_pwm_motor_off(E_motor motor);

/**
	* @brief Sets the speed and direction for a specified PWM motor.
	*
	* This function configures the PWM signals for a specified motor, enabling control over its
	* speed and direction based on the provided flags. Depending on the combinations of flags
	* used (e.g., forward, backward, switch connectors), the function will adjust PWM duty cycles
	* to achieve the desired motor behavior.
	*
	* @param[in] motor E_motor The motor to control. Selection from predefined motor enumeration.
	* @param[in] speed T_Speed The speed value for the motor, ranging from -100 to 100.
	* @param[in] flags uint8_t Bitmask for specifying direction and behavior. Uses definitions like
	*                          E_DRIVE_MOTOR_FORWARD_FLAG, E_DRIVE_MOTOR_BACKWARD_FLAG, etc.
	* @return void
	*/
void robokit_pwm_motor_speed(E_motor motor, T_Speed speed, uint8_t flags);

/**
	* @brief Sets the speed and configuration flags for PWM motor 1.
	*
	* This function adjusts the speed and operational flags for motor 1.
	* It is effective when motor 1 is enabled, adjusting parameters such as speed and direction flags.
	*
	* @param[in] speed T_Speed The desired speed of the motor. The value typically ranges from -100 to 100, where positive values are forward and negative values are backward.
	* @param[in] flags uint8_t Configuration flags that determine the motor's behavior. These can include direction, decay mode, and more.
	* @return void
	*/
void robokit_pwm_motor_1_speed(T_Speed speed, uint8_t flags);

/**
	* @brief Sets the speed for PWM motor 2.
	*
	* This function configures the speed and direction of the second PWM motor
	* based on the specified speed and flags. It utilizes the appropriate PWM
	* control mechanism to achieve the desired motor behavior.
	*
	* @param [in] speed T_Speed The speed value for the motor, ranging from 0 (stopped) to 100 (full speed).
	* @param [in] flags uint8_t Bitmask flags to specify motor control options, such as direction and decay mode.
	*/
void robokit_pwm_motor_2_speed(T_Speed speed, uint8_t flags);

/**
	* @brief Sets the speed of PWM motor 3.
	*
	* This function controls the speed and direction of motor 3 using PWM signals.
	* It utilizes the given speed and flags to adjust the motor's behavior according
	* to the specified directives.
	*
	* @param[in] speed T_Speed The desired speed of the motor, ranging from 0 to 100.
	* @param[in] flags uint8_t Configuration flags to control motor behavior, such as
	*                direction and decay settings.
	* @return void
	*/
void robokit_pwm_motor_3_speed(T_Speed speed, uint8_t flags);

/**
	* @brief Sets the speed and control flags for Motor 4.
	*
	* This function adjusts the speed and control parameters of Motor 4 based on
	* the specified speed and flags. The function uses predefined flags to
	* control the direction, drive mode, and connector settings of the motor.
	*
	* @param [in] speed T_Speed Desired speed of Motor 4 ranging from 0 (stopped)
	*                    to 100 (full speed).
	* @param [in] flags uint8_t Bitmask for controlling motor direction, drive
	*                    mode, and connector settings.
	*
	* @return void
	*/
void robokit_pwm_motor_4_speed(T_Speed speed, uint8_t flags);

#endif //PWM_MOTORS_H
