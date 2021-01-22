//
//  PNG画像の読み込みサンプル
//
#include <GL/glut.h>
#include <GL/glpng.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <time.h>

#include <windows.h>
#include <mmsystem.h>

#define FILE_NUMBER 8 // 読み込む画像ファイルの個数
#define number 500	  // 作成するブロックの個数

void Display(void);
void Reshape(int, int);
void Timer(int);
void PutSprite(int, int, int, pngInfo *);
void Keyboard(unsigned char, int, int);
void img_load();
void init_gojou();
void next_gojou();
void game_title();
void game_main();
void game_ready();
void game_countdown();
void game_result();
void cursor_title();
void cursor_mode();
void limit_count();
void gojou_check(unsigned char);
void gojou_move();
void move_disp();
void back_disp();
void limit_disp();
void score_disp();
void res_disp();
void init_mdata();

//  PNG画像を読み込むための変数
char file_name[20];

GLuint gojou_img[5];
GLuint num_img[10];
GLuint title_img[5];
GLuint mode_sel_img[5];
GLuint game_ready_img[5];
GLuint countdown_img[5];
GLuint back_img[5];
GLuint res_img[5];
GLuint man_img[5];
GLuint bnum_img[10];

pngInfo gojou_info[5];
pngInfo num_info[10];
pngInfo title_info[5];
pngInfo mode_sel_info[5];
pngInfo game_ready_info[5];
pngInfo countdown_info[5];
pngInfo back_info[5];
pngInfo res_info[5];
pngInfo man_info[5];
pngInfo bnum_info[10];

//　ブロックの順番管理
int gojou[number];

/**** ゲームの状態を管理する変数群 ****/

//　タイトルとか本編とかの切り替えを行う
//	1:タイトル, 2:モード選択画面, 3: 本編(仮)
int status = 1;

//　タイトル画面での選択
// 1: 始める, 2: 遊び方, 3: やめる
int sel = 1;

//　ゲーム時間を格納する引数
//　modeの値によって決定
int limit = 30;

//	ゲームの開始カウントダウンの値を格納する引数
int start_count = 3;

// ゲーム中のスコア管理
// 正： +1 , 誤: -1
int score = 0;

//　時間取得に関する変数
int time_flag = 0;
time_t first_time, second_time;

//　正解,不正解の管理をする変数
int q_flag = 0;

//　コンベア場を動く寿司の座標管理
int m_gojoudata[3][number];

//　配列のデータ数を管理
int data_value = 0;

int main(int argc, char **argv)
{
	//  準備
	srandom(12345);
	glutInit(&argc, argv);
	glutInitWindowSize(1024, 512); // 画面サイズの設定(720 x 405)
	glutCreateWindow("GOJOU");	   //ウィンドウ名
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

	//  テクスチャのアルファチャネルを有効にする設定
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	//	画像の読み込み
	img_load();

	//  コールバック関数の登録
	glutDisplayFunc(Display);
	glutReshapeFunc(Reshape);
	glutKeyboardUpFunc(Keyboard);
	glutTimerFunc(10, Timer, 0);

	//  イベントループ突入
	glutMainLoop();

	return (0);
}

//  ウィンドウの表示内容を更新する
void Display(void)
{
	switch (status)
	{
	case 1: // タイトル画面の表示
		game_title();
		break;

	case 2: //ゲーム開始のカウントダウン
		game_countdown();
		break;

	case 3: //ゲーム本編の表示
		game_main();
		break;

	case 4: //リザルト画面の表示
		game_result();
		break;

	default:
		printf("ERROR\n");
		break;
	}
}

//タイトル画面の表示
void game_title()
{
	glClear(GL_COLOR_BUFFER_BIT);

	//カーソルを表示(selをもとに表示)
	cursor_title();

	glFlush();
	glutSwapBuffers();
}

