#include "search_algo.h"
#include "../iodefine.h"
#include "Parameters/static_parameters.h"
#include "Parameters/adjust_parameters.h"
#include "Definations/macro.h"
#include "glob_var.h"

void init_map(int x, int y)
{
	int i,j;
	
	for(i = 0; i < MAZESIZE_X; i++)		//���H�̑傫�������[�v(x���W)
	{
		for(j = 0; j < MAZESIZE_Y; j++)	//���H�̑傫�������[�v(y���W)
		{
			map[i][j] = 255;	//���ׂ�255�Ŗ��߂�B�}�b�v�̏�����
		}
	}
	
	map[x][y] = 0;				//�S�[�����W�̕������O�ɐݒ�
	
}

void make_map(int x, int y, int mask)	//�����}�b�v���쐬����
{

	int i,j;
	t_bool change_flag;			//Map�쐬�I�������ɂ߂邽�߂̃t���O

	init_map(x,y);				//Map������������

	do
	{
		change_flag = false;				//�ύX���Ȃ������ꍇ�ɂ̓��[�v�𔲂���
		for(i = 0; i < MAZESIZE_X; i++)			//���H�̑傫�������[�v(x���W)
		{
			for(j = 0; j < MAZESIZE_Y; j++)		//���H�̑傫�������[�v(y���W)
			{
				if(map[i][j] == 255)		//255�̏ꍇ�͎��ցB�S�[�����W����ǂ�ǂ�ƍL�����Ă����C���[�W
				{
					continue;
				}
				
				if(j < MAZESIZE_Y-1)					//�͈̓`�F�b�N
				{
					if( (wall[i][j].north & mask) == NOWALL)	//�ǂ��Ȃ����(mask�̈Ӗ���static_parameters���Q��)
					{
						if(map[i][j+1] == 255)			//�܂��l�������Ă��Ȃ����
						{
							map[i][j+1] = map[i][j] + 1;	//�l����
							change_flag = true;		//�l���X�V���ꂽ���Ƃ�����
						}
					}
				}
			
				if(i < MAZESIZE_X-1)					//�͈̓`�F�b�N
				{
					if( (wall[i][j].east & mask) == NOWALL)		//�ǂ��Ȃ����
					{
						if(map[i+1][j] == 255)			//�l�������Ă��Ȃ����
						{
							map[i+1][j] = map[i][j] + 1;	//�l����
							change_flag = true;		//�l���X�V���ꂽ���Ƃ�����
						}
					}
				}
			
				if(j > 0)						//�͈̓`�F�b�N
				{
					if( (wall[i][j].south & mask) == NOWALL)	//�ǂ��Ȃ����
					{
						if(map[i][j-1] == 255)			//�l�������Ă��Ȃ����
						{
							map[i][j-1] = map[i][j] + 1;	//�l����
							change_flag = true;		//�l���X�V���ꂽ���Ƃ�����
						}
					}
				}
			
				if(i > 0)						//�͈̓`�F�b�N
				{
					if( (wall[i][j].west & mask) == NOWALL)		//�ǂ��Ȃ����
					{
						if(map[i-1][j] == 255)			//�l�������Ă��Ȃ����
						{
							map[i-1][j] = map[i][j] + 1;	//�l����	
							change_flag = true;		//�l���X�V���ꂽ���Ƃ�����
						}
						
					}
					
				}
				
			}
			
		}
		
	}while(change_flag == true);	//�S�̂����I���܂ő҂�
	
}

void set_wall(int x, int y)
{
	
    char wall_write[4]; //�Ǐ��̕ۑ��p

    //�����̕����ɉ����ď������ރf�[�^�𐶐�
    //mypos.dir�ɂ̓��{�b�g�̌����Ă����Ε������ۑ�����Ă���
	//CONV_SEN2WALL()��macro.h���Q��
	wall_write[mypos.dir] = CONV_SEN2WALL(sen_fr.is_wall || sen_fl.is_wall);
    wall_write[(mypos.dir+1)%4] = CONV_SEN2WALL(sen_r.is_wall);
    wall_write[(mypos.dir+2)%4] = NOWALL;
    wall_write[(mypos.dir+3)%4] = CONV_SEN2WALL(sen_l.is_wall);
	
	wall[x][y].north = wall_write[north];	//���ۂɕǏ�����������
	wall[x][y].east  = wall_write[east];	//���ۂɕǏ�����������
    wall[x][y].south = wall_write[south];	//���ۂɕǏ�����������
	wall[x][y].west  = wall_write[west];	//���ۂɕǏ�����������
	
	if(y < MAZESIZE_Y-1)	//�͈̓`�F�b�N
	{
		wall[x][y+1].south = wall_write[north];	//���Α����猩���ǂ���������
	}
	
	if(x < MAZESIZE_X-1)	//�͈̓`�F�b�N
	{
		wall[x+1][y].west = wall_write[east];	//���Α����猩���ǂ���������
	}
	
	if(y > 0)	//�͈̓`�F�b�N
	{
		wall[x][y-1].north = wall_write[south];	//���Α����猩���ǂ���������
	}
	
	if(x > 0)	//�͈̓`�F�b�N
	{
		wall[x-1][y].east = wall_write[west];	//���Α����猩���ǂ���������
	}
	
}

