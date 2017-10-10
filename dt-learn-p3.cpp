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
int m =4;
treeNode* root;
int n_correct = 0;
int n_wrong = 0;
vector<int> v = {2, 5, 10, 20};

void reset(){
	att.clear();
	data.clear();
	n_correct = n_wrong = 0;
}
vector<int> getTrainingSet(double ratio,int size){
	vector<int> res;
	unordered_set<int> us;
	while(us.size() < int(ratio*size)){
		int k = rand()%size;
		us.insert(k);
	}
	for (auto i : us)res.push_back(i);
		return res;
}

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

void readData(string file,bool train);
void getAttribute(string line);
void getData(string line);
void TestData(string line, treeNode* dtTree);
int findNextComma(string s, int cur);
pair<int,int> countLabel( vector<int>& v);
treeNode* MakeSubtree( vector<int>& set,  vector<int>& candidateSplit,int label,int level, pair<int,int> p);
pair<int,vector<vector<int>>> FindBestSplit( vector<int>& set,  vector<int>& candidateSplit,double& thr, pair<int,int> parent_p);
pair<double,vector<vector<int>>> GetEntropy(int feature_id,  vector<int>& set,double& thr);
vector<double> getThreshold(int feature_id,  vector<int>& set);
void run(vector<string>& f){
	// vector<string> f = {"heart_train.arff","heart_test.arff","diabetes_train.arff","diabetes_test.arff"};
	reset();
	readData(f[0],true);
	vector<int> attr;
	for (int i = 0 ; i <att.size()-1;i++){
		attr.push_back(i);
	}
	vector<int> set;
	for (int i = 0 ; i < data.size();i++){
		set.push_back(i);
	}
	for (auto r : v){
		m = r;
		 // double minr = 1;
		 // double maxr = 0;
		 // double mean = 0;
		// for (int i = 0 ; i< 10; i++ ){
		n_wrong = 0;
		n_correct = 0;
			// vector<int> set = getTrainingSet(ratio,data.size());
		auto pa = countLabel(set);
		int o_label = (pa.first < pa.second?1:0);
			
		root = MakeSubtree(set,attr,o_label,0,pa);
			// cout<<"<Predictions for the Test Set Instances>"<<endl;
		readData(f[1],false);
			// cout<<"Number of correctly classified: "<<n_correct<<" Total number of test instances: "<<n_wrong+n_correct<<endl;
		double pr = 1.0*n_correct/(n_correct+n_wrong);
			// cout<<i<<" "<<"correct rate:"<<pr<<endl;
			// minr = min(minr,pr);
			// maxr = max(maxr,pr);
			// mean+=pr;

		// }
		// mean = mean/10;
		cout<<"file name: "<<f[0]<<" m : "<<m<<" correct_rate : "<<pr<<endl;
	}

}
int main(int argc, char const *argv[])
{	
		vector<vector<string>> f = {{"heart_train.arff","heart_test.arff"},{"diabetes_train.arff","diabetes_test.arff"}};
		for (auto fa :f)run(fa);
	// readData(argv[1],true);  // true for training false for test
	// m = atoi(argv[3]);
	// int o_label;
	// vector<int> set;
	// for (int i = 0 ; i < data.size();i++){
	// 	set.push_back(i);
	// // }
	// for (double d : r){
	// 	for (int i = 0 ; i < 10 ; i++){

	// 	}
	// }
	// auto pa = countLabel(set);
	// o_label = (pa.first < pa.second?1:0);
	// vector<int> attr;
	// for (int i = 0 ; i <att.size()-1;i++){
	// 	attr.push_back(i);
	// }
	// root = MakeSubtree(set,attr,o_label,0,pa);
	// cout<<"<Predictions for the Test Set Instances>"<<endl;
	// readData(argv[2],false);
	// cout<<"Number of correctly classified: "<<n_correct<<" Total number of test instances: "<<n_wrong+n_correct<<endl;
	return 0;
}

