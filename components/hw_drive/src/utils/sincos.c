//
//  sincos.c
//  robokit
//
//  Created by Thomas Abplanalp on 09.11.24.
//

#include "sincos.h"

// sinus-consinus-lookup-stable
//-------------------------------------------------------
// - Saved in 6 degree steps -> minute steps
// - First column:  sine
// - Second column: cosine
// sin(xdegree*pi/180) = sincostable[xdegree/6][0]/100
// cos(xdegree*pi/180) = sincostable[xdegree/6][1]/100
//-------------------------------------------------------
const int8_t sincostable[60][2]={
   // sin ,  cos
   {    0 ,  100 }, //  0 deg, 0 min
   {   10 ,   99 }, //  6 deg, 1 min
   {   21 ,   98 }, // 12 deg, 2 min
   {   31 ,   95 }, // 18 deg, 3 min
   {   41 ,   91 }, // 24 deg, 4 min
   {   50 ,   87 }, // 30 deg, 5 min
   {   59 ,   81 }, // 36 deg, 6 min
   {   67 ,   74 }, // 42 deg, 7 min
   {   74 ,   67 }, // 48 deg, 8 min
   {   81 ,   59 }, // 54 deg, 9 min
   {   87 ,   50 }, // 60 deg, 10 min
   {   91 ,   41 }, // 66 deg, 11 min
   {   95 ,   31 }, // 72 deg, 12 min
   {   98 ,   21 }, // 78 deg, 13 min
   {   99 ,   10 }, // 84 deg, 14 min
   {  100 ,    0 }, // 90 deg, 15 min
   {   99 ,  -10 }, // 96 deg, 16 min
   {   98 ,  -21 }, //102 deg, 17 min
   {   95 ,  -31 }, //108 deg, 18 min
   {   91 ,  -41 }, //114 deg, 19 min
   {   87 ,  -50 }, //120 deg, 20 min
   {   81 ,  -59 }, //126 deg, 21 min
   {   74 ,  -67 }, //132 deg, 22 min
   {   67 ,  -74 }, //138 deg, 23 min
   {   59 ,  -81 }, //144 deg, 24 min
   {   50 ,  -87 }, //150 deg, 25 min
   {   41 ,  -91 }, //156 deg, 26 min
   {   31 ,  -95 }, //162 deg, 27 min
   {   21 ,  -98 }, //168 deg, 28 min
   {   10 ,  -99 }, //174 deg, 29 min
   {    0 , -100 }, //180 deg, 30 min
   {  -10 ,  -99 }, //186 deg, 31 min
   {  -21 ,  -98 }, //192 deg, 32 min
   {  -31 ,  -95 }, //198 deg, 33 min
   {  -41 ,  -91 }, //204 deg, 34 min
   {  -50 ,  -87 }, //210 deg, 35 min
   {  -59 ,  -81 }, //216 deg, 36 min
   {  -67 ,  -74 }, //222 deg, 37 min
   {  -74 ,  -67 }, //228 deg, 38 min
   {  -81 ,  -59 }, //234 deg, 39 min
   {  -87 ,  -50 }, //240 deg, 40 min
   {  -91 ,  -41 }, //246 deg, 41 min
   {  -95 ,  -31 }, //252 deg, 42 min
   {  -98 ,  -21 }, //258 deg, 43 min
   {  -99 ,  -10 }, //264 deg, 44 min
   { -100 ,    0 }, //270 deg, 45 min
   {  -99 ,   10 }, //276 deg, 46 min
   {  -98 ,   21 }, //282 deg, 47 min
   {  -95 ,   31 }, //288 deg, 48 min
   {  -91 ,   41 }, //294 deg, 49 min
   {  -87 ,   50 }, //300 deg, 50 min
   {  -81 ,   59 }, //306 deg, 51 min
   {  -74 ,   67 }, //312 deg, 52 min
   {  -67 ,   74 }, //318 deg, 53 min
   {  -59 ,   81 }, //324 deg, 54 min
   {  -50 ,   87 }, //330 deg, 55 min
   {  -41 ,   91 }, //336 deg, 56 min
   {  -31 ,   95 }, //342 deg, 57 min
   {  -21 ,   98 }, //348 deg, 58 min
   {  -10 ,   99 }, //354 deg, 59 min
};

int8_t robokit_sin(int16_t degree) {
    uint8_t min = degree >= 0 ? (((degree+3) / 6) % 60) : 60 - (((-degree+3) / 6) % 60);
    return sincostable[min][0];
}

int8_t robokit_cos(int16_t degree) {
    uint8_t min = degree >= 0 ? (((degree+3) / 6) % 60) : (((-degree+3) / 6) % 60);
    return sincostable[min][1];
}

void robokit_sincos(int16_t degree, int8_t *sine, int8_t *cosine) {
    uint8_t min = degree >= 0 ? (((degree+3) / 6) % 60) : 60 - (((-degree+3) / 6) % 60);
    if(sine)
        *sine = sincostable[min][0];
    if(cosine)
        *cosine = sincostable[min][1];
}
