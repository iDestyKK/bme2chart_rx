#include "bme.h"

using namespace std;

int ToReal(string str) {
	istringstream output;
	output.clear();
	output.str(str);
	int i;
	output >> i;
	return i;
}

double ToDouble(string str) {
	istringstream output;
	output.clear();
	output.str(str);
	double i;
	output >> i;
	return i;
}

int HexToInt(string str) {
	unsigned int x;
	stringstream ss;
	ss << hex << str;
	ss >> x;
	return x;
}

string ToString(int integer) {
	string res;
	ostringstream convert;
	convert << integer;
	return convert.str();
}

//AUDIO
AUDIO::AUDIO() {
	attr = "";
	path = "";
}

AUDIO::AUDIO(string a, string b) {
	attr = a;
	path = b;
}

//BPM
BPM::BPM() {
	attr = "";
	bpm = 0;
}

BPM::BPM(string a, int b) {
	attr = a;
	bpm = b;
}

BPM::BPM(int a, int b) {
	tick = a;
	bpm = b;
}

//NOTE
NOTE::NOTE() {
	tick = 0;
	colour = 0;
	sound = 0;
}

NOTE::NOTE(int a, int b, int c) {
	tick = a;
	colour = b;
	sound = c;
}

bool NOTE::operator<(NOTE &rhs) {
	if (tick == rhs.tick)
		return (colour < rhs.colour);
	return (tick < rhs.tick);
}

//STRETCHER
STRETCHER::STRETCHER() {
	measure = 0;
	amount = 1;
}

STRETCHER::STRETCHER(int a, double b) {
	measure = a;
	amount = b;
}

//BME
BME::BME() {
	path = "";
}

BME::BME(string a) {
	path = a;
}

string BME::path帰る() {
	return path;
}

void BME::set_path(string a) {
	path = a;
}

void BME::soundfont加える(AUDIO inst) {
	soundfont.push_back(inst);
}

int BME::getNote(int type) {
	switch (type) {
		case 11: return 0;
		case 12: return 1;
		case 13: return 2;
		case 14: return 3;
		case 15: return 4;
		case 18: return 5;
		case 19: return 6;
		case 16: return 7;
		case  1: return 8;
		default: return -1;
	}
}

int BME::calcTick(int measure, int 位置, int length, int resolution) {
	return (measure位置[measure]) + ((resolution * 4) * ((double)位置 / length) * measure_stretch[measure]);
}

int BME::findSound(vector<AUDIO> &sndbk, string ID) {
	for (int i = 0; i < sndbk.size(); i++)
		if (ID == sndbk[i].attr)
			return i;
	return -1;
}

void BME::addNotes(int measure, int 色, string events, int resolution) {
	int event_num = events.length() / 2;
	string ev_str;
	for (int i = 0; i < event_num; i++) {
		ev_str = events.substr(i * 2, 2);
		if (ev_str != "00") {
			notes.push_back(NOTE(calcTick(measure, i, event_num, resolution), 色, findSound(soundfont, ev_str)));
		}
	}
}

void BME::display_notes() {
	for (int i = 0; i < notes.size(); i++) {
		cout << notes[i].tick << " = N " << notes[i].colour << " " << notes[i].sound << endl;
	}
}

void BME::sort_notes() {
	sort(notes.begin(), notes.end());
}

void BME::set_stretchers(vector<STRETCHER> &_stretchers) {
	stretchers = _stretchers;
}

void BME::process_measure位置(int resolution) {
	measure位置.clear();
	measure_stretch.clear();
	measure_stretch.resize(1000, 1);

	int pos = 0;
	for (int i = 0; i < stretchers.size(); i++) {
		measure_stretch[stretchers[i].measure] = stretchers[i].amount;
	}

	for (int i = 0; i < 1000; i++) {
		//BMEのMeasureリミットは999個のMeasureです。
		measure位置.push_back(pos);
		double stretch = measure_stretch[i];
		pos += (resolution * 4) * stretch;
	}
}

void BME::process_chart(int resolution) {
	//BMEのファイルを開けて読みます。
	ifstream fp(path.c_str());
	if (fp.fail()) {
		cerr << "File Failed to be opened" << endl;
	}

	process_measure位置(resolution);

	string line, prtln, bfln, afln, attln, bycol, events;
	int measure, attribute, ev_size;

	while (getline(fp,line)) {
		if (line.length() == 0)
			continue;

		if (line[0] == '#') {
			//EVENT!
			prtln = line.substr(1, 3);
			attln = line.substr(4, 2);
			bycol = line.substr(7, line.length() - 7);
			bfln  = line.substr(1, line.find(' ') - 1);
			afln  = line.substr(line.find(' ') + 1, line.length() - (line.find(' ') + 1));

			if (prtln == "WAV" || prtln == "BMP" || bfln == "BPM" || prtln == "BPM" || bfln == "RANK" || bfln == "TOTAL" || bfln == "STAGEFILE" || bfln == "TITLE"|| bfln == "ARTIST" || bfln == "GENRE" || bfln == "PLAYLEVEL" || bfln == "PLAYER") {
				//しません。
			} else {
				measure   = ToReal(prtln);
				attribute = ToReal(attln);
				events = bycol;
				ev_size = events.length() / 2;
				if ((attribute >= 11 && attribute <= 16) || attribute == 18 || attribute == 19 || attribute == 1) {
					//ノートです。
					addNotes(measure, getNote(attribute), events, resolution);
				}
			}
		}
	}

	fp.close();

	sort_notes();
	//display_notes();
}

void BME::process_chart(string a, int resolution) {
	path = a;
	process_chart(resolution); //笑い。
}

void BME::write_chart_to_file(ofstream &op) {
	for (int i = 0; i < notes.size(); i++) {
		if (notes[i].colour == 8)
			op << "\t" << notes[i].tick << " = A " << notes[i].sound << endl;
		else
			op << "\t" << notes[i].tick << " = N " << notes[i].colour << " 0 " << notes[i].sound << endl;
	}
}

void BME::write_soundfont_to_file(ofstream &op) {
	for (int i = 0; i < soundfont.size(); i++)
		op << soundfont[i].path << endl;
}