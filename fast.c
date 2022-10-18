
#include "portdef.h"
#include "iodefine.h"
#include "parameters.h"
#include "mytypedef.h"
#include "glob_var.h"
#include "run.h"
#include "interface.h"
#include "sci.h"

#define H_LIMIT 2

extern int get_nextdir(int x, int y, int mask, t_direction *dir);

void fast_run(int x, int y)
{
//引数の座標x,yに向かって最短走行する
	int t = 0;
	t_direction glob_nextdir;
	do_back = -1;

	while((mypos.x != x) || (mypos.y != y)){
		switch(get_nextdir(x,y,MASK_SECOND,&glob_nextdir)){
			case front:
				stlen[t]++;
				if(!(isWallonSide(right) && isWallonSide(left))) iswall[t] = 0;
				break;
			case right:
			case left:
				nonwall[t] = isWallonSide(front);
				t++;
				break;
			default:
				t++;
				break;
		}
		mypos.dir = glob_nextdir;
		switch(mypos.dir){
			case north:
				mypos.y++;	//北を向いた時はY座標を増やす
				break;
				
			case east:
				mypos.x++;	//東を向いた時はX座標を増やす
				break;
				
			case south:
				mypos.y--;	//南を向いた時はY座標を減らす
				break;
			
			case west:
				mypos.x--;	//西を向いたときはX座標を減らす
				break;

		}
	}
	mypos.x=mypos.y=0,t=0;
	mypos.dir = north;

	//現在の向きから、次に行くべき方向へ向く
	switch(get_nextdir(x,y,MASK_SECOND,&glob_nextdir))	//次に行く方向を戻り値とする関数を呼ぶ
	{
		case front:
			len_mouse-=SLIP_DIST_FAST;
			if(isWallonSide(rear)){
				back(-40);
				gyro_get_ref();
				straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);		
			}else{
				straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);		
			}
			break;
		
		case right:
			turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				
			if(isWallonSide(left)){
				back(-40);
				straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				do_back = INT_BACK;
			}else{
				straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
			}
			t++;
			break;
		
		case left:
			turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
			if(isWallonSide(right)){
				back(-40);
				straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				do_back = INT_BACK;
			}else{
				straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
			}
			t++;
			break;
		
		case rear:
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					
			straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
			t++;	
			break;
	}

	mypos.dir = glob_nextdir;	//自分の向きを更新


	//向いた方向によって自分の座標を更新する
	switch(mypos.dir)
	{
		case north:
			mypos.y++;	//北を向いた時はY座標を増やす
			break;
			
		case east:
			mypos.x++;	//東を向いた時はX座標を増やす
			break;
			
		case south:
			mypos.y--;	//南を向いた時はY座標を減らす
			break;
		
		case west:
			mypos.x--;	//西を向いたときはX座標を減らす
			break;

	}

	
	while((mypos.x != x) || (mypos.y != y)){			//ゴールするまで繰り返す


		switch(get_nextdir(x,y,MASK_SECOND,&glob_nextdir))	//次に行く方向を戻り値とする関数を呼ぶ
		{
			case front:
				LED(iswall[t]);
				len_mouse-=SLIP_DIST_FAST;
				if(iswall[t]){
					straight(SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);		
				}else{
					straight_NC(SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);		
				}
				break;
			
			case right:
				t++;
				straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,0);		
				turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);
				if(isWallonSide(left) && (stlen[t] >= H_LIMIT || !(nonwall[t]))){
					back(-40);
					straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}else{
					straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}
				break;
			
			case left:
				t++;
				straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,0);		
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right) && (stlen[t] >= H_LIMIT || !(nonwall[t]))){
					back(-40);
					straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}else{
					straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}
				break;
			
			case rear:
				straight(HALF_SECTION,FAST_ACCEL,FAST_SPEED,0);		
				
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right)){
					back(-40);
					straight(25,1.0,1.0,0.0);
				}
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);
				if(isWallonSide(front)){
					back(-40);
					straight_NC(HALF_SECTION+20,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}else{
					straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);
				}
				t++;
				break;
		}
	
		mypos.dir = glob_nextdir;	//自分の向きを修正
		
		//向いた方向によって自分の座標を更新する
		switch(mypos.dir)
		{
			case north:
				mypos.y++;	//北を向いた時はY座標を増やす
				break;
				
			case east:
				mypos.x++;	//東を向いた時はX座標を増やす
				break;
				
			case south:
				mypos.y--;	//南を向いた時はY座標を減らす
				break;
			
			case west:
				mypos.x--;	//西を向いたときはX座標を減らす
				break;

		}
	}
	straight(HALF_SECTION,FAST_ACCEL,FAST_SPEED,0.0);
}