t_bool is_unknown(int x, int y)
{
	
	if((wall[x][y].north == UNKNOWN) || (wall[x][y].east == UNKNOWN) || (wall[x][y].south == UNKNOWN) || (wall[x][y].west == UNKNOWN))
	{			//�ǂ����̕Ǐ�񂪕s���̂܂܂ł����
		return true;	//���T��
	}
	else
	{
		return false;	//�T����
	}
}

int get_priority(int x, int y, t_direction dir)	//���̃}�X�̏�񂩂�A�D��x���Z�o����
{

	int priority;	//�D��x���L�^����ϐ�
	
	priority = 0;
	
	if(mypos.dir == dir)				//�s���������������݂̐i�s�����Ɠ����ꍇ
	{
		priority = 2;
	}
	else if( ((4+mypos.dir-dir)%4) == 2)		//�s���������������݂̐i�s�����Ƌt�̏ꍇ
	{
		priority = 0;
	}
	else						//����ȊO(���E�ǂ��炩)�̏ꍇ
	{
		priority = 1;
	}
	
	
	if(is_unknown(x,y) == true)
	{
		priority += 4;				//���T���̏ꍇ�D��x������ɕt��
	}
	
	return priority;				//�D��x��Ԃ�
	
}


int get_nextdir(int x, int y, int mask, t_direction *dir)	
{
	int little,priority,tmp_priority;		//�ŏ��̒l��T�����߂Ɏg�p����ϐ�


	make_map(x,y,mask);				//����Map����
	little = 255;					//�ŏ�������255��(map��unsigned char�^�Ȃ̂�)�ɐݒ�	

	priority = 0;					//�D��x�̏����l��0
	
		//mask�̈Ӗ���static_parameter.h���Q��
	if( (wall[mypos.x][mypos.y].north & mask) == NOWALL)			//�k�ɕǂ��Ȃ����
	{
		tmp_priority = get_priority(mypos.x, mypos.y + 1, north);	//�D��x���Z�o
		if(map[mypos.x][mypos.y+1] < little)				//��ԕ�����������������������
		{
			little = map[mypos.x][mypos.y+1];			//�ЂƂ܂��k�����������������ɂ���
			*dir = north;						//������ۑ�
			priority = tmp_priority;				//�D��x��ۑ�
		}
		else if(map[mypos.x][mypos.y+1] == little)			//�����������ꍇ�͗D��x���画�f����
		{
			if(priority <= tmp_priority )				//�D��x��]��
			{
				*dir = north;					//�������X�V
				priority = tmp_priority;			//�D��x��ۑ�
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].east & mask) == NOWALL)			//���ɕǂ��Ȃ����
	{
		tmp_priority = get_priority(mypos.x + 1, mypos.y, east);	//�D��x���Z�o
		if(map[mypos.x + 1][mypos.y] < little)				//��ԕ�����������������������
		{
			little = map[mypos.x+1][mypos.y];			//�ЂƂ܂��������������������ɂ���
			*dir = east;						//������ۑ�
			priority = tmp_priority;				//�D��x��ۑ�
		}
		else if(map[mypos.x + 1][mypos.y] == little)			//�����������ꍇ�A�D��x���画�f
		{
			if(priority <= tmp_priority)				//�D��x��]��
			{
				*dir = east;					//������ۑ�
				priority = tmp_priority;			//�D��x��ۑ�
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].south & mask) == NOWALL)			//��ɕǂ��Ȃ����
	{
		tmp_priority = get_priority(mypos.x, mypos.y - 1, south);	//�D��x���Z�o
		if(map[mypos.x][mypos.y - 1] < little)				//��ԕ�����������������������
		{
			little = map[mypos.x][mypos.y-1];			//�ЂƂ܂��삪���������������ɂ���
			*dir = south;						//������ۑ�
			priority = tmp_priority;				//�D��x��ۑ�
		}
		else if(map[mypos.x][mypos.y - 1] == little)			//�����������ꍇ�A�D��x�ŕ]��
		{
			if(priority <= tmp_priority)				//�D��x��]��
			{
				*dir = south;					//������ۑ�
				priority = tmp_priority;			//�D��x��ۑ�
			}
		}
	}
	
	if( (wall[mypos.x][mypos.y].west & mask) == NOWALL)			//���ɕǂ��Ȃ����
	{
		tmp_priority = get_priority(mypos.x - 1, mypos.y, west);	//�D��x���Z�o
		if(map[mypos.x-1][mypos.y] < little)				//��ԕ�����������������������
		{
			little = map[mypos.x-1][mypos.y];			//����������������
			*dir = west;						//������ۑ�
			priority = tmp_priority;				//�D��x��ۑ�
		}
		else if(map[mypos.x - 1][mypos.y] == little)			//�����������ꍇ�A�D��x�ŕ]��
		{
			if(priority <= tmp_priority)				//�D��x��]��
			{
				*dir = west;					//������ۑ�
				priority = tmp_priority;			//�D��x��ۑ�
			}
		}
	}
	//���Z�̈Ӗ���mytyedef.h����enum�錾����B�ǂ����Ɍ������ׂ�����Ԃ�
	return ( (int)( ( 4 + *dir - mypos.dir) % 4 ) );
}



