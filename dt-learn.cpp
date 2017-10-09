#include <bits/stdc++.h>
using namespace std;
inline double log2(double n){
	return n?log(n)/log(2):0;
}
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
	double threshold;
	int label;
	treeNode(int id){
		this->feature_id = id;
		this->label = -1;
		this->threshold = 0;
		this->isNominal = false;
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
pair<int,int> countLabel( vector<int>& v);
treeNode* MakeSubtree( vector<int>& set,  unordered_set<int>& candidateSplit,int label,int level, pair<int,int> p);
pair<int,vector<vector<int>>> FindBestSplit( vector<int>& set,  unordered_set<int>& candidateSplit,double& thr);
pair<double,vector<vector<int>>> GetEntropy(int feature_id,  vector<int>& set,double& thr);
vector<double> getThreshold(int feature_id,  vector<int>& set);

int main(int argc, char const *argv[])
{
	/* code */
	
	readData("heart_train.arff");
	// printData();
	m = 2;
	int o_label;
	vector<int> set;
	for (int i = 0 ; i < data.size();i++){
		set.push_back(i);
	}
	auto pa = countLabel(set);
	o_label = (pa.first < pa.second?1:0);
	unordered_set<int> attr;
	for (int i = 0 ; i <att.size()-1;i++){
		// cout<<i<<":"<<att[i].name<<(att[i].isNominal?"Nominal":"REAL")<<endl;
		attr.insert(i);
	}
	treeNode* root = MakeSubtree(set,attr,o_label,0,pa);
	// double thr = 0 ;
	// auto t = FindBestSplit(set,attr,thr);

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

treeNode* MakeSubtree(vector<int>& set, unordered_set<int>& C, int label,int level, pair<int,int> p){   
	double thr = 0;
	int nLabel;
	// pair<int,int> p = countLabel(set);
	if (p.first == p.second) nLabel = label;
	else {
		if (p.first > p.second) nLabel = 0;
		else nLabel = 1;
	}
	// cout<<endl;
	auto s = FindBestSplit(set,C,thr); 
	// cout<<"pass findBestSplit"<<endl;
	if (set.size() < m || C.empty() || s.first < 0 || (p.first == 0 || p.second == 0)){
		// cout<<"oops"<<endl;
		treeNode* ch = new treeNode(-1);
		ch->label = nLabel;
		return ch;
	}
	else {
		cout<<endl;
		bool flag = false;
		
		// C.erase(s.first);
		// cout<<"feature_id:"<<s.first<<endl;
		// cout<<"Feature used this time  : "<<att[s.first].name<<" ratio:"<<p.first<<":"<<p.second<<endl;
		treeNode* ch = new treeNode(s.first);
		if (!att[s.first].isNominal){
			ch->threshold = thr;
			flag = true;
		}
		for (int i = 0 ; i < s.second.size(); i++){
			for (int j = 0 ; j < level;j++)cout<<"|\t";
			cout<<att[s.first].name;
			if (flag){
				cout<<(i?" > ":" <= ");
				cout<<thr;
			}
			else cout<<" = "<<att[s.first].nominal_type[i];
			auto np = countLabel(s.second[i]);
			cout<<" ["<<np.first<<" "<<np.second<<"]";
			auto r = MakeSubtree(s.second[i],C,nLabel,level+1,np);
			if (r->label > -1) cout<<": "<<att.back().nominal_type[r->label]<<endl;
			ch->child.push_back(r);			
		}
		return ch;
	}
}
// bool isAllSame(const vector<int>& set){
// 	for (int i = 1; i < set.size; i++){		
// 		if ( data[set[i]].back().nomi != data[set[i-1]].back().nomi) return false;
// 	}
// 	return true;
// }
pair<int,int> countLabel(vector<int>& set){
	pair<int,int> res = {0,0};
	for (int i = 0; i < set.size(); i++){
		data[set[i]].back().nomi?res.second++:res.first++;
	}
	return res;
}


pair<int,vector<vector<int>>> FindBestSplit(vector<int>& set, unordered_set<int>& candidateSplit, double& thr){
	pair<int,vector<vector<int>>> res;
	double entro = DBL_MAX;
	auto parent_p = countLabel(set);
	double p = (double(parent_p.first))/(parent_p.first+parent_p.second);
	double parent_entropy = (-1)*p*log2(p)-(1-p)*log2(1-p);
	for (int id : candidateSplit){
		double cur_th = 0;
		auto E = GetEntropy(id,set,cur_th);
		// cout<<"feature_name:"<<att[id].name<<"  entropy:"<<E.first<<" curTH= "<<cur_th<<endl;
		// cout<<"feature_sub_set_size:"<<endl;
		// for (auto t : E.second){
		// 	cout<<t.size()<<"  ";
		// }
		// cout<<endl;
		if (E.first < entro){
			entro = E.first;
			res.first = id;
			res.second = E.second;
			thr = cur_th;
		}
	}
	if (parent_entropy - entro < 0) res.first = -1;
	return res;
}
pair<double,vector<vector<int>>> GetEntropy(int feature_id, vector<int>& set, double& thr){
	if (att[feature_id].isNominal){
		int n = att[feature_id].nominal_type.size();
		vector<vector<int>> subsets(n,vector<int>());
		for (auto x : set){
			subsets[data[x][feature_id].nomi].push_back(x);
		}
		double entro = 0 ;
		for (auto subset : subsets){
			if (subset.empty()) continue;
			auto p = countLabel(subset);
			double r = double(p.first+p.second)/set.size();
			double pr = (1.0*p.first)/(p.first+p.second);
			entro += (-1)*r*(pr*log2(pr)+(1-pr)*log2(1-pr));
		}
		return make_pair(entro,subsets);
	}
	else {
		vector<double> candidate = getThreshold(feature_id,set);
		// cout<<"threshold candidate size:"<<candidate.size()<<endl;
		// if (set.size()==10)cout<<att[feature_id].name<<endl;
		double ResEntro = DBL_MAX;
		vector<vector<int>> ResSets(2,vector<int>());
		for (auto th : candidate){
			// if (set.size()==10)cout<<"candidate thr:"<<th<<endl;
			vector<vector<int>> subsets(2,vector<int>());
			for (auto x : set){
			int k = data[x][feature_id].r>th?1:0;
			subsets[k].push_back(x);
			}
			double entro = 0 ;
			for (auto subset : subsets){
				if (subset.empty())continue;
				auto p = countLabel(subset);
				double r = (double(p.first+p.second))/set.size();
				double pr = (1.0*p.first)/(p.first+p.second);
				entro += (-1)*r*(pr*log2(pr)+(1-pr)*log2(1-pr));
			}
			// if (set.size()==10)cout<<"entro :"<<entro<<endl;
			if (entro < ResEntro){
				ResEntro = entro;
				ResSets = subsets;
				thr = th;
			}

		}
		return make_pair(ResEntro,ResSets);
	}
}
vector<double> getThreshold(int feature_id, vector<int>& set){
	// if (set.size()==10)cout<<"the feature "<<att[feature_id].name<<" is using a set of size "<<set.size()<<" to compute entropy"<<endl; 
	vector<double> C;
	vector<double> value;
	unordered_map<double,vector<int>> subsets;
	for (auto x : set){
		double val = data[x][feature_id].r;
		if (!subsets.count(val)) value.push_back(val);
		subsets[val].push_back(x);
	}
	sort(value.begin(),value.end());
	// if (set.size()==10){
	// 	cout<<"values are "<<endl;
	// 	for (auto x : value)cout<<x<<" ";
	// 	cout<<endl;
	// }
	for (int i = 1 ; i < value.size();i++){
		auto p1 = countLabel(subsets[value[i-1]]);
		auto p2 = countLabel(subsets[value[i]]);
		// if (set.size()==10){
		// 	cout<<p1.first<<" "<<p1.second<<endl;
		// 	cout<<p2.first<<" "<<p2.second<<endl;
		// }
		if ((p1.first && p2.second) || (p1.second && p2.first)){
			C.push_back((value[i-1] + value[i])/2);
		}
	}
	// if (!C.size()) {
	// 	cout<<"feature_name: "<<att[feature_id].name<<endl;
	// }
	return C;
}

