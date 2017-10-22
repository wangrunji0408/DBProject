#ifndef FILE_TABLE
#define FILE_TABLE
#include <string>
#include "../utils/MyBitMap.h"
#include <map>
#include <vector>
#include <set>
#include <fstream>
using namespace std;
class FileTable {
private:
	multiset<string> isExist;
	multiset<string> isOpen;
	vector<string> fname;
	vector<string> format;
	map<string, int> nameToID;
	string* idToName;
	MyBitMap* ft, *ff;
	int n;
	void load() {
		ifstream fin("filenames");
		fin >> n;
		for (int i = 0; i < n; ++ i) {
			string s, a;
			fin >> s;
			isExist.insert(s);
			fname.push_back(s);
			fin >> a;
			format.push_back(a);
		}
		fin.close();
	}
	void save() {
		ofstream fout("filenames");
		fout << fname.size() << endl;
		for (uint i = 0; i < fname.size(); ++ i) {
			fout << fname[i] << endl;
			fout << format[i] << endl;
		}
		fout.close();
	}
public:
	int newTypeID() {
		int k = ft->findLeftOne();
		ft->setBit(k, 0);
		return k;
	}
	int newFileID(const string& name) {
		int k = ff->findLeftOne();
		ff->setBit(k, 0);
		nameToID[name] = k;
		isOpen.insert(name);
		idToName[k] = name;
		return k;
	}
	bool ifexist(const string& name) {
		return (isExist.find(name) != isExist.end());
	}
	void addFile(const string& name, const string& fm) {
		isExist.insert(name);
		fname.push_back(name);
		format.push_back(fm);
	}
	int getFileID(const string& name) {
		if (isOpen.find(name) == isOpen.end()) {
			return -1;
		}
		return nameToID[name];
	}
	void freeTypeID(int typeID) {
		ft->setBit(typeID, 1);
	}
	void freeFileID(int fileID) {
		ff->setBit(fileID, 1);
		string name = idToName[fileID];
		isOpen.erase(name);
	}
	string getFormat(string name) {
		for (uint i = 0; i < fname.size(); ++ i) {
			if (name == fname[i]) {
				return format[i];
			}
		}
		return "-";
	}
	FileTable(int fn, int tn) {
		load();
		ft = new MyBitMap(tn, 1);
		ff = new MyBitMap(fn, 1);
		idToName = new string[fn];
		isOpen.clear();
	}
	~FileTable() {
		save();
	}
};
#endif