void game_ready()
{
	limit = 20;

	//ブロック列の生成
	init_gojou();

	//寿司座標の初期化
	init_mdata();

	// スコア引数の初期化
	score = 0;

	// カウント引数の初期化
	start_count = 3;

	// データ個数の初期化
	data_value = 0;

	status = 2;
}

void game_countdown()
{
	switch (start_count)
	{
	case 0:
		glClear(GL_COLOR_BUFFER_BIT);
		PutSprite(countdown_img[3], 0, 0, &countdown_info[3]);
		glFlush();
		glutSwapBuffers();
		sleep(1);
		status = 3; //ゲーム本編に移動
		break;

	case 1:
		glClear(GL_COLOR_BUFFER_BIT);
		PutSprite(countdown_img[0], 0, 0, &countdown_info[0]);
		glFlush();
		glutSwapBuffers();
		sleep(1);
		start_count--;
		break;

	case 2:
		glClear(GL_COLOR_BUFFER_BIT);
		PutSprite(countdown_img[1], 0, 0, &countdown_info[1]);
		glFlush();
		glutSwapBuffers();
		sleep(1);
		start_count--;
		break;

	case 3:
		glClear(GL_COLOR_BUFFER_BIT);
		PutSprite(countdown_img[2], 0, 0, &countdown_info[2]);
		glFlush();
		glutSwapBuffers();
		sleep(1);
		start_count--;
		break;

	default:
		break;
	}
}

//ゲーム本編の表示内容
void game_main()
{
	int x, y; //  PNG画像を置く座標

	glClear(GL_COLOR_BUFFER_BIT);

	// ブロックの置き始め座標の設定
	x = 400;
	y = 110;

	//　表示
	back_disp();

	switch (gojou[0])
	{
	case 1:
		PutSprite(gojou_img[4], x, y, &gojou_info[4]); // gojou5.png(マグロ) を配置
		break;

	case 2:
		PutSprite(gojou_img[1], x, y, &gojou_info[1]); // gojou2.png(寒ブリ) を配置
		break;

	default:
		break;
	}

	move_disp();
	score_disp();
	limit_disp();

	glFlush();
	glutSwapBuffers();

	//	時間関連
	//　最初の一回目
	if (time_flag == 0)
	{
		first_time = time(NULL);
		time_flag = 1;
	}

	second_time = time(NULL);

	if ((second_time - first_time) >= 1)
	{
		limit_count();
		// 一秒ごとに正誤表示とカウントダウンの管理をする変数を初期化する
		q_flag = 0;
		time_flag = 0;
	}
}

void limit_count()
{
	if (limit > 0)
	{
		// sleep(1);
		limit--;
	}
	else if (limit == 0)
	{
		printf("time over.\n");
		PutSprite(res_img[0], 0, 0, &res_info[0]); // 終了画面の表示
		glFlush();
		glutSwapBuffers();
		sleep(2);

		status = 4; //リザルト画面に移動
	}
}

void game_result()
{
	PutSprite(res_img[1], 0, 0, &res_info[1]);
	res_disp();

	glFlush();
	glutSwapBuffers();
}

void gojou_check(unsigned char key)
{
	switch (key)
	{
	case 'j':
		if (gojou[0] == 1)
		{
			score++;
			q_flag = 1;
			PlaySound("true.wav", NULL, SND_ASYNC);
			gojou_move();
		}
		else
		{
			if (score > 0)
			{
				score--;
			}
			q_flag = 2;
			PlaySound("false.wav", NULL, SND_ASYNC);
			printf("キーが違います\n");
		}
		break;

	case 'f':
		if (gojou[0] == 2)
		{
			PlaySound("true.wav", NULL, SND_ASYNC);
			score++;
			q_flag = 1;
			gojou_move();
		}
		else
		{
			if (score > 0)
			{
				score--;
			}
			q_flag = 2;
			PlaySound("false.wav", NULL, SND_ASYNC);
			printf("キーが違います\n");
		}
		break;

	default:
		q_flag = 0;
		printf("ERROR\n");
		break;
	}
}

