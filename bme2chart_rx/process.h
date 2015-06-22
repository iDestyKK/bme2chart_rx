#ifndef PROCESS_H
#define PROCESS_H

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <map>

#include "bme.h"

using namespace std;

//Fun stuff :)
void generate_INI      (string, vector<string> &, vector<BME> &, vector<BPM> &, vector<BPM> &, const string &, string &);
void generate_keysounds(        vector<string> &, vector<BME> &, vector<BPM> &, const string &);
void generate_BPM      (        vector<string> &, vector<BME> &, vector<BPM> &, vector<BPM> &, vector<STRETCHER> &, const string &);

#endif