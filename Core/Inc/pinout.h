/*
 * pinout.h
 *
 *  Created on: Mar 3, 2023
 *      Author: cpriscal
 */

#ifndef INC_PINOUT_H_
#define INC_PINOUT_H_

#include "stm32l4xx_hal.h"

static inline void setTurnSignalLight(GPIO_PinState PinState) {
	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_2, PinState);
}

static inline void setTailLight(GPIO_PinState PinState) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_4, PinState);
}

static inline void setBrakeLight(GPIO_PinState PinState) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_15, PinState);
}

static inline void setReverseLight(GPIO_PinState PinState) {
	HAL_GPIO_WritePin(GPIOA, GPIO_PIN_6, PinState);
}

#endif /* INC_PINOUT_H_ */