void gojou_move()
{
	m_gojoudata[0][data_value] = gojou[0]; // 寿司の種類
	m_gojoudata[1][data_value] = 400;	   // x座標
	m_gojoudata[2][data_value] = 110;	   // y座標

	data_value++; // データの個数を更新

	next_gojou(); //次の寿司を表示
}

void move_disp()
{
	int i;

	for (i = 0; i < data_value; i++)
	{
		switch (m_gojoudata[0][i])
		{
		case 1: //さとる
			if (m_gojoudata[1][i] < 1024)
			{
				m_gojoudata[1][i] += 3;
			}
			PutSprite(gojou_img[4], m_gojoudata[1][i], m_gojoudata[2][i], &gojou_info[4]);
			break;

		case 2: //まさる
			if (m_gojoudata[2][i] > -80)
			{
				m_gojoudata[1][i] -= 3;
			}
			PutSprite(gojou_img[1], m_gojoudata[1][i], m_gojoudata[2][i], &gojou_info[1]);
			break;

		default:
			break;
		}
	}
}

void back_disp() { PutSprite(back_img[0], 0, 0, &back_info[0]); }

void limit_disp()
{
	// 十の位
	int val1 = limit / 10;
	//　一の位
	int val2 = limit % 10;

	PutSprite(num_img[val1], 880, 90, &num_info[val1]);
	PutSprite(num_img[val2], 930, 90, &num_info[val2]);
}

void score_disp()
{
	// 十の位
	int val1 = score / 10;
	//　一の位
	int val2 = score % 10;

	PutSprite(num_img[val1], 100, 90, &num_info[val1]);
	PutSprite(num_img[val2], 150, 90, &num_info[val2]);
}

void res_disp()
{
	// 十の位
	int val1 = score / 10;
	//　一の位
	int val2 = score % 10;

	if (score > 99)
	{
		val1 = 9;
		val2 = 9;
	}

	PutSprite(bnum_img[val1], 450, 200, &bnum_info[val1]);
	PutSprite(bnum_img[val2], 530, 200, &bnum_info[val2]);
}

// タイトル画面でのカーソル移動
void cursor_title()
{
	//カーソルを表示する座標の設定
	int x = 0;
	int y = 0;

	switch (sel)
	{
	case 1:
		PutSprite(title_img[0], x, y, &title_info[0]);
		break;

	case 2:
		PutSprite(title_img[1], x, y, &title_info[1]);
		break;

	case 3:
		PutSprite(title_img[2], x, y, &title_info[2]);
		break;

	default:
		printf("ERROR");
		break;
	}
}

// キー入力の処理
void Keyboard(unsigned char key, int x, int y)
{
	//タイトル画面にいるときの操作
	if (status == 1)
	{
		// ゲーム開始
		if (key == ' ' && sel == 1)
		{
			printf("game start.\n");
			PlaySound("start.wav", NULL, SND_ASYNC);
			game_ready();
		}

		//  ゲーム終了
		if (key == ' ' && sel == 2)
		{
			exit(0);
		}

		//カーソル上移動
		if (key == 'w' && sel > 1)
		{
			PlaySound("startmenu.wav", NULL, SND_ASYNC);
			sel--;
		}

		//カーソル下移動
		if (key == 's' && sel < 2)
		{
			PlaySound("exit.wav", NULL, SND_ASYNC);
			sel++;
		}
	}
	else if (status == 2)
	{
		if (key == 'q')
		{
			status = 4;
		}
	}
	// ゲーム中の操作
	else if (status == 3)
	{
		if (key == 'j')
		{
			gojou_check(key);
		}

		if (key == 'f')
		{
			gojou_check(key);
		}

		if (key == 'k')
		{
			gojou_check(key);
		}

		if (key == 'd')
		{
			gojou_check(key);
		}
	}
	else if (status == 4)
	{
		if (key == ' ')
		{
			printf("タイトル画面に戻ります.\n");
			status = 1;
		}
	}

	//　現在の位置の表示
	// printf("(x, y) = (%d, %d)\n", i, j);
}

