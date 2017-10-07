#include <bits/stdc++.h>
using namespace std;

struct attribute{
	string name;
	bool isNominal;
	vector<string> nominal_type;
	double r;
	int nomi; 
	attribute(){
		this->isNominal = false;
		this->r = 0;
		this->nomi = 0;
	}
};

struct treeNode
{
	int feature_id;
	bool isNominal;
	vector<treeNode*> child;
	int nomiVal;
	double threshold;
	int label;
	pair<int,int> cl;
	treeNode(int id,pair<int,int> pii){
		this->feature_id = id;
		this->label = -1;
		this->nomiVal = 0;
		this->threshold = 0;
		this->isNominal = false;
		this->cl = pii;
	}
};

// Declaration of glb_var
vector<attribute> att;
vector<vector<attribute>> data;
int m;

void printData(){
	int cnt = 0;
	for (auto ins : data){
		cout<<"instance# "<<++cnt<<endl;
		for (auto f : ins){
			cout<<"****";
			cout<<f.name<<"   "<<(f.isNominal?"Nominal ":"Real ");
			f.isNominal?(cout<<"value: "<<f.nominal_type[f.nomi]):(cout<<"value: "<<f.r);
			cout<<endl;
		}
	}
}

void readData(string file);
void getAttribute(string line);
void getData(string line);
int findNextComma(string s, int cur);

int main(int argc, char const *argv[])
{
	/* code */
	
	readData("heart_train.arff");
	// printData();
	vector<int> set;
	for (int i = 0 ; i < data.size();i++){
		set.push_back(i);
	}

	return 0;
}

void readData(string file){
	string line;
	ifstream myfile(file);
	if (myfile.is_open()){
		while(getline(myfile,line)){
			string head = line.substr(0,2);
			if (head =="@r") continue;
			if (head =="@a") {
				getAttribute(line);
			}
			else if (head == "@d"){
				continue;
			}
			else {
				getData(line);
			}
		}
		myfile.close();
	}
	else cout<< "file read error"<<endl;
}
 void getAttribute(string line){
 	attribute nAttr = attribute();
 	att.push_back(nAttr);
 	auto& a = att.back();
 	string tmp;
 	stringstream ss(line);
 	ss>>tmp;
 	while(ss>>tmp){
 		// cout<<tmp<<endl;
 		if (tmp[0] == '\''){
 			// cout<<tmp<<endl;
 			a.name = tmp.substr(1,tmp.size()-2);
 		}
 		else if (tmp == "{") a.isNominal = true ;
 		else if (a.isNominal){
 			if (tmp.back() == '}' || tmp.back() == ',') tmp = tmp.substr(0,tmp.size()-1);
 			a.nominal_type.push_back(tmp);
 		}
 	}
}
void getData(string line){
	int feature_c = 0;
	data.push_back(att);
	int i = -1;
	while(i < int(line.size())){
		// cout<<i<<">--<"<<endl;
		int start = i+1;
		i = findNextComma(line,start);
		string tmp = line.substr(start,i-start);
		auto& instance = data.back();
		if (instance[feature_c].isNominal){
			for (int i = 0 ; i < instance[feature_c].nominal_type.size(); i++){
				if (tmp == instance[feature_c].nominal_type[i]){
					instance[feature_c].nomi = i;
					break;
				}
			}
		}
		else {
			instance[feature_c].r = stod(tmp);
		}
		feature_c++;
		if (feature_c > instance.size()) {
			cout<<"Data seems go wrong plz check!"<<endl;
			return;
		} 
	}
}

int findNextComma(string line,int cur){
	for (int i = cur; i < line.size(); i++){
		if (line[i] == ',') return i;
	}
	return line.size();
}

treeNode* MakeSubtree(const vector<int>& set, const unordered_set<int>& candidateSplit){   // p for label_classification pair for this specific pair
	auto C = candidateSplit;
	pair<int,int> p = countLabel(set);
	auto s = FindBestSplit(set,C); 
	double infoGain = 
	if (set.size < m || C.empty() || s < 0 || (p.first == 0 || p.second == 0)){
		treeNode* ch = new treeNode(-1,p);
		ch->label = p.first > p.second ? 0:1 ;
		return ch;
	}
	else {
		 // Note that during the calculation of this function the feature with no information gain and the feature s will be deleted by reference
	}
}
// bool isAllSame(const vector<int>& set){
// 	for (int i = 1; i < set.size; i++){		
// 		if ( data[set[i]].back().nomi != data[set[i-1]].back().nomi) return false;
// 	}
// 	return true;
// }
pair<int,int> countLabel(const vector<int>& set){
	pair<int,int> res;
	for (int i = 0; i < set.size; i++){
		data[set[i]].back().nomi?res.first++:res.second++;
	}
	return res;
}


pair<int,vector<vector<int>>> FindBestSplit(const vector<int>& set, const unordered_set<int>& candidateSplit){
	pair<int,vector<vector<int>>> res;
	double entro = INT_MAX;
	auto parent_p = countLabel(set);
	for (int id : candidateSplit){
		auto E = GetEntropy(id,set);
		if (E.first < entro){
			entro = E.first;
			res.first = id;
			res.second = E.second;
		}
	}
	return res;
}
pair<double,vector<vector<int>>> GetEntropy(int feature_id, const vector<int>& set){
	if (att[feature_id].isNominal){
		int n = att[feature_id].nominal_type.size();
		vector<vector<int>> subsets(n,vector<int>());
		for (auto x : set){
			subsets[data[x][feature_id]].push_back(x);
		}
		double entro = 0 ;
		for (auto subset : subsets){
			auto p = countLabel(subset);
			double r = double(p.first+p.second)/set.size();
			double pr = 1.0*p.first/(p.first+p.second);
			entro += -1*r*(pr*log(pr)/log(2)+(1-pr)*log(1-pr)/log(2));
		}
		return make_pair(entro,subsets);
	}
	else {
		double thr = getThreshold(feature_id,set);
		vector<vector<int>> subsets(2,vector<int>());
		for (auto x : set){
			subsets[(data[x][feature_id]>thr?1:0)].push_back(x);
		}
		double entro = 0 ;
		for (auto subset : subsets){
			auto p = countLabel(subset);
			double r = double(p.first+p.second)/set.size();
			double pr = 1.0*p.first/(p.first+p.second);
			entro += -1*r*(pr*log(pr)/log(2)+(1-pr)*log(1-pr)/log(2));
		}
		return make_pair(entro,subsets);
	}
}
double getThreshold(int feature_id, cosnt vector<int>& set){
	int minV;
	int maxV;
	minV = maxV = data[set[0]][feature_id].r;
	for (auto x : set){
		maxV = max(data[x][feature_id].r,maxV);
		minV = min(data[x][feature_id].r,minV);
	}
	return (double(maxV+minV))/2;
}

