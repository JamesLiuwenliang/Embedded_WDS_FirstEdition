
static double g_kx;
static double g_ky;

static int g_ts_xc , g_ts_yc;
static int g_lcd_xc,g_lcd_yc;

static int g_ts_xy_swap = 0;

static unsigned int fb_base;
static int xres, yres, bpp;


void get_calibrate_point_data(int lcd_x,int lcd_y,int *px,int*py)
{
	int pressure ;
	int x,y;
	fb_disp_cross(lcd_x, lcd_y ,0xffffff);
	
	/*等待点击*/


	do{
		ts_read_raw(&x,&y,&pressure);
	}while(pressure == 0);

	
	do{
		*px = x; /*为的是使px,py仍然读到的是上一次的有效值,避免抖动造成的影响*/
		*py = y;
		
		ts_read_raw(&x,&y,&pressure);/*pressure 为0的时候表示松开,不为0的话就认为是笔还在此次按的过程*/
		/*当出现第一个十字架之后,再轻碰一下,就会执行完之后的操作,因为触摸屏会误认为完成所有操作*/

		
		printf("get raw data: x = %08d, y = %08d\n\r", x, y);

	}while(pressure);
	/*直到松开才返回*/


	printf("return raw data: x = %08d, y = %08d\n\r", *px, *py);

	fb_disp_cross(lcd_x, lcd_y ,0);


	
	
}


/*判断屏幕是否反过来*/
int is_ts_xy_swap (int a_ts_x,int a_ts_y,int b_ts_x,int b_ts_y)
{
	int dx = b_ts_x - a_ts_x;
	int dy = b_ts_y - a_ts_y;

	if(dx<0)
		dx = 0-dx;
	if(dy<0)
		dy = 0-dy;/*没有反转的情况下,这个数应该是趋近于0*/

	if(dx>dy)
		return 1; /*xy没有反转*/
	else
		return 0; /*xy有反转*/
}

void swap_xy(int *x,int *y)
{
	int tmp ;
	tmp = *x;
	*x = *y;
	*y = tmp;
}



/*
----------------------------
|                          |
|  +(A)              (B)+  |
|                          |
|                          |
|                          |
|            +(E)          |
|                          |
|                          |
|                          |
|  +(D)              (C)+  |
|                          |
----------------------------
*/


void ts_calibrate(void)
{
	

	/*从触摸屏上获得的数据保存在下列参数中*/
	int a_ts_x,a_ts_y; 
	int b_ts_x,b_ts_y;
	int c_ts_x,c_ts_y;
	int d_ts_x,d_ts_y;
	int e_ts_x,e_ts_y;

	/*x轴方向*/
	int ts_s1,ts_s2;
	int lcd_s;

	/*y轴方向*/
	int ts_d1,ts_d2;
	int lcd_d;

	
	/* 先获得LCD参数: unsigned int *fb_base,int *xres ,int *yres,int *bpp  */
	get_lcd_params(&fb_base,&xres,&yres,&bpp);


	/*对于ABCDE循环操作,显示"+",点击,读ts原始值*/

	/*A(50,50)*/
	get_calibrate_point_data(50, 50,&a_ts_x,&a_ts_y );

	/*B(xres-50,50)*/
	get_calibrate_point_data(xres-50, 50,&b_ts_x,&b_ts_y );
	
	/*C(xres-50,yres-50)*/
	get_calibrate_point_data(xres-50, yres-50,&c_ts_x,&c_ts_y );

	/*D(50,yres-50)*/
	get_calibrate_point_data(50, yres-50,&d_ts_x,&d_ts_y );

	/*E(xres/2,yres)*/
	get_calibrate_point_data(xres/2, yres/2,&e_ts_x,&e_ts_y );

	/*确认触摸屏有没有反转*/
	if( is_ts_xy_swap( a_ts_x, a_ts_y,b_ts_x, b_ts_y) == 0)
	{
		g_ts_xy_swap = 1;

		/*对调所有点的xy坐标*/
		swap_xy(&a_ts_x,&a_ts_y);
		swap_xy(&b_ts_x,&b_ts_y);
		swap_xy(&c_ts_x,&c_ts_y);
		swap_xy(&d_ts_x,&d_ts_y);
		swap_xy(&e_ts_x,&e_ts_y);


	}

	/*确认公式的参数并保存*/
	ts_s1 = b_ts_x - a_ts_x;
	ts_s2 = c_ts_x - d_ts_x;
	lcd_s = xres - 50 -50;

	ts_d1 = d_ts_y - a_ts_y;
	ts_d2 = c_ts_y - b_ts_y;
	lcd_d = yres - 50 - 50;

	g_kx = ((double)(2*lcd_s)) / (ts_s1 + ts_s2);
	g_ky = ((double)(2*lcd_d)) / (ts_d1 + ts_d2);

	g_ts_xc = e_ts_x;
	g_ts_yc = e_ts_y;

	g_lcd_xc = xres/2;
	g_lcd_yc = yres/2;
	
}


/*	
 * 读ts原始数据,转换为LCD坐标
 */
int ts_read(int *lcd_x,int *lcd_y,int *lcd_pressure)
{
	int ts_x,ts_y , ts_pressure;
	int tmp_x,tmp_y ;
	ts_read_raw(&ts_x, &ts_y,&ts_pressure);

	if(g_ts_xy_swap)
	{	
		swap_xy(&ts_x,&ts_y);
	}

	/*使用公式计算*/
	tmp_x = g_kx * (ts_x - g_ts_xc) + g_lcd_xc;
	tmp_y = g_ky * (ts_y - g_ts_yc) + g_lcd_yc;



	if(tmp_x<0 || tmp_x >= xres || tmp_y<0 || tmp_y >= yres)
		return -1;

	


	*lcd_x = tmp_x;
	*lcd_y = tmp_y;
	*lcd_pressure = ts_pressure;
	return 0;
}

