#include "FreeRTOS.h"
#include "task.h"

#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#include <irremote/irremote.h>

#include <Daikin_Commands.h>

uint8_t temperature;
uint8_t mode;
uint8_t fan;
uint8_t daikin_type;

bool swing;

uint8_t DaikinTemplate[35] = {
// First header
0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
//0   1     2     3     4     5     6     7
// Second header
0x11, 0xDA, 0x27, 0x00, 0x42, 0x00, 0x00, 0x54,
//8   9     10    11    12    13    14    15
// Third header
0x11, 0xDA, 0x27, 0x00, 0x00, 0x39, 0x32, 0x00, 0x30, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0xD3 };
//16  17    18    19    20    21    22    23    24    25   26     27    28    29    30    31    32    33    34
uint8_t DaikinTemplate_152[19] = {
// First header
0x11, 0xDA, 0x27, 0x00, 0x00, 0x00, 0x00,
//0   1     2     3     4     5     6
// Second header
0x00, 0xA0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xC5, 0x00, 0x00 ,0x00};
//7   8     9     10    11    12    13    14    15    16    17    18
uint8_t DaikinTemplate_160[20] = {
// First header
0x11, 0xDA, 0x27, 0xF0, 0x0D, 0x00, 0x0F,
//0   1     2     3     4     5     6
// Second header
0x11, 0xDA, 0x27, 0x00, 0xD3, 0x00, 0x00, 0x00, 0x00, 0x1E, 0x0A, 0x08, 0x00 };
//7   8     9     10    11    12    13    14    15    16    17    18    19
uint8_t DaikinTemplate_216[27] = {
// First header
0x11, 0xDA, 0x27, 0x00, 0xC5, 0x00, 0x00, 0xD7,
//0   1     2     3     4     5     6     7
// Second header
0x11, 0xDA, 0x27, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x06, 0x60, 0x00, 0x00, 0xC0, 0x00, 0x00, 0x00 };
//8   9     10    11    12    13    14    15    16    17    18    19    20    21    22    23    24    25    26

uint8_t checksum;
uint8_t checksum_216;
uint8_t checksum_152;
uint8_t checksum_160;

void sendIRbyte(uint8_t sendByte, int bitMarkLength, int zeroSpaceLength, int oneSpaceLength) {
for (int i = 0; i < 8; i++) {
if (sendByte & 0x01) {
ir_mark(bitMarkLength);
ir_space(oneSpaceLength);
}else{
ir_mark(bitMarkLength);
ir_space(zeroSpaceLength);
}
sendByte >>= 1;
}
}

uint8_t IRbitReverse(uint8_t x) {
//          01010101  |         10101010
x = ((x >> 1) & 0x55) | ((x << 1) & 0xaa);
//          00110011  |         11001100
x = ((x >> 2) & 0x33) | ((x << 2) & 0xcc);
//          00001111  |         11110000
x = ((x >> 4) & 0x0f) | ((x << 4) & 0xf0);

return x;
}

void  pass_temp_mode_values(int target_temp, int target_state, uint8_t target_fan, bool target_swing, int device_type_static) {

temperature = target_temp;
mode = target_state;
fan = target_fan;
swing = target_swing;
daikin_type = device_type_static;
}

