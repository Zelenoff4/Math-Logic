#include <bits/stdc++.h>
using namespace std;

vector<vector<int> > g, table, lesser;
const int MAXN = 101;
int n, latticeMax, latticeMin;
bool used[MAXN];

/*
	Летят на самолете англичанин, француз, немка и Ржевский. Самолет начинает падать. 
	И тут выясняется, что парашют один на всех. 
	Ржевский начинает надевать парашют на себя. Англичанин и француз: 
		— Поручик, среди нас женщина! 
	Ржевский, задумчиво, глядя на часы: 
		— Думаете, успеем?
*/

void dfs(int v, int where){
	if (used[v]){
		return;
	}
	used[v] = true;
	for (size_t i = 0; i < g[v].size(); i++){
		int to = g[v][i];
		if (find(table[where].begin(), table[where].end(), to) == table[where].end())
			table[where].push_back(to);
		dfs(to, where);
	}
}

bool isSmaller(int a, int b){
	if (find(table[a].begin(), table[a].end(), b) != table[a].end()){
		return true;
	}
	else{
		return false;
	}
}

bool isBigger(int a, int b){
	if (find(lesser[a].begin(), lesser[a].end(), b) != lesser[a].end()){
		return true;
	}
	else{
		return false;
	}
}

int Plus(int a, int b){
	vector<int> wannabeplus;
	for (size_t i = 0; i < table[a].size(); i++){
		if (find(table[b].begin(), table[b].end(), table[a][i]) != table[b].end()){
			wannabeplus.push_back(table[a][i]);
		}
	}

	for (size_t i = 0; i < wannabeplus.size(); i++){
		bool success = false;
		for (size_t j = 0; j < wannabeplus.size(); j++){
			if (!isSmaller(wannabeplus[i], wannabeplus[j])){
				success = true;
				break;
			}
		}
		if (!success){
			return wannabeplus[i];
		}
		
	}
	return -1;
}

int Mul(int a, int b){
	vector<int> wannabemul;
	for (size_t i = 0; i < lesser[a].size(); i++){
		if (find(lesser[b].begin(), lesser[b].end(), lesser[a][i]) != lesser[b].end()){
			wannabemul.push_back(lesser[a][i]);
		}
	}

	for (size_t i = 0; i < wannabemul.size(); i++){
		bool success = false;
		for (size_t j = 0; j < wannabemul.size(); j++){
			if (!isBigger(wannabemul[i], wannabemul[j])){
				success = true;
				break;
			}
		}
		if (!success){
			return wannabemul[i];
		}
	}
	return -1;

}

int Impl(int a, int b){
	vector<int> wannabeimpl;
	for (size_t c = 0; c < n; c++){
		if (isSmaller(Mul(c, a), b)){
			wannabeimpl.push_back(c);
		}
	}
	for (size_t i = 0; i < wannabeimpl.size(); i++){
		bool success = false;
		for (size_t j = 0; j < wannabeimpl.size(); j++){
			if (!isBigger(wannabeimpl[i], wannabeimpl[j])){
				success = true;
				break;
			}
		}
		if (!success){
			return wannabeimpl[i];
		}
	}
	return -1;
}

int defineMax(){
	for (size_t i = 0; i < table.size(); i++){
		if (table[i].size() == 1){
			return i;
		}
	}
	return -1;
}

int defineMin(){
	for (size_t i = 0; i < lesser.size(); i++){
		if (lesser[i].size() == 1){
			return i;
		}
	}
	return -1;
}

bool checkPlus(){
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			if (Plus(i, j) == -1){
				cout << "Операция '+' не определена: " << i + 1 << '+' << j + 1;
				return false;
			}
		}
	}
	return true;
}

bool checkMul(){
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			if (Mul(i, j) == -1){
				cout << "Операция '*' не определена: " << i + 1 << '*' << j + 1;
				return false;
			}
		}
	}
	return true;
}

bool checkDist(){
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			for (size_t k = 0; k < n; k++){
				int left = Mul(i, Plus(j, k)), right = Plus(Mul(i, j), Mul(i, k));
				if (left != right){
					cout << "Нарушается дистрибутивность: " << i + 1 << "*(" << j + 1 << '+' << k + 1 << ")";
					return false;
				}
			}
		}
	}
	return true;
}

bool checkImpl(){
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			if (Impl(i, j) == -1){
				cout << "Операция '->' не определена: " << i + 1 << "->" << j + 1;
				return false;
			}
		}
	}
	return true;
}

bool checkBool(){
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < n; j++){
			if (Plus(i, Impl(i, latticeMin)) != latticeMax){
				cout << "Не булева алгебра: " << i + 1 << "+~" << i + 1;
				return false;
			}
		}
	}
	return true;

}


int parseString(string word){
	string nums = "1234567890";
	int temp = 0, degree = 1;
	for (int k = word.length() - 1; k >= 0; k--){
		if (nums.find(word[k]) == -1)
			continue;
		int ch = word[k] - '0';
		temp += ch * degree;
		degree *= 10;
	}
	return temp;
}

int main(){
	freopen("input.txt", "r", stdin);
	freopen("output.txt", "w", stdout);
	string first;
	getline(cin, first);
	n = parseString(first);
	g.resize(n);
	table.resize(n);
	lesser.resize(n);
	fill(used, used + MAXN, false);
	for (size_t i = 0; i < n; i++){
		string tmp;
		getline(cin, tmp);
		size_t j = 0;
		while (j < tmp.length()){
			string word = "";
			while (j < tmp.length() && tmp[j] != ' '){
				word += tmp[j];
				j++;
			}
			int temp = parseString(word) - 1;
			g[i].push_back(temp);
			j++;
		}

		if (find(g[i].begin(), g[i].end(), i) == g[i].end()) {
			g[i].push_back(i);
		}
		
	}
	for (size_t i = 0; i < n; i++){
		for (size_t j = 0; j < g[i].size(); j++){
			fill(used, used + MAXN, false);
			dfs(g[i][j], i);
		}
	}
	for (size_t k = 0; k < n; k++){
		for (size_t i = 0; i < table.size(); i++){
			if (find(table[i].begin(), table[i].end(), k) != table[i].end()){
				lesser[k].push_back(i);
			}
		}
	}
	latticeMin = defineMin();
	latticeMax = defineMax();
	if (checkPlus()){
		if (checkMul()){
			if (checkDist()){
				if (checkImpl()){
					if (checkBool()){
						cout << "Булева алгебра";
					}
				}
			}
		}
	}
	return 0;
}	
