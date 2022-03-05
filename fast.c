
#include "portdef.h"
#include "iodefine.h"
#include "parameters.h"
#include "mytypedef.h"
#include "glob_var.h"
#include "run.h"
#include "interface.h"
#include "sci.h"

extern int get_nextdir(int x, int y, int mask, t_direction *dir);

void fast_run(int x, int y)
{
//引数の座標x,yに向かって最短走行する

	t_direction glob_nextdir;

	//現在の向きから、次に行くべき方向へ向く
	switch(get_nextdir(x,y,MASK_SECOND,&glob_nextdir))	//次に行く方向を戻り値とする関数を呼ぶ
	{
		case front:
			if(isWallonSide(rear)){
				back(-40);
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
			break;
		
		case rear:
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					
			straight_NC(HALF_SECTION,FAST_ACCEL,FAST_SPEED,FAST_SPEED);		
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
				if(isWallonSide(left) && isWallonSide(right) && sen_fr.is_control && sen_fl.is_control){
					straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
				}else{
					straight_NC(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
				}
				break;
			
			case right:
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				
				if(isWallonSide(left)){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}
				break;
			
			case left:
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right)){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}
				break;
			
			case rear:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right)){
					back(-40);
					straight(25,1.0,1.0,0.0);
				}
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);
				if(isWallonSide(front)){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}
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