void init_mdata()
{
	int i, j;

	for (j = 0; j < number; j++)
	{
		for (i = 0; i < 3; i++)
		{
			m_gojoudata[i][j] = 0;
		}
	}

	printf("finish\n");
}

// 新しい五条を用意するのに利用
void next_gojou()
{
	int i;
	for (i = 0; i < number - 1; i++)
	{
		gojou[i] = gojou[i + 1]; // 一個ずつずらす
	}

	gojou[number - 1] = 0; // 一番最後のところに0をつめる.
}

// 配置する五条の初期化, ランダムに生成
void init_gojou()
{
	int i;
	srand((unsigned int)time(NULL));

	for (i = 0; i < number; i++)
	{
		gojou[i] = rand() % 2 + 1;
	}
}

//  タイマー割り込みで画面を描画し直す
void Timer(int t)
{
	glutPostRedisplay();
	glutTimerFunc(10, Timer, 0);
}

void img_load()
{
	//	五条の読み込み(何故かgojou1を読み込まない...)
	for (int i = 0; i < 5; i++)
	{
		sprintf(file_name, "image/gojou%d.png", i + 1); //読み込みファイル名注意
		gojou_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &gojou_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	数字画像の読み込み
	for (int i = 0; i < 10; i++)
	{
		sprintf(file_name, "num_file/num_parts%d.png", i); //読み込みファイル名注意

		num_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &num_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	タイトル画面の画像の読み込み
	for (int i = 0; i < 3; i++)
	{
		sprintf(file_name, "image/title%d.png", i + 1); //読み込みファイル名注意

		title_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &title_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	ゲーム開始のカウントダウンの画像の読み込み
	for (int i = 0; i < 4; i++)
	{
		sprintf(file_name, "image/countdown%d.png", i + 1); //読み込みファイル名注意

		countdown_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &countdown_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	ゲーム画面画像の読み込み
	for (int i = 0; i < 1; i++)
	{
		sprintf(file_name, "image/q_back%d.png", i + 1); //読み込みファイル名注意

		back_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &back_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	リザルト画面の読み込み
	for (int i = 0; i < 2; i++)
	{
		sprintf(file_name, "image/res%d.png", i + 1); //読み込みファイル名注意

		res_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &res_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}

	//	リザルト画面で使う数字の読み込み
	for (int i = 0; i < 10; i++)
	{
		sprintf(file_name, "num_file/big_num%d.png", i); //読み込みファイル名注意

		bnum_img[i] = pngBind(file_name, PNG_NOMIPMAP, PNG_ALPHA, &bnum_info[i], GL_CLAMP, GL_NEAREST, GL_NEAREST);
	}
}

//  ウィンドウのサイズ変更が発生したときに座標系を再設定する関数
void Reshape(int w, int h)
{
	glutReshapeWindow(1024, 512);
	glViewport(0, 0, w, h);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluOrtho2D(0, w, 0, h);
	glScaled(1, -1, 1);
	glTranslated(0, -h, 0);
}

//  num番のPNG画像を座標(x,y)に表示する
void PutSprite(int num, int x, int y, pngInfo *info)
{
	int w, h; //  テクスチャの幅と高さ

	w = info->Width; //  テクスチャの幅と高さを取得する
	h = info->Height;

	glPushMatrix();
	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, num);
	glColor4ub(255, 255, 255, 255);

	glBegin(GL_QUADS); //  幅w, 高さhの四角形

	glTexCoord2i(0, 0);
	glVertex2i(x, y);

	glTexCoord2i(0, 1);
	glVertex2i(x, y + h);

	glTexCoord2i(1, 1);
	glVertex2i(x + w, y + h);

	glTexCoord2i(1, 0);
	glVertex2i(x + w, y);

	glEnd();

	glDisable(GL_TEXTURE_2D);
	glPopMatrix();
}
