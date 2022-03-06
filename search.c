
#include "mytypedef.h"
#include "parameters.h"
#include "iodefine.h"
#include "portdef.h"
#include "glob_var.h"
#include "macro.h"
#include "run.h"
#include "interface.h"

void search_lefthand(void);					

void init_map(int x, int y);					
void make_map(int x, int y, int mask);				
void set_wall(int x, int y);					
t_bool is_unknown(int x, int y);				
int get_priority(int x, int y, t_direction dir);		
int get_nextdir(int x, int y, int mask, t_direction *dir);	
void search_adachi(int gx, int gy);				

extern void wait_ms(int wtime);

void search_lefthand(void)
{
	
	max_speed = SEARCH_SPEED;				
	accel = SEARCH_ACCEL;
	
	straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);			
		
	while(1)
	{
		if(sen_l.is_wall == false)			
		{
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
			turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);	
		}
		else if( (sen_fl.is_wall == false) && (sen_fr.is_wall == false) )	
		{
			straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
		}
		else if(sen_r.is_wall == false)			
		{
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
			turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);			
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);	
		}
		else						
		{
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);			
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);	
		}	
	
	}
}


void init_map(int x, int y)
{


	int i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)		
	{
		for(j = 0; j < MAZESIZE_Y; j++)	
		{
			map[i][j] = 255;	
		}
	}
	
	map[x][y] = 0;				
	
}


void make_map(int x, int y, int mask)	
{




	int i,j;
	t_bool change_flag;			

	init_map(x,y);				

	do
	{
		change_flag = false;				
		for(i = 0; i < MAZESIZE_X; i++)			
		{
			for(j = 0; j < MAZESIZE_Y; j++)		
			{
				if(map[i][j] == 255)		
				{
					continue;
				}
				
				if(j < MAZESIZE_Y-1)					
				{
					if( (wall[i][j].north & mask) == NOWALL)	
					{
						if(map[i][j+1] == 255)			
						{
							map[i][j+1] = map[i][j] + 1;	
							change_flag = true;		
						}
					}
				}
			
				if(i < MAZESIZE_X-1)					
				{
					if( (wall[i][j].east & mask) == NOWALL)		
					{
						if(map[i+1][j] == 255)			
						{
							map[i+1][j] = map[i][j] + 1;	
							change_flag = true;		
						}
					}
				}
			
				if(j > 0)						
				{
					if( (wall[i][j].south & mask) == NOWALL)	
					{
						if(map[i][j-1] == 255)			
						{
							map[i][j-1] = map[i][j] + 1;	
							change_flag = true;		
						}
					}
				}
			
				if(i > 0)						
				{
					if( (wall[i][j].west & mask) == NOWALL)		
					{
						if(map[i-1][j] == 255)			
						{
							map[i-1][j] = map[i][j] + 1;	
							change_flag = true;		
						}
						
					}
					
				}
				
			}
			
		}
		
	}while(change_flag == true);	
	
}



void set_wall(int x, int y)	
{

	int n_write,s_write,e_write,w_write;
	
	
	
	
	switch(mypos.dir){
		case north:	
		
			n_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);	
			e_write = CONV_SEN2WALL(sen_r.is_wall);				
			w_write = CONV_SEN2WALL(sen_l.is_wall);				
			s_write = NOWALL;						
			
			break;
			
		case east:	
			
			e_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);	
			s_write = CONV_SEN2WALL(sen_r.is_wall);				
			n_write = CONV_SEN2WALL(sen_l.is_wall);				
			w_write = NOWALL;						
			
			break;
			
		case south:	
		
			s_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);	
			w_write = CONV_SEN2WALL(sen_r.is_wall);				
			e_write = CONV_SEN2WALL(sen_l.is_wall);				
			n_write = NOWALL;						

			break;
			
		case west:	
		
			w_write = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);	
			n_write = CONV_SEN2WALL(sen_r.is_wall);				
			s_write = CONV_SEN2WALL(sen_l.is_wall);				
			e_write = NOWALL;						
			
			break;
			
	}
	
	wall[x][y].north = n_write;	
	wall[x][y].south = s_write;	
	wall[x][y].east  = e_write;	
	wall[x][y].west  = w_write;	
	
	if(y < MAZESIZE_Y-1)	
	{
		wall[x][y+1].south = n_write;	
	}
	
	if(x < MAZESIZE_X-1)	
	{
		wall[x+1][y].west = e_write;	
	}
	
	if(y > 0)	
	{
		wall[x][y-1].north = s_write;	
	}
	
	if(x > 0)	
	{
		wall[x-1][y].east = w_write;	
	}
	
}


t_bool is_unknown(int x, int y)	
{
	
	
	if((wall[x][y].north == UNKNOWN) || (wall[x][y].east == UNKNOWN) || (wall[x][y].south == UNKNOWN) || (wall[x][y].west == UNKNOWN))
	{			
		return true;	
	}
	else
	{
		return false;	
	}
}



int get_priority(int x, int y, t_direction dir)	
{
	
	
	
	
	

	int priority;	
	
	priority = 0;
	
	if(mypos.dir == dir)				
	{
		priority = 2;
	}
	else if( ((4+mypos.dir-dir)%4) == 2)		
	{
		priority = 0;
	}
	else						
	{
		priority = 1;
	}
	
	
	if(is_unknown(x,y) == true)
	{
		priority += 4;				
	}
	
	return priority;				
	
}


