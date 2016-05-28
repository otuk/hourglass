#ifndef _MACROS_H
#define _MACROS_H

#ifdef PCRUN
//print with cout on PC
#include <iostream>


#define PRINTERR(msg)				\
  std::cout << msg << std::endl

#define PRINT(msg)				\
  std::cout << msg
  
#define M_PI 3.14

#else
//print to serial line on arduino
#include "Arduino.h"
#define PRINTERR(msg)				\
  Serial.println(msg)

#define PRINT(msg)\
  Serial.print(msg)
#endif //PCRUN

#endif //_MACROS_H
