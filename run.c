/***********************************************************************/
/*                                                                     */
/*  FILE        :spi.c			                               */
/*  DATE        :Tue, Jun 08, 2017                                     */
/*  DESCRIPTION :SPI Program                                           */
/*  CPU TYPE    :                                                      */
/*                                                                     */
/*  NOTE:THIS IS A TYPICAL EXAMPLE.                                    */
/*                                                                     */
/***********************************************************************/
#include "iodefine.h"
#include "mathf.h"
#include "sci.h"
#include "init.h"
#include "spi.h"
#include "parameters.h"
#include "glob_var.h"
#include "mytypedef.h"
#include "portdef.h"
#include "interface.h"

extern wait_ms(int wtime);

void running(float len, float acc, float max_sp, float end_sp,t_bool mode,unsigned limit_timer){
	unsigned int start_timer;
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;
	//走行モードを直線にする
	run_mode = STRAIGHT_MODE;
	//壁制御を有効にする
	con_wall.enable = mode;
	//目標距離をグローバル変数に代入する
	len_target = len;
	//目標速度を設定
	end_speed = end_sp;
	//加速度を設定
	accel = acc;
	//最高速度を設定
	max_speed = max_sp;
	
	//モータ出力をON
	MOT_POWER_ON;
	start_timer = timer;
	

	if(end_speed == 0.0){	//最終的に停止する場合
		//減速処理を始めるべき位置まで加速、定速区間を続行
		while( ((len_target - 20) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < (len_target + 5)){		//停止したい距離の少し手前まで継続
			if((sen_fr.is_wall==false) && (sen_fl.is_wall==false) && (len_mouse > len_target)){//前壁なし
				
        		break;
    		}
    		if( (sen_fr.value > RIGHT_90) && (sen_fl.value > LEFT_90) && ( len_mouse > (len_target - 5)) ) {//目標の位置の5mm手前
				
        		break;
    		}
			//一定速度まで減速したら最低駆動トルクで走行
			if(tar_speed <= MIN_SPEED){	//目標速度が最低速度になったら、加速度を0にする
				accel = 0;
				tar_speed = MIN_SPEED;
			}
			if((timer - start_timer) >= limit_timer && limit_timer > 0){
				
				break;
			}
		}
		accel = 0;
		tar_speed = 0;
		//速度が0以下になるまで逆転する
		while(speed >= 0.0);
			
	}else{

		//減速処理を始めるべき位置まで加速、定速区間を続行
		while( ((len_target - 25) - len_mouse) >  1000.0*((float)(tar_speed * tar_speed) - (float)(end_speed * end_speed))/(float)(2.0*accel));
		
		//減速処理開始
		accel = -acc;					//減速するために加速度を負の値にする	
		while(len_mouse < (len_target)){		//停止したい距離の少し手前まで継続
			//一定速度まで減速したら最低駆動トルクで走行
			if(tar_speed <= end_speed){	//目標速度が最低速度になったら、加速度を0にする
				accel = 0;
				//tar_speed = end_speed;
			}
			if((timer - start_timer) >= limit_timer && limit_timer > 0){

				break;
			}
			
		}
	}
	//加速度を0にする
	accel = 0;
	//現在距離を0にリセット
	len_mouse = 0;
}

void straight(float len, float acc, float max_sp, float end_sp){
	running(len,acc,max_sp,end_sp,true,3000);
}

void straight_NC(float len, float acc, float max_sp, float end_sp){
	running(len,acc,max_sp,end_sp,false,3000);
}
extern volatile unsigned int timer;
void back(float len){
	LED(0);
	unsigned int start_timer;//タイヤがロックした時の対策
    I_tar_ang_vel = 0;
    I_ang_vel = 0;
    I_tar_speed = 0;
    len_mouse = 0;//距離の初期化
    run_mode = STRAIGHT_MODE;
    con_wall.enable = false;//壁制御を無効にする
    len_target = len;//lenの値はマイナスが入っている
    end_speed = 0;
    accel = -0.4;//速度をマイナスにするとバックする
    max_speed = 0.1;
    MOT_POWER_ON;
    while(((len_target -10) -len_mouse) > 1000.0*((float)(tar_speed*tar_speed)-(float)(end_speed*end_speed))/(float)(2.0*accel));
    accel = 1.5;//マイナスの速度を0にするためプラスにしている
    start_timer = timer;
    while(len_mouse > (len_target + 1)){
        if(tar_speed >= -1*MIN_SPEED){//MIN_SPEEDはプラスの値なので、-1を掛けてマイナスにする
            accel=0;
            tar_speed = -1*MIN_SPEED;
        }
        if((timer - start_timer) > 200 && (speed - MIN_SPEED) >= 0){ //0.2秒経過後にケツ当たるか0.8秒経過で終了
            break;
        }
		if((timer - start_timer) > 800) break;
    }
    accel = 0;
    tar_speed = 0;
    len_mouse = 0;
}

void turn(int deg, float ang_accel, float max_ang_velocity, short dir){
	wait_ms(WAIT_TIME);
	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;
	tar_degree = 0;

	float local_degree = 0;
	accel = 0;
	tar_speed = 0;
	tar_ang_vel = 0;
	//走行モードをスラロームモードにする
	run_mode = TURN_MODE;

	//回転方向定義
	TURN_DIR = dir;	
	
	//車体の現在角度を取得
	local_degree = degree;
	tar_degree = 0;
	
	//角加速度、加速度、最高角速度設定
	MOT_POWER_ON;
	if(dir == LEFT){
		ang_acc = ang_accel;			//角加速度を設定
		max_ang_vel = max_ang_velocity;
		max_degree = deg;
		while( (max_degree - (degree - local_degree))*PI/180.0 > (tar_ang_vel*tar_ang_vel/(2.0 * ang_acc)));
		
	}else if(dir == RIGHT){
		ang_acc = -ang_accel;			//角加速度を設定
		max_ang_vel = -max_ang_velocity;
		max_degree = -deg;
		while(-(float)(max_degree - (degree - local_degree))*PI/180.0 > (float)(tar_ang_vel*tar_ang_vel/(float)(2.0 * -ang_acc)));
	}

	//BEEP();
	//角減速区間に入るため、角加速度設定
	MOT_POWER_ON;
	if(dir == LEFT){
		ang_acc = -ang_accel;			//角加速度を設定
		//減速区間走行
		while((degree - local_degree) < max_degree){
			if(tar_ang_vel < TURN_MIN_SPEED){
				ang_acc = 0;
				tar_ang_vel = TURN_MIN_SPEED;
			}
		}
		
		ang_acc = 0;
		tar_ang_vel = 0;
		tar_degree = max_degree;
		
	}else if(dir == RIGHT){
		ang_acc = +ang_accel;			//角加速度を設定
		//減速区間走行
		while((degree - local_degree) > max_degree){
			if(-tar_ang_vel < TURN_MIN_SPEED){
				ang_acc = 0;
				tar_ang_vel = -TURN_MIN_SPEED;
			}
		}
		ang_acc = 0;
		tar_ang_vel = 0;
		tar_degree = max_degree;


	}
	
	while(ang_vel >= 0.05 || ang_vel <= -0.05 );
	
	tar_ang_vel = 0;
	ang_acc = 0;
	//現在距離を0にリセット
	len_mouse = 0;
	wait_ms(WAIT_TIME);
}

void slalom(int deg,float acc, float max_sp, float end_sp,short dir){
	wait_ms(WAIT_TIME);
	LED(5);

	I_tar_ang_vel = 0;
	I_ang_vel = 0;
	I_tar_speed = 0;
	I_speed = 0;

	tar_speed = max_sp;
	max_speed = max_sp;
	accel = acc;
	TURN_DIR = dir;

	run_mode = TURN_MODE;
	float local_degree = degree;

	MOT_POWER_ON;

	if(dir == LEFT){
		tar_ang_vel = 12.0;
		max_ang_vel = TURN_SPEED;
		max_degree = deg;
	}else if(dir == RIGHT){
		tar_ang_vel = -12.0;
		max_ang_vel = -TURN_SPEED;
		max_degree = -deg;
	}

	if(dir == LEFT){
		while((degree - local_degree) < max_degree){
		}
	}else if(dir == RIGHT){
		while((degree - local_degree) > max_degree){
		}
	}
	run_mode = STRAIGHT_MODE;
	MOT_POWER_ON;

	ang_acc = 0;
	tar_ang_vel = 0;
	tar_degree = max_degree;
	tar_speed = end_sp;

	while(ang_vel >= 0.05 || ang_vel <= -0.05 );
	len_mouse = 0;
	tar_ang_vel = 0;
	ang_acc = 0;
	LED(0);
}