// MACRO OFF
void ac_button_off_task() {
switch (daikin_type) {

case 0:

// Operating Mode
DaikinTemplate[21] = 0x00; // OFF

// Calculate the checksum
checksum = 0x00;

for (int i=16; i<34; i++) {
checksum += DaikinTemplate[i];
}

DaikinTemplate[34] = checksum;

taskENTER_CRITICAL();

// Header
for (int i=0; i<5; i++) {
ir_mark(428);
ir_space(428);
}
ir_mark(428);
ir_space(29000);

ir_mark(3650);
ir_space(1623);

// First Header
for (int i=0; i<8; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

// Pause + new header
ir_mark(428);
ir_space(29000);
ir_mark(3650);
ir_space(1623);

// Second Header
for (int i=8; i<16; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

// Pause + new header
ir_mark(428);
ir_space(29000);
ir_mark(3650);
ir_space(1623);

// Second Header
for (int i=16; i<35; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

ir_mark(428);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 1:

DaikinTemplate_152[5] = 0x00; // OFF

// Calculate the checksum
checksum_152 = 0x00;

for (int i=0; i<18; i++) {
checksum_152 += DaikinTemplate_152[i];
}

DaikinTemplate_152[18] = checksum_152;

taskENTER_CRITICAL();

// Header
for (int i=0; i<5; i++) {
ir_mark(433);
ir_space(433);
}
ir_mark(433);
ir_space(25182);

ir_mark(3492);
ir_space(1718);

// First Header
for (int i=0; i<19; i++) {
sendIRbyte(DaikinTemplate_152[i], 433, 433, 1529);
}

ir_mark(433);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 2:

DaikinTemplate_160[12] = 0x00; // OFF

// Calculate the checksum
checksum_160 = 0x00;

for (int i=7; i<19; i++) {
checksum_160 += DaikinTemplate_160[i];
}

DaikinTemplate_160[19] = checksum_160;

taskENTER_CRITICAL();

// Header
ir_mark(5000);
ir_space(2145);

// First Header
for (int i=0; i<7; i++) {
sendIRbyte(DaikinTemplate_160[i], 342, 700, 1786);
}

// Pause + new header
ir_mark(342);
ir_space(29650);
ir_mark(5000);
ir_space(2145);

// Last bytes - the actual payload
for (int i=7; i<20; i++) {
sendIRbyte(DaikinTemplate_160[i], 342, 700, 1786);
}

ir_mark(342);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 3:

// Operating Mode
DaikinTemplate_216[13] = 0x00; // OFF

// Calculate the checksum
checksum_216 = 0x00;

for (int i=8; i<26; i++) {
checksum_216 += DaikinTemplate_216[i];
}

DaikinTemplate_216[26] = checksum_216;

taskENTER_CRITICAL();

// Header
ir_mark(3360);
ir_space(1760);

// First Header
for (int i=0; i<8; i++) {
sendIRbyte(DaikinTemplate_216[i], 360, 520, 1370);
}

// Pause + new header
ir_mark(360);
ir_space(32300);
ir_mark(3360);
ir_space(1760);

// Second Header
for (int i=8; i<27; i++) {
sendIRbyte(DaikinTemplate_216[i], 360, 520, 1370);
}

ir_mark(360);
ir_space(0);

taskEXIT_CRITICAL();

break;


default:
printf("No action \n");
}
vTaskDelete(NULL);
}

// MACRO
void ac_button_temp_task() {

switch (daikin_type) {

case 0:

// HEAT/COOL Mode
if (mode == 1) {
DaikinTemplate[21] = 0x41; // HEAT
}else if (mode == 2) {
DaikinTemplate[21] = 0x31; // COOL
}

// Calculate Fan
if (fan == 0) {
DaikinTemplate[24] = 0xA0; // AUTO
}else if (fan == 1) {
DaikinTemplate[24] = 0x30; // FAN 1
}else if (fan == 2) {
DaikinTemplate[24] = 0x40; // FAN 2
}else if (fan == 3) {
DaikinTemplate[24] = 0x50; // FAN 3
}else if (fan == 4) {
DaikinTemplate[24] = 0x60; // FAN 4
}else if (fan == 5) {
DaikinTemplate[24] = 0x70; // FAN 5
}

// Calculate Swing
if (swing == true) {
DaikinTemplate[24]++;
}else{
}

// Calculate Temp
DaikinTemplate[22] = temperature << 1;

// Calculate the checksum
checksum = 0x00;

for (int i=16; i<34; i++) {
checksum += DaikinTemplate[i];
}

DaikinTemplate[34] = checksum;

taskENTER_CRITICAL();

// Header
for (int i=0; i<5; i++) {
ir_mark(428);
ir_space(428);
}
ir_mark(428);
ir_space(29000);

ir_mark(3650);
ir_space(1623);

// First Header
for (int i=0; i<8; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

// Pause + new header
ir_mark(428);
ir_space(29000);
ir_mark(3650);
ir_space(1623);

// Second Header
for (int i=8; i<16; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

// Pause + new header
ir_mark(428);
ir_space(29000);
ir_mark(3650);
ir_space(1623);

// Second Header
for (int i=16; i<35; i++) {
sendIRbyte(DaikinTemplate[i], 428, 428, 1280);
}

ir_mark(428);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 1:

// HEAT/COOL Mode
if (mode == 1) {
DaikinTemplate_152[5] = 0x41; // HEAT
}else if (mode == 2) {
DaikinTemplate_152[5] = 0x31; // COOL
}

// Calculate Fan
if (fan == 0) {
DaikinTemplate_152[8] = 0xA0; // AUTO
}else if (fan == 1) {
DaikinTemplate_152[8] = 0x30; // FAN 1
}else if (fan == 2) {
DaikinTemplate_152[8] = 0x40; // FAN 2
}else if (fan == 3) {
DaikinTemplate_152[8] = 0x50; // FAN 3
}else if (fan == 4) {
DaikinTemplate_152[8] = 0x60; // FAN 4
}else if (fan == 5) {
DaikinTemplate_152[8] = 0x70; // FAN 5
}

if (swing == true) {
DaikinTemplate_152[8] = (uint8_t)(0x0F + DaikinTemplate_152[8]);
}else{
}

// Calculate Temp
DaikinTemplate_152[6] = temperature << 1;

// Calculate the checksum
checksum_152 = 0x00;

for (int i=0; i<18; i++) {
checksum_152 += DaikinTemplate_152[i];
}

DaikinTemplate_152[18] = checksum_152;

taskENTER_CRITICAL();

// Header
for (int i=0; i<5; i++) {
ir_mark(433);
ir_space(433);
}
ir_mark(433);
ir_space(25182);

ir_mark(3492);
ir_space(1718);

// First Header
for (int i=0; i<19; i++) {
sendIRbyte(DaikinTemplate_152[i], 433, 433, 1529);
}

ir_mark(433);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 2:

// HEAT/COOL Mode
if (mode == 1) {
DaikinTemplate_160[12] = 0x41; // HEAT
}else if (mode == 2) {
DaikinTemplate_160[12] = 0x31; // COOL
}

// Calculate Fan
if (fan == 0) {
DaikinTemplate_160[17] = 0x0A; // AUTO
}else if (fan == 1) {
DaikinTemplate_160[17] = 0x03; // FAN 1
}else if (fan == 2) {
DaikinTemplate_160[17] = 0x04; // FAN 2
}else if (fan == 3) {
DaikinTemplate_160[17] = 0x05; // FAN 3
}else if (fan == 4) {
DaikinTemplate_160[17] = 0x06; // FAN 4
}else if (fan == 5) {
DaikinTemplate_160[17] = 0x07; // FAN 5
}

// Swing
if (swing == false) {
DaikinTemplate_160[13] = 0x00; // OFF
}else if (swing == true) {
DaikinTemplate_160[13] = 0xF0; // AUTO
}

// Calculate Temp
DaikinTemplate_160[16] = (temperature << 1) - 20;

// Calculate the checksum
checksum_160 = 0x00;

for (int i=7; i<19; i++) {
checksum_160 += DaikinTemplate_160[i];
}

DaikinTemplate_160[19] = checksum_160;

taskENTER_CRITICAL();

// Header
ir_mark(5000);
ir_space(2145);

// First Header
for (int i=0; i<7; i++) {
sendIRbyte(DaikinTemplate_160[i], 342, 700, 1786);
}

// Pause + new header
ir_mark(342);
ir_space(29650);
ir_mark(5000);
ir_space(2145);

// Last 19 bytes - the actual payload
for (int i=7; i<20; i++) {
sendIRbyte(DaikinTemplate_160[i], 342, 700, 1786);
}

ir_mark(342);
ir_space(0);

taskEXIT_CRITICAL();

break;

case 3:

// HEAT/COOL Mode
if (mode == 1) {
DaikinTemplate_216[13] = 0x41; // HEAT
}else if (mode == 2) {
DaikinTemplate_216[13] = 0x31; // COOL
}

// Calculate Fan
if (fan == 0) {
DaikinTemplate_216[16] = 0xA0; // AUTO
}else if (fan == 1) {
DaikinTemplate_216[16] = 0x30; // FAN 1
}else if (fan == 2) {
DaikinTemplate_216[16] = 0x40; // FAN 2
}else if (fan == 3) {
DaikinTemplate_216[16] = 0x50; // FAN 3
}else if (fan == 4) {
DaikinTemplate_216[16] = 0x60; // FAN 4
}else if (fan == 5) {
DaikinTemplate_216[16] = 0x70; // FAN 5
}

// Calculate Swing
if (swing == true) {
DaikinTemplate_216[16]++;
}else{
}

// Calculate Temp
DaikinTemplate_216[14] = temperature << 1;

// Calculate the checksum
checksum_216 = 0x00;

for (int i=8; i<26; i++) {
checksum_216 += DaikinTemplate_216[i];
}

DaikinTemplate_216[26] = checksum_216;

taskENTER_CRITICAL();

// Header
ir_mark(3360);
ir_space(1760);

// First Header
for (int i=0; i<8; i++) {
sendIRbyte(DaikinTemplate_216[i], 360, 520, 1370);
}

// Pause + new header
ir_mark(360);
ir_space(32300);
ir_mark(3360);
ir_space(1760);

// Last 19 bytes - the actual payload
for (int i=8; i<27; i++) {
sendIRbyte(DaikinTemplate_216[i], 360, 520, 1370);
}

ir_mark(360);
ir_space(0);

taskEXIT_CRITICAL();

break;


default:
printf("No action \n");
}
vTaskDelete(NULL);
}

// TASKS
void ac_button_off() {
xTaskCreate(ac_button_off_task, "Send IR", 2048, NULL, 2 | portPRIVILEGE_BIT, NULL);
}

void ac_button_temp() {
xTaskCreate(ac_button_temp_task, "Send IR", 2048, NULL, 2 | portPRIVILEGE_BIT, NULL);
}