void readData(string file,bool train){
	int k = 0;
	string line;
	ifstream myfile(file);
	if (myfile.is_open()){
		while(getline(myfile,line)){
			string head = line.substr(0,2);
			if (head =="@r") continue;
			if (head =="@a") {
				if(train) getAttribute(line);
				else continue;
			}
			else if (head == "@d"){
				continue;
			}
			else {
				if(train)getData(line);
				else {
					// cout<<++k<<": ";
					TestData(line,root);
				}
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
void TestData(string line, treeNode* dtTree){
	vector<attribute> instance = att;
	int feature_c = 0;
	int i = -1;
	while(i < int(line.size())){
		// cout<<i<<">--<"<<endl;
		int start = i+1;
		i = findNextComma(line,start);
		string tmp = line.substr(start,i-start);
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
	treeNode* cur = root;
	while(cur->label == -1){
		int f = cur->feature_id;
		if (instance[f].isNominal)cur = cur->child[instance[f].nomi];
		else{
			double v = instance[f].r;
			if (v <= cur->threshold)cur = cur->child[0];
			else cur = cur->child[1];
		}
	}
	instance.back().nomi == cur->label?n_correct++:n_wrong++;
	// cout<<"Actual: "<<att.back().nominal_type[instance.back().nomi]<<" Predicted: "<<att.back().nominal_type[cur->label]<<endl;
}
 
int findNextComma(string line,int cur){
	for (int i = cur; i < line.size(); i++){
		if (line[i] == ',') return i;
	}
	return line.size();
}

treeNode* MakeSubtree(vector<int>& set, vector<int>& C, int label,int level, pair<int,int> p){   
	double thr = 0;
	int nLabel;
	if (p.first == p.second) nLabel = label;
	else {
		if (p.first > p.second) nLabel = 0;
		else nLabel = 1;
	}
	auto s = FindBestSplit(set,C,thr,p); 
	if (set.size() < m || C.empty() || s.first < 0 || (p.first == 0 || p.second == 0)){
		treeNode* ch = new treeNode(-1);
		ch->label = nLabel;
		return ch;
	}
	else {
		// cout<<endl;
		bool flag = false;
		treeNode* ch = new treeNode(s.first);
		if (!att[s.first].isNominal){
			ch->threshold = thr;
			flag = true;
		}
		for (int i = 0 ; i < s.second.size(); i++){
			// for (int j = 0 ; j < level;j++)cout<<"|\t";
			// cout<<att[s.first].name;
			// if (flag){
			// 	cout<<(i?" > ":" <= ");
			// 	cout<<thr;
			// }
			// else cout<<" = "<<att[s.first].nominal_type[i];
			auto np = countLabel(s.second[i]);
			// cout<<" ["<<np.first<<" "<<np.second<<"]";
			auto r = MakeSubtree(s.second[i],C,nLabel,level+1,np);
			// if (r->label > -1) cout<<": "<<att.back().nominal_type[r->label]<<endl;
			ch->child.push_back(r);			
		}
		return ch;
	}
}

pair<int,int> countLabel(vector<int>& set){
	pair<int,int> res = {0,0};
	for (int i = 0; i < set.size(); i++){
		data[set[i]].back().nomi?res.second++:res.first++;
	}
	return res;
}


pair<int,vector<vector<int>>> FindBestSplit(vector<int>& set, vector<int>& candidateSplit, double& thr, pair<int,int> parent_p){
	pair<int,vector<vector<int>>> res;
	double entro = DBL_MAX;
	double p = (double(parent_p.first))/(parent_p.first+parent_p.second);
	double parent_entropy = (-1)*p*log2(p)-(1-p)*log2(1-p);
	for (int id : candidateSplit){
		double cur_th = 0;
		auto E = GetEntropy(id,set,cur_th);
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
		double ResEntro = DBL_MAX;
		vector<vector<int>> ResSets(2,vector<int>());
		for (auto th : candidate){

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
	vector<double> C;
	vector<double> value;
	unordered_map<double,vector<int>> subsets;
	for (auto x : set){
		double val = data[x][feature_id].r;
		if (!subsets.count(val)) value.push_back(val);
		subsets[val].push_back(x);
	}
	sort(value.begin(),value.end());

	for (int i = 1 ; i < value.size();i++){
		auto p1 = countLabel(subsets[value[i-1]]);
		auto p2 = countLabel(subsets[value[i]]);

		if ((p1.first && p2.second) || (p1.second && p2.first)){
			C.push_back((value[i-1] + value[i])/2);
		}
	}

	return C;
}

