// Graph.cpp : コンソール アプリケーションのエントリ ポイントを定義します。
//

#include "stdafx.h"
#include <stdio.h>
#include <iostream>
#include <math.h>
#pragma execution_character_set("utf-8")

struct Question
{
	unsigned int T1;//[K]ケルビン温度
	unsigned int Tb;//[K]ケルビン温度
	double r1;//[m]距離
	double r2;//[m]距離
	double L;//[m]距離
	double lambda;//[W/m*k]熱伝導率
	double h2;//[W/m*k]熱伝導率
};

class Answer
{
private:
	Question q;
public:
	Answer(Question q);
	~Answer();
	void change_r2(double r2);
	double get_r2();
	double printQ();
};

Answer::Answer(Question q)
{
	this->q = q;
}

Answer::~Answer()
{
}

void Answer::change_r2(double r2)
{
	this->q.r2 = r2;
}

double Answer::get_r2()
{
	return this->q.r2;
}

double Answer::printQ()
{
	const double pi = 3.14159265359;

	//分子
	const double numer = 2 * pi * this->q.L * (this->q.T1 - this->q.Tb);
	//分母
	double denom = 1 / (this->q.r2 * this->q.h2) + (1 / this->q.lambda) * log(this->q.r2 / this->q.r1);

	double Q = numer / denom;
	return Q;
}

class printFileCreator
{
private:
	FILE *fp;
	bool error = false;
public:
	printFileCreator(const char *Filename);
	~printFileCreator();
	bool printFileMain(Answer a);
};

printFileCreator::printFileCreator(const char *Filename)
{
	try {
		if (Filename == NULL) {
			wchar_t *e;
			char *message = "ファイル名が無記入です";
			mbstowcs(e, message, sizeof(message));//char->wchar_t
			throw *e;
		}
		fopen_s(&fp, Filename, "w");
		if (fp == nullptr) {
			wchar_t *e;
			char *message = "ファイルが開けません";
			mbstowcs(e, message, sizeof(message));//char->wchar_t
			throw *e;
		}
	}
	catch (wchar_t *e)
	{
		std::cout << *e << std::endl;
		error = true;
	}
}

printFileCreator::~printFileCreator()
{
	//UnInitialized
	fclose(fp);
}

bool printFileCreator::printFileMain(Answer a)
{
	if (error)
	{
		return false;
	}

	//計算をしてtest.batに代入

		//現在のr2
	double current_r2 = a.get_r2();
	//現在のQ
	double current_Q = 0;
	//一回のループでr2の値がいくつ増えるか
	const double loop_up_r2 = 0.1;

	fprintf(fp, "# r2 Q");//ラベルを記入
	while (a.get_r2() <= 12)
	{
		fprintf(fp, "\n");//改行
		current_r2 += loop_up_r2;//r2の値を増やす
		a.change_r2(current_r2);//r2の値をオブジェクトに代入
		current_Q = a.printQ();//オブジェクトにて計算されたQをもらう
		fprintf(fp, "%lf %lf", current_r2, current_Q);//値をファイルに書き込む
	}
	return true;
}

class batFileCreator
{
private:
	FILE *fp;
	bool error = false;
public:
	batFileCreator(const char *Filename);
	~batFileCreator();
	bool createbatFile(const char *printFile);

};

batFileCreator::batFileCreator(const char *Filename)
{
	try {
		if (Filename == NULL) {
			wchar_t *e;
			char *message = "ファイル名が無記入です";
			mbstowcs(e, message, sizeof(message));//char->wchar_t
			throw *e;
		}
		fopen_s(&fp, Filename, "w");
		if (fp == nullptr) {
			wchar_t *e;
			char *message = "ファイルが開けません";
			mbstowcs(e, message, sizeof(message));//char->wchar_t
			throw *e;
		}
	}
	catch (wchar_t *e)
	{
		std::cout << *e << std::endl;
		error = true;
	}
}

batFileCreator::~batFileCreator()
{
	//UnInitialized
	fclose(fp);
}

bool batFileCreator::createbatFile(const char *printFile)
{
	if (error || printFile == NULL)
	{
		return false;
	}
	fprintf(fp, "set encoding utf8\n");//UTF-8に設定
	fprintf(fp, "set xrange[0:12]\n");//x軸の範囲を指定
	fprintf(fp, "set xl \"外壁の距離 r2[m]\"\n");//x軸に名前を付ける
	fprintf(fp, "set yl \"熱量 Q[J]\"\n");//y軸に名前を付ける
	fprintf(fp, "set grid\n");//グリッド線を付ける
	fprintf(fp, "plot \"");//描画
	fprintf(fp, printFile);
	fprintf(fp, "\" title \"Heating Value\" with lp\n");
	return true;
}


int main()
{

	bool error;
	char *Filename = "test.bat";

	//オブジェクトをインスタンスして値を代入
	Question *q = new Question{ 500,300,0.02,0.02,1,20,10 };
	Answer a(*q);
	delete q;

	printFileCreator *printFile = new printFileCreator(Filename);
	error = printFile->printFileMain(a);
	delete printFile;

	if (!error)
	{
		return -1;
	}

	batFileCreator *batFile = new batFileCreator("command.bat");
	error = batFile->createbatFile(Filename);
	delete batFile;

	if (!error)
	{
		return -1;
	}

	//コマンド用batをwgnuplotで開く
	system("C:\\\"Program Files (x86)\"\\gnuplot\\bin\\wgnuplot.exe -persist \"command.bat\"");

	return 0;
}
