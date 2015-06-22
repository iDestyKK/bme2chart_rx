/*
	BME2CHART - RX版
	について：BMEファイルからChartファイルを変換します。それから、あなたは、Project RXにBMSファイルを開けられます。

		説明書：
			bme2chart_rx.exe [-b PATH] [-n PATH] [-h PATH] [-a PATH] -o PATH
		例：
			bme2chart_rx.exe -n "Plastic_Mind_[N].bme" -h "Plastic_Mind_[H].bme" -o "Plastic_Mind.chart"

	～テイラー利恵 ♥
*/

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>

#include "bme.h"
#include "process.h"

using namespace std;

//難易度を言い切ります。
const string difficulty[4] = { "Easy", "Medium", "Hard", "Expert" };

int main(int argc, char* argv[]) {
	
	vector<string> charts;
	vector<BME> BME表;
	vector<BPM> BPM表;
	vector<BPM> AdvBPM;
	vector<STRETCHER> STRETCHER表;

	string out;

	//4個の難易度とBMEがあります。
	charts.resize(4, "");
	BME表.resize(4);	

	//Argument Parse
	if (argc < 2) {
		cerr << "Usage: " << argv[0] << " [-b PATH] [-n PATH] [-h PATH] [-a PATH] -o PATH" << endl;
		cerr << "ARGUMENT DETAILS: " << endl << "\t-b - BEGINNER Chart\n\t-n - NORMAL Chart\n\t-h - HYPER Chart\n\t-a - ANOTHER Chart\n\t-o - Output Chart File (Default is \"song.chart\")" << endl << endl;
		cerr << "Example of converting chart with only ANOTHER difficulty:" << endl;
		cerr << "\t" << argv[0] << " -a \"chart_a.bme\" -o \"song.chart\"" << endl << endl;
		cerr << "Example of converting chart with HYPER and ANOTHER difficulty:" << endl;
		cerr << "\t" << argv[0] << " -h \"chart_h.bme\" -a \"chart_a.bme\" -o \"song.chart\"" << endl;
		return -1;
	}
	else {
		//Argumentを読みます。
		cout << "Clara's bme2chart for Project RX... \n\nParsing Arguments:\n";
		for (int i = 1; i < argc; i++) {
			if (string(argv[i]) == "-b") {
				cout << "BEGINNER Chart... " << argv[++i] << endl;
				charts[0] = argv[i];
				BME表[0].set_path(charts[0]);
			}
			if (string(argv[i]) == "-n") {
				cout << "NORMAL Chart...   " << argv[++i] << endl;
				charts[1] = argv[i];
				BME表[1].set_path(charts[1]);
			}
			if (string(argv[i]) == "-h") {
				cout << "HYPER Chart...    " << argv[++i] << endl;
				charts[2] = argv[i];
				BME表[2].set_path(charts[2]);
			}
			if (string(argv[i]) == "-a") {
				cout << "ANOTHER Chart...  " << argv[++i] << endl;
				charts[3] = argv[i];
				BME表[3].set_path(charts[3]);
			}
			if (string(argv[i]) == "-o") {
				cout << "Output Path...    " << argv[++i] << endl;
				out = argv[i];
			}
		}
	}

	int resolution = 960;
	string chart_header;
	cout << "\nProcessing..." << endl;

	//KBH_INFO.INIを作ります。
	generate_INI      ("KBH_INFO.INI", charts, BME表, AdvBPM, BPM表, out, chart_header);
	generate_BPM      (                charts, BME表, AdvBPM, BPM表, STRETCHER表, out);
	generate_keysounds(                charts, BME表, AdvBPM, out);

	if (STRETCHER表.size() == 0)
		STRETCHER表.push_back(STRETCHER(0, 1));

	//個々のChartファイルを処理します。
	for (int i = 0; i < BME表.size(); i++) {
		if (BME表[i].path帰る() == "")
			continue;
		cout << "Processing \"" << BME表[i].path帰る() << "\"...";
		BME表[i].set_stretchers(STRETCHER表);
		BME表[i].process_chart(resolution);
		cout << " Done!" << endl;
	}
	
	cout << "\nConversion Successful... Writing to Chart File..." << endl;
	//いよいよ、Chartのファイルを書きます。
	ofstream op(out);

	//音楽のInformationを書きます。
	op << chart_header << endl;

	//BPMチェンジを書きます。
	op << "[SyncTrack]\n{\n";
	op << "\t0 = TS 4" << endl;

	//Stretcherを処理します。
	vector<int> measure位置;
	vector<double> measure_stretch;
	measure_stretch.resize(1000, 1);

	int pos = 0;
	for (int i = 0; i < STRETCHER表.size(); i++) {
		measure_stretch[STRETCHER表[i].measure] = STRETCHER表[i].amount;
	}

	for (int i = 0; i < 1000; i++) {
		//BMEのMeasureリミットは999個のMeasureです。
		measure位置.push_back(pos);
		double stretch = measure_stretch[i];
		pos += (resolution * 4) * stretch;
	}

	for (int i = 0; i < BPM表.size(); i++) {
		int measure = BPM表[i].tick / (resolution * 4);
		int remainder = BPM表[i].tick % (resolution * 4);
		op << "\t" << measure位置[measure] + (remainder * measure_stretch[measure]) << " = B " << BPM表[i].bpm << endl;
	}
	op << "}\n";

	//BMEファイルのEventがありません。
	op << "[Events]\n{\n}\n";

	for (int i = 0; i < BME表.size(); i++) {
		if (BME表[i].path帰る() == "")
			continue;
		cout << "Writing \"" << BME表[i].path帰る() << "\" to \"" << out << "\"... ";

		op << "[" << difficulty[i] << "Beatmania]\n{\n";
		BME表[i].write_chart_to_file(op);
		op << "}\n";

		cout << " Done!" << endl;
	}

	op.close();

	op.open("soundbank.xsd");
	for (int i = 0; i < BME表.size(); i++) {
		if (BME表[i].path帰る() == "")
			continue;
		cout << "Writing Soundfont to \"soundbank.xsd\"... ";

		BME表[i].write_soundfont_to_file(op);

		cout << " Done!" << endl;
		break;
	}
	op.close();
}