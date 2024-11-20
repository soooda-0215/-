#include <h8/reg3067.h>
#include <mes2.h>

int ADdata = 0;
// A/D Conv. Start
void ADStart(void){
ADCSR = ADCSR | 0x20; 
}
 
// Read aft. A/D Conv.
int ADRead(void){
    if((ADCSR & 0x80) == 0x80){ 
        ADdata = (ADDRAH << 2) | (ADDRAL & 0x03);
        ADCSR = (ADCSR & (~0x80));
    }
    return ADdata; 
}
 
// A/D Conv. Execution
void ADCexec(){
if((ADCSR & 0x20) == 0x00) // 問５
        ADStart();
}
void init() {
    // モータ用端子(P4)の設定
    // P40とP41はモータ1の出力用、P42とP43はモータ2の出力用、P44〜P47はエンコーダ（入力用）
    P4DDR = 0x0F; // 下位4ビット(P40-P43)を出力、上位4ビット(P44-P47)を入力に設定
 
    // LED用端子(P6)の設定
    // 全て出力用（P60, P61, P62のみ使用）
    P6DDR = 0x07; // 下位3ビット(P60-P62)を出力に設定
 
    // スイッチ(PA)の設定
    // PA4が白、PA5が黒、PA6が青、PA7が赤に対応（入力用）、PA0〜3は7セグメントLED（出力用）に対応
    PADDR = 0x0F; // 上位4ビット(PA4-PA7)を入力、下位4ビット(PA0-PA3)を出力に設定
}
 
// 車体を前進させる
void motor_fwd() {
    P4DR = 0x06 | (P4DR & 0xF0); // P42とP41を1にセット（0000 0110）
}
 
// 車体を停止させる
void motor_stop() {
    P4DR = P4DR & 0xF0; // 下位4ビットを全て0にして停止
}
 
// 車体を後退させる
void motor_back() {
    P4DR = 0x09 | (P4DR & 0xF0); // P43とP40を1にセット（0000 1001）
}
 
// 車体を右旋回させる
void motor_right() {
    P4DR = 0x02 | (P4DR & 0xF0); // P41のみ1にセット（0000 0010）
}
 
// 車体を左旋回させる
void motor_left() {
    P4DR = 0x04 | (P4DR & 0xF0); // P42のみ1にセット（0000 0100）
}
 
int main(void) {
    int flag = 0; // 0: 停止、1: 動作中
    int adcdata = 0;

    init(); // 初期化
 
    ADStart();
    while (1) {
        // ボタンの状態を確認
        if ((PADR & 0x40)==0x00) { // 青ボタン（PA6）が押されたとき
            flag = 1; // 動作開始
        }
        if ((PADR & 0x80)==0x00) { // 赤ボタン（PA7）が押されたとき
            flag = 0; // 動作停止
	      }

        ADCexec();
        adcdata = ADRead();//a/d 変換した値を保存

        // flagの状態に応じて動作を制御
        if (flag == 1) {
            if(adcdata>180){
                motor_fwd(); // 前進
            }
            if(adcdata<180){//find black
                motor_stop(); // 停止
            }
          }

        if (flag == 0){
            motor_stop(); //停止
        }
    }
 
    return 0;
}
