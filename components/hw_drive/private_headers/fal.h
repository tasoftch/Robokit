//
// Created by Thomas Abplanalp on 27.11.24.
//

#ifndef FAL_H
#define FAL_H

#define GPIO_RED   10  // GPIO_10 als Ausgang für Rot
#define GPIO_GREEN 11  // GPIO_11 als Ausgang für Grün
#define GPIO_BLUE  12  // GPIO_12 als Ausgang für Blau

void fal_init(void);

void fal_update(void);

#endif //FAL_H