void search_adachi(int gx, int gy)
{
	t_direction glob_nextdir;					//���Ɍ������������L�^����ϐ�

	accel=SEARCH_ACCEL;

	switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		//���ɍs��������߂�l�Ƃ���֐����Ă�
	{
		case front:
			
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//�����i��
			break;
		
		case right:
			turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				//�E�ɋȂ�����
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//�����i��
			break;
		
		case left:
			turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				//���ɋȂ�����
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//�����i��
			break;
		
		case rear:
			turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					//180�^�[��
			straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//�����i��
			break;
	}

	accel=SEARCH_ACCEL;				//�����x��ݒ�
	con_wall.enable = true;					//�ǐ����L���ɂ���
	len_mouse = 0;					//�i�񂾋����J�E���g�p�ϐ������Z�b�g
	MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;		//�J�E���g�X�^�[�g

	mypos.dir = glob_nextdir;				//�������X�V


	//�����������ɂ���Ď����̍��W���X�V����
	switch(mypos.dir)
	{
		case north:
			mypos.y++;	//�k������������Y���W�𑝂₷
			break;
			
		case east:
			mypos.x++;	//��������������X���W�𑝂₷
			break;
			
		case south:
			mypos.y--;	//�������������Y���W�����炷
			break;
		
		case west:
			mypos.x--;	//�����������Ƃ���X���W�����炷
			break;

	}

	
	while((mypos.x != gx) || (mypos.y != gy)){				//�S�[������܂ŌJ��Ԃ�

		set_wall(mypos.x,mypos.y);					//�ǂ��Z�b�g

		switch(get_nextdir(gx,gy,MASK_SEARCH,&glob_nextdir))		//���ɍs��������߂�l�Ƃ���֐����Ă�
		{
			case front:

				straight(SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);		//�����i��
				break;
			
			case right:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//�����i��
				turn(90,TURN_ACCEL,TURN_SPEED,RIGHT);				//�E�ɋȂ�����
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
			
			case left:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//�����i��
				turn(90,TURN_ACCEL,TURN_SPEED,LEFT);				//���ɋȂ�����
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
			
			case rear:
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);		//�����i��
				turn(180,TURN_ACCEL,TURN_SPEED,RIGHT);					//180�^�[��
				straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,SEARCH_SPEED);
				break;
		}

		con_wall.enable = true;						//�ǐ����L���ɂ���
		len_mouse = 0;						//�i�񂾋������J�E���g����ϐ������Z�b�g
		MTU.TSTR.BIT.CST3 = MTU.TSTR.BIT.CST4 = 1;			//�J�E���g�X�^�[�g
	
		mypos.dir = glob_nextdir;					//�������X�V
		
		//�����������ɂ���Ď����̍��W���X�V����
		switch(mypos.dir)
		{
			case north:
				mypos.y++;	//�k������������Y���W�𑝂₷
				break;
				
			case east:
				mypos.x++;	//��������������X���W�𑝂₷
				break;
				
			case south:
				mypos.y--;	//�������������Y���W�����炷
				break;
			
			case west:
				mypos.x--;	//�����������Ƃ���X���W�����炷
				break;

		}
		
	}
	set_wall(mypos.x,mypos.y);		//�ǂ��Z�b�g
	straight(HALF_SECTION,SEARCH_ACCEL,SEARCH_SPEED,0);	
}