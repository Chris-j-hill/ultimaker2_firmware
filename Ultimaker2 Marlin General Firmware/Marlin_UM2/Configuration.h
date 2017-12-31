#ifndef CONFIGURATION_H
#define CONFIGURATION_H

// Define the ultimaker type here and the only relavent config file will be included.
// All config files must be included in project folder


//completed config files
//#define UM2_1   // standard unmodified ultimaker
//#define UM2_2   // 1 extruder, enabled filament sensor
//#define UM2_3   // direct drive modification, 1 extruder, no filament sensor
//#define UM2_4   // direct drive modification, 1 extruder, enabled filament sensor
#define UM2_5   // 1 bowden and 1 direct drive extruder, no filament sensor
//#define UM2_9   // pen mod


//TO DO:

//#define UM2_6   // 1 bowden and 1 direct drive extruder, enabled filament sensor
//#define UM2_7   // 2 bowden extruders, no filament sensor
//#define UM2_8   // 2 bowden extruders, enabled filament sensor



#ifdef UM2_1
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config1.h"
#endif
#endif


#ifdef UM2_2
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config2.h"
#endif
#endif 


#ifdef UM2_3
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config3.h"
#endif
#endif


#ifdef UM2_4
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config4.h"
#endif
#endif


#ifdef UM2_5
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config5.h"
#endif
#endif

#ifdef UM2_6
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config6.h"
#endif
#endif

#ifdef UM2_7
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config7.h"
#endif
#endif


#ifdef UM2_8
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config8.h"
#endif
#endif


#ifdef UM2_9
#ifdef CONFIG_INCLUDED
#error "Too many configurations defined, can only build one at a time, only uncomment the relevant build in Configuation.h"
#else
#define CONFIG_INCLUDED
#include "Um2Config9.h"
#endif
#endif


#ifndef CONFIG_INCLUDED
#error "No printer configuration defined, uncomment the relevant build in Configuation.h"
#error "Ensure configuration is complete, and .h file included by configuration.h"
#endif

#endif //__CONFIGURATION_H

