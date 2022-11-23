#include "search_algo.h"
#include "../iodefine.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "Definations/macro.h"
#include "glob_var.h"

void init_map(int x, int y)
{
	int i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)		//迷路の大きさ分ループ(x座標)
	{
		for(j = 0; j < MAZESIZE_Y; j++)	//迷路の大きさ分ループ(y座標)
		{
			map[i][j] = 255;	//すべて255で埋める。マップの初期化
		}
	}
	
	map[x][y] = 0;				//ゴール座標の歩数を０に設定
	
}

void make_map(int x, int y, int mask)	//歩数マップを作成する
{

	int i,j;
	t_bool change_flag;			//Map作成終了を見極めるためのフラグ

	init_map(x,y);				//Mapを初期化する

	do
	{
		change_flag = false;				//変更がなかった場合にはループを抜ける
		for(i = 0; i < MAZESIZE_X; i++)			//迷路の大きさ分ループ(x座標)
		{
			for(j = 0; j < MAZESIZE_Y; j++)		//迷路の大きさ分ループ(y座標)
			{
				if(map[i][j] == 255)		//255の場合は次へ。ゴール座標からどんどんと広がっていくイメージ
				{
					continue;
				}
				
				if(j < MAZESIZE_Y-1)					//範囲チェック
				{
					if( (wall[i][j].north & mask) == NOWALL)	//壁がなければ(maskの意味はstatic_parametersを参照)
					{
						if(map[i][j+1] == 255)			//まだ値が入っていなければ
						{
							map[i][j+1] = map[i][j] + 1;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
				}
			
				if(i < MAZESIZE_X-1)					//範囲チェック
				{
					if( (wall[i][j].east & mask) == NOWALL)		//壁がなければ
					{
						if(map[i+1][j] == 255)			//値が入っていなければ
						{
							map[i+1][j] = map[i][j] + 1;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
				}
			
				if(j > 0)						//範囲チェック
				{
					if( (wall[i][j].south & mask) == NOWALL)	//壁がなければ
					{
						if(map[i][j-1] == 255)			//値が入っていなければ
						{
							map[i][j-1] = map[i][j] + 1;	//値を代入
							change_flag = true;		//値が更新されたことを示す
						}
					}
				}
			
				if(i > 0)						//範囲チェック
				{
					if( (wall[i][j].west & mask) == NOWALL)		//壁がなければ
					{
						if(map[i-1][j] == 255)			//値が入っていなければ
						{
							map[i-1][j] = map[i][j] + 1;	//値を代入	
							change_flag = true;		//値が更新されたことを示す
						}
						
					}
					
				}
				
			}
			
		}
		
	}while(change_flag == true);	//全体を作り終わるまで待つ
	
}

void set_wall(int x, int y)
{
	
    char wall_write[4]; //壁情報の保存用

    //自分の方向に応じて書き込むデータを生成
    //mypos.dirにはロボットの向いている絶対方向が保存されている
	//CONV_SEN2WALL()はmacro.hを参照
	wall_write[mypos.dir] = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);
    wall_write[(mypos.dir+1)%4] = CONV_SEN2WALL(sen_r.is_wall);
    wall_write[(mypos.dir+2)%4] = NOWALL;
    wall_write[(mypos.dir+3)%4] = CONV_SEN2WALL(sen_l.is_wall);
	
	wall[x][y].north = wall_write[north];	//実際に壁情報を書き込み
	wall[x][y].east  = wall_write[east];	//実際に壁情報を書き込み
    wall[x][y].south = wall_write[south];	//実際に壁情報を書き込み
	wall[x][y].west  = wall_write[west];	//実際に壁情報を書き込み
	
	if(y < MAZESIZE_Y-1)	//範囲チェック
	{
		wall[x][y+1].south = wall_write[north];	//反対側から見た壁を書き込み
	}
	
	if(x < MAZESIZE_X-1)	//範囲チェック
	{
		wall[x+1][y].west = wall_write[east];	//反対側から見た壁を書き込み
	}
	
	if(y > 0)	//範囲チェック
	{
		wall[x][y-1].north = wall_write[south];	//反対側から見た壁を書き込み
	}
	
	if(x > 0)	//範囲チェック
	{
		wall[x-1][y].east = wall_write[west];	//反対側から見た壁を書き込み
	}
	
}

t_bool is_unknown(int x, int y)
{
	
	if((wall[x][y].north == UNKNOWN) || (wall[x][y].east == UNKNOWN) || (wall[x][y].south == UNKNOWN) || (wall[x][y].west == UNKNOWN))
	{			//どこかの壁情報が不明のままであれば
		return true;	//未探索
	}
	else
	{
		return false;	//探索済
	}
}

int get_priority(int x, int y, t_direction dir)	//そのマスの情報から、優先度を算出する
{

	int priority;	//優先度を記録する変数
	
	priority = 0;
	
	if(mypos.dir == dir)				//行きたい方向が現在の進行方向と同じ場合
	{
		priority = 2;
	}
	else if( ((4+mypos.dir-dir)%4) == 2)		//行きたい方向が現在の進行方向と逆の場合
	{
		priority = 0;
	}
	else						//それ以外(左右どちらか)の場合
	{
		priority = 1;
	}
	
	
	if(is_unknown(x,y) == true)
	{
		priority += 4;				//未探索の場合優先度をさらに付加
	}
	
	return priority;				//優先度を返す
	
}


int get_nextdir(int x, int y, int mask, t_direction *dir)	
{
	int little,priority,tmp_priority;		//最小の値を探すために使用する変数


	make_map(x,y,mask);				//歩数Map生成
	little = 255;					//最小歩数を255歩(mapがunsigned char型なので)に設定	

	priority = 0;					//優先度の初期値は0
	
		//maskの意味はstatic_parameter.hを参照
	if( (wall[mypos.x][mypos.y].north & mask) == NOWALL)			//北に壁がなければ
	{
		tmp_priority = get_priority(mypos.x, mypos.y + 1, north);	//優先度を算出
		if(map[mypos.x][mypos.y+1] < little)				//一番歩数が小さい方向を見つける
		{
			little = map[mypos.x][mypos.y+1];			//ひとまず北が歩数が小さい事にする
			*dir = north;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if(map[mypos.x][mypos.y+1] == little)			//歩数が同じ場合は優先度から判断する
		{
			if(priority <= tmp_priority )				//優先度を評価
			{
				*dir = north;					//方向を更新
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].east & mask) == NOWALL)			//東に壁がなければ
	{
		tmp_priority = get_priority(mypos.x + 1, mypos.y, east);	//優先度を算出
		if(map[mypos.x + 1][mypos.y] < little)				//一番歩数が小さい方向を見つける
		{
			little = map[mypos.x+1][mypos.y];			//ひとまず東が歩数が小さい事にする
			*dir = east;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if(map[mypos.x + 1][mypos.y] == little)			//歩数が同じ場合、優先度から判断
		{
			if(priority <= tmp_priority)				//優先度を評価
			{
				*dir = east;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].south & mask) == NOWALL)			//南に壁がなければ
	{
		tmp_priority = get_priority(mypos.x, mypos.y - 1, south);	//優先度を算出
		if(map[mypos.x][mypos.y - 1] < little)				//一番歩数が小さい方向を見つける
		{
			little = map[mypos.x][mypos.y-1];			//ひとまず南が歩数が小さい事にする
			*dir = south;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if(map[mypos.x][mypos.y - 1] == little)			//歩数が同じ場合、優先度で評価
		{
			if(priority <= tmp_priority)				//優先度を評価
			{
				*dir = south;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].west & mask) == NOWALL)			//西に壁がなければ
	{
		tmp_priority = get_priority(mypos.x - 1, mypos.y, west);	//優先度を算出
		if(map[mypos.x-1][mypos.y] < little)				//一番歩数が小さい方向を見つける
		{
			little = map[mypos.x-1][mypos.y];			//西が歩数が小さい
			*dir = west;						//方向を保存
			priority = tmp_priority;				//優先度を保存
		}
		else if(map[mypos.x - 1][mypos.y] == little)			//歩数が同じ場合、優先度で評価
		{
			if(priority <= tmp_priority)				//優先度を評価
			{
				*dir = west;					//方向を保存
				priority = tmp_priority;			//優先度を保存
			}
		}
	}
	//演算の意味はmytyedef.h内のenum宣言から。どっちに向かうべきかを返す
	return ( (int)( ( 4 + *dir - mypos.dir) % 4 ) );
}



void search_adachi(int gx, int gy)
{
	t_direction glob_nextdir;					//次に向かう方向を記録する変数

	accel=SEARCH_ACCEL;

	switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		//次に行く方向を戻り値とする関数を呼ぶ
	{
		case front:
			
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//半区画進む
			break;
		
		case right:
			turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				//右に曲がって
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//半区画進む
			break;
		
		case left:
			turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				//左に曲がって
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//半区画進む
			break;
		
		case rear:
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					//180ターン
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//半区画進む
			break;
	}

	accel=SEARCH_ACCEL;				//加速度を設定
	con_wall.enable = true;					//壁制御を有効にする
	len_mouse = 0;					//進んだ距離カウント用変数をリセット
	MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;		//カウントスタート

	mypos.dir = glob_nextdir;				//方向を更新


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

	
	while((mypos.x != gx) || (mypos.y != gy)){				//ゴールするまで繰り返す

		set_wall(mypos.x,mypos.y);					//壁をセット

		switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		//次に行く方向を戻り値とする関数を呼ぶ
		{
			case front:

				straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//半区画進む
				break;
			
			case right:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//半区画進む
				turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				//右に曲がって
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
			
			case left:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//半区画進む
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				//左に曲がって
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
			
			case rear:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//半区画進む
				turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					//180ターン
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
		}

		con_wall.enable = true;						//壁制御を有効にする
		len_mouse = 0;						//進んだ距離をカウントする変数をリセット
		MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;			//カウントスタート
	
		mypos.dir = glob_nextdir;					//方向を更新
		
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
	set_wall(mypos.x,mypos.y);		//壁をセット
	straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);	
}