int get_nextdir(int x, int y, int mask, t_direction *dir)	
{
	
	
	int little,priority,tmp_priority;		


	make_map(x,y,mask);				
	little = 255;					

	priority = 0;					
	
		
	if( (wall[mypos.x][mypos.y].north & mask) == NOWALL)			
	{
		tmp_priority = get_priority(mypos.x, mypos.y + 1, north);	
		if(map[mypos.x][mypos.y+1] < little)				
		{
			little = map[mypos.x][mypos.y+1];			
			*dir = north;						
			priority = tmp_priority;				
		}
		else if(map[mypos.x][mypos.y+1] == little)			
		{
			if(priority < tmp_priority )				
			{
				*dir = north;					
				priority = tmp_priority;			
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].east & mask) == NOWALL)			
	{
		tmp_priority = get_priority(mypos.x + 1, mypos.y, east);	
		if(map[mypos.x + 1][mypos.y] < little)				
		{
			little = map[mypos.x+1][mypos.y];			
			*dir = east;						
			priority = tmp_priority;				
		}
		else if(map[mypos.x + 1][mypos.y] == little)			
		{
			if(priority < tmp_priority)				
			{
				*dir = east;					
				priority = tmp_priority;			
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].south & mask) == NOWALL)			
	{
		tmp_priority = get_priority(mypos.x, mypos.y - 1, south);	
		if(map[mypos.x][mypos.y - 1] < little)				
		{
			little = map[mypos.x][mypos.y-1];			
			*dir = south;						
			priority = tmp_priority;				
		}
		else if(map[mypos.x][mypos.y - 1] == little)			
		{
			if(priority < tmp_priority)				
			{
				*dir = south;					
				priority = tmp_priority;			
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].west & mask) == NOWALL)			
	{
		tmp_priority = get_priority(mypos.x - 1, mypos.y, west);	
		if(map[mypos.x-1][mypos.y] < little)				
		{
			little = map[mypos.x-1][mypos.y];			
			*dir = west;						
			priority = tmp_priority;				
		}
		else if(map[mypos.x - 1][mypos.y] == little)			
		{
			*dir = west;						
			priority = tmp_priority;				
		}
	}


	return ( (int)( ( 4 + *dir - mypos.dir) % 4 ) );			
										
	
}

short isWallonSide(short dire)
{
	short rt = 0;
	switch((int)((4+mypos.dir + dire) % 4)){
		case north:
			rt = wall[mypos.x][mypos.y].north & MASK_SECOND;
			break;
		case east:
			rt = wall[mypos.x][mypos.y].east & MASK_SECOND;
			break;
		case south:
			rt = wall[mypos.x][mypos.y].south & MASK_SECOND;
			break;
		case west:
			rt = wall[mypos.x][mypos.y].west & MASK_SECOND;
			break;		 
	}
	return rt;
}

//‘«—§–@
void search_adachi(int gx, int gy)
{

	t_direction glob_nextdir;					

	accel=SEARCH_ACCEL;
	do_back = INT_BACK;

	switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		
	{
		case front:
			if(isWallonSide(rear)){
				back(-40);
				straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
			}else{
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
			}
			break;
		
		case right:
			turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				
			if(isWallonSide(left)){
				back(-40);
				straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				do_back = INT_BACK;
			}else{
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
			}		
			break;
		
		case left:
			turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
			if(isWallonSide(right)){
				back(-40);
				straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				do_back = INT_BACK;
			}else{
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
			}		
			break;
		
		case rear:
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					
			straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
			break;
	}
		accel=SEARCH_ACCEL;				
		con_wall.enable = true;					
		
		len_mouse = 0;					
		MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;		
	
		mypos.dir = glob_nextdir;				


	
	switch(mypos.dir)
	{
		case north:
			mypos.y++;	
			break;
			
		case east:
			mypos.x++;	
			break;
			
		case south:
			mypos.y--;	
			break;
		
		case west:
			mypos.x--;	
			break;

	}

	while((mypos.x != gx) || (mypos.y != gy)){			

		set_wall(mypos.x,mypos.y);					

		switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		
		{
			case front:
				len_mouse-=SLIP_DIST;
				if(isWallonSide(left) && isWallonSide(right) && sen_fr.is_control && sen_fl.is_control){
					straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
				}else{
					straight_NC(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		
				}
				do_back-=2;
				break;
			
			case right:
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				/*
				if(isWallonSide(front) && sen_fr.is_wall && sen_fl.is_wall){
					running(30.0,1.0,1.5,0.0,false,300);
					back(-15);
				}
				*/
				turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				
				if(isWallonSide(left) && do_back < 0){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
					do_back = INT_BACK;
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
					do_back--;
				}
				break;
			
			case left:
				straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				/*
				if(isWallonSide(front) && sen_fr.is_wall && sen_fl.is_wall){
					running(30.0,1.0,1.5,0.0,false,300);
					back(-15);
				}
				*/
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right) && do_back < 0){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
					do_back = INT_BACK;
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
					do_back--;
				}
				break;
			
			case rear:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		
				
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				
				if(isWallonSide(right) && do_back < 0){
					back(-40);
					straight(25,1.0,1.0,0.0);
					do_back = INT_BACK;
				}
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);
				if(isWallonSide(front)){
					back(-40);
					straight_NC(HALF_SECTION+20,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}else{
					straight_NC(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				}
				do_back-=2;
				break;
		}

		con_wall.enable = true;						
		len_mouse = 0;						
		MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;			
	
		mypos.dir = glob_nextdir;					
		
		
		switch(mypos.dir)
		{
			case north:
				mypos.y++;	
				break;
				
			case east:
				mypos.x++;	
				break;
				
			case south:
				mypos.y--;	
				break;
			
			case west:
				mypos.x--;	
				break;

		}
		
	}
	set_wall(mypos.x,mypos.y);		
	straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);	
}

