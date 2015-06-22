#ifndef BME_H
#define BME_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>
#include <algorithm>

using namespace std;

//補助のFunction
int ToReal(string);
double ToDouble(string);
int HexToInt(string);
string ToString(int);

//Fun stuff :)
struct AUDIO {
	//構築子
	AUDIO();
	AUDIO(string, string);

	string attr;
	string path;
};

struct BPM {
	//構築子
	BPM(string, int);
	BPM(int, int);
	BPM();

	int tick;
	string attr;
	int bpm;
};

struct STRETCHER {
	//構築子
	STRETCHER(int, double);
	STRETCHER();

	int measure;
	double amount;
};

struct NOTE {
	//構築子
	NOTE();
	NOTE(int, int, int);

	int tick;
	int colour;
	int sound;

	bool operator<(NOTE &);
};

class BME {
	public:
		//構築子
		BME();
		BME(string);
		
		//帰るのFunctionです。
		string path帰る();

		//セットするのFunctionです。
		void set_path(string);
		void set_stretchers(vector<STRETCHER> &);
		void soundfont加える(AUDIO);

		//いよいよ、ProcessするのFunctionです。
		void process_chart(int);
		void process_chart(string, int);
		void write_chart_to_file(ofstream &);
		void write_soundfont_to_file(ofstream &);
		void process_measure位置(int);

	private:
		int getNote(int);
		void addNotes(int, int, string, int);
		int calcTick(int, int, int, int);
		int findSound(vector<AUDIO> &, string);
		void sort_notes();
		void display_notes();

		string path;
		vector<AUDIO> soundfont;
		vector<NOTE > notes;
		vector<STRETCHER> stretchers;
		vector<int> measure位置;
		vector<double> measure_stretch;
};

#endif