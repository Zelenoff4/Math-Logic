#include <bits/stdc++.h>
using namespace std;

struct Node{
    Node* left = nullptr;
    Node* right = nullptr;
    Node* parent = nullptr;
    int number = 0;
    string value = "";
    set<string> forced;

    Node(Node* parent_, string message, int num){
        this->parent = parent_;
        this->value = message;
        this->number = num;
    }
    Node(string message){
        this->value = message;
    }

    Node(){
    }

};

Node* Trees[14];
Node* FormulaTree;
string cbs[14] = {"(((())))", "((()()))", "((())())", "((()))()", "(()(()))", "(()()())", "(()())()", "(())(())",
                  "(())()()", "()((()))", "()(()())", "()(())()", "()()(())", "()()()()"};

string alph = "QWERTYUIOPASDFGHJKLZXCVBNM7894561230";
set<string> usedLetters;
vector<string> LettersVector;
vector<int> generationNumbers(9);
vector<Node*> subtreesRoots;
vector<set<int>> subtreesVertexes;
set<int> tempVertexes;
set<set<int>> topology;
vector<set<int>> whereVariablesAreForced;

bool isBinaryOperation(char c){
    if (c == '-' or c == '|' or c == '&'){
        return true;
    }
    return false;
}

bool isLit(string s){
    for (size_t i = 0; i < s.length(); i++){
        if (alph.find(s[i]) == -1){
            return false;
        }
    }
    return true;
}

void exprToTree(string s, Node* tree){
    size_t i = 0;
    while (i < s.size()){
        if (isBinaryOperation(s[i])) {
            int balance = 0;
            size_t firstComInd = 0, secondComInd = 0;
            tree->value = s[i];
            size_t j = i + 1;
            if (s[j] == '>'){
                j += 1;
            }
            firstComInd = j;
            for (size_t k = j + 1; k < s.size(); k++){
                if (s[k] == '('){
                    balance += 1;
                }
                if (s[k] == ')'){
                    balance -= 1;
                }
                if (s[k] == ',' && balance == 0){
                    secondComInd = k;
                    break;
                }
            }
            tree->left = new Node();
            tree->right = new Node();
            exprToTree(s.substr(firstComInd + 1, secondComInd - firstComInd - 1), tree->left);
            exprToTree(s.substr(secondComInd + 1, s.length() - 2 - secondComInd), tree->right);
            break;
        }
        else{
            if (s[i] == '!'){
                tree->value = s[i];
                tree->right = nullptr;
                tree->left = new Node();
                exprToTree(s.substr(i + 1, s.length() - 2 - i), tree->left);
                break;
            }
            string tmp = "";
            while (i < s.size() && !isBinaryOperation(s[i]) && s[i] != '(' && s[i] != ')') {
                tmp += s[i];
                i++;
            }
            tree->value = tmp;
            tree->left = nullptr;
            tree->right = nullptr;
            }
        i++;
    }
}

void mktree(string cbs, Node* tree, int number){
    for (int i = 0; i < cbs.size(); i++){
        if (cbs[i] == '('){
            tree->left = new Node(tree, "left son", ++number);
            tree = tree->left;
        }
        else {
            tree = tree->parent;
            if (tree->right != nullptr){
                tree = tree->parent;
            }
            tree->right = new Node(tree, "right son", ++number);
            tree = tree->right;
        }
    }
}

void printTree(Node* tree){
    cout << tree->value << " " << tree->number << endl;
    for (auto i: tree->forced){
        cout << i << " ";
    }
    cout << " <- set of forced variables " << endl;
    if (tree->left != nullptr){
        cout << "=========Left child is below=======" << endl;
        printTree(tree->left);
    }
    if (tree->right != nullptr){
        cout << "=========Right child is below=======" << endl;
        printTree(tree->right);
    }
    return;
}

string expression(string s);
string negation(string s){
    int balance = 0;
    for (size_t i = 0; i < s.length(); i++){
        if (s[i] == '('){
            balance += 1;
        }
        if (s[i] == ')'){
            balance -= 1;
        }
        if (s[i] == '!' && balance == 0){
            return "(!" + negation(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    if (s.length() > 0 && s[0] == '('){
        return expression(s.substr(1, s.length() - 2));
    }
    else{
        return s;
    }
}

string conjunction(string s){
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--){
        if (s[i] == '('){
            balance += 1;
        }
        if (s[i] == ')'){
            balance -= 1;
        }
        if (s[i] == '&' && balance == 0){
            return "(&," + conjunction(s.substr(0, i)) + "," + negation(s.substr(i + 1, s.length() - i)) + ")";
        }
    }
    return negation(s);
}

string disjunction(string s){
    int balance = 0;
    for (int i = s.length() - 1; i >= 0; i--){
        if (s[i] == '('){
            balance += 1;
        }
        if (s[i] == ')'){
            balance -= 1;
        }
        if (s[i] == '|' && balance == 0){
            return "(|," + disjunction(s.substr(0, i)) + "," + conjunction(s.substr(i + 1, s.length() - i - 1)) + ")";
        }
    }
    return conjunction(s);
}

string expression(string s){
    int balance = 0;
    for (size_t i = 0; i < s.length(); i++){
        if (s[i] == '('){
            balance += 1;
        }
        if (s[i] == ')'){
            balance -= 1;
        }
        if (s[i] == '-' && balance == 0){
            return "(->," + disjunction(s.substr(0, i)) + "," + expression(s.substr(i + 2, s.length() - i - 2)) + ")";
        }
    }
    return disjunction(s);
}

void init(){
    for (int i = 0; i < 14; i++){
        Trees[i] = new Node("Root");
        mktree(cbs[i], Trees[i], 0);
    }
    FormulaTree = new Node();
}

void findUsedLetters(Node* tree){
    if (tree->left != nullptr){
        findUsedLetters(tree->left);
    }
    if (tree->right != nullptr){
        findUsedLetters(tree->right);
    }
    if (isLit(tree->value)) {
        usedLetters.insert(tree->value);
    }
}

vector<int> genNextVector(vector<int> curVector){
    for (int i = curVector.size() - 1; i >= 0; i--){
        if (curVector[i] < usedLetters.size()){
            curVector[i]++;
            break;
        }
        else{
            curVector[i] = 0;
        }
    }
    return curVector;
}

int vecSum(vector<int> v){
    int ans = 0;
    for (auto i: v){
        ans += i;
    }
    return ans;
}

void forceKid(Node* tree){
    tree->forced.insert(tree->parent->forced.begin(), tree->parent->forced.end());
    if (tree->left != nullptr){
        forceKid(tree->left);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
    }
}


void forcingVariables(Node* tree, vector<int> curNumbers){
    if (curNumbers[tree->number] != 0){
        tree->forced.insert(LettersVector[curNumbers[tree->number] - 1]);
        if (tree->left != nullptr){
            forceKid(tree->left);
        }
        if (tree->right != nullptr){
            forceKid(tree->right);
        }
    }
    if (tree->left != nullptr){
        forcingVariables(tree->left, curNumbers);
    }
    if (tree->right != nullptr){
        forcingVariables(tree->right, curNumbers);
    }

}

void forceOR(Node* tree, string left, string right){

    if (tree->forced.count(expression(left)) > 0 || tree->forced.count(expression(right)) > 0){
        tree->forced.insert(expression("(" + left + ")|(" + right + ")"));
    }
    if (tree->left != nullptr){
        forceKid(tree->left);
        forceOR(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceOR(tree->right, left, right);
    }
}

void forceAND(Node* tree, string left, string right){
    if (tree->forced.count(expression(left)) > 0 && tree->forced.count(expression(right)) > 0){
        tree->forced.insert(expression("(" + left + ")&(" + right + ")"));
    }
    if (tree->left != nullptr){
        forceKid(tree->left);
        forceAND(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceAND(tree->right, left, right);
    }
}

bool childForcedNot(Node* tree, string left){
    if (tree->forced.count(expression(left)) > 0){
        return false;
    }
    else {
        if (tree->left != nullptr){
            if (tree->right != nullptr){
                return (childForcedNot(tree->left, left) && childForcedNot(tree->right, left));
            }
            else{
                return childForcedNot(tree->left, left);
            }
        }
        else{
            if (tree->right != nullptr){
                return childForcedNot(tree->right, left);
            }
            else {
                return !(tree->forced.count(left) > 0);
            }
        }
    }

}

bool childForcedImpl(Node* tree, string left, string right){
    if (tree->forced.count(expression(left))){
        if (tree->forced.count(expression(right))){
            if (tree->left != nullptr){
                if (tree->right != nullptr){
                    return (childForcedImpl(tree->left, left, right) && childForcedImpl(tree->right, left, right));
                }
                else{
                    return childForcedImpl(tree->left, left, right);
                }
            }
            else {
                if (tree->right != nullptr){
                    return childForcedImpl(tree->right, left, right);
                }
                else{
                    return true;
                }
            }
        }
        else {
            return false;
        }
    }
    else{
        if (tree->left != nullptr){
            if (tree->right != nullptr){
                return (childForcedImpl(tree->left, left, right) && childForcedImpl(tree->right, left, right));
            }
            else{
                return childForcedImpl(tree->left, left, right);
            }
        }
        else {
            if (tree->right != nullptr){
                return childForcedImpl(tree->right, left, right);
            }
            else{
                return true;
            }
        }
    }
}

void forceImpl(Node* tree, string left, string right){
    if (childForcedImpl(tree, left, right)){
        tree->forced.insert(expression("(" + left + ")->(" + right + ")"));
    }
    if (tree->left != nullptr){
        forceKid(tree->left);
        forceImpl(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceImpl(tree->right, left, right);
    }
}

void forceNot(Node* tree, string left){
    if (childForcedNot(tree, left)){
        tree->forced.insert(expression("!(" + left + ")"));
    }
    if (tree->left != nullptr){
        forceKid(tree->left);
        forceNot(tree->left, left);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceNot(tree->right, left);
    }

}
void checkOperations(Node* formTree, Node* worldTree);

string leftNrightCheck(Node* tree, Node* worldTree){
    if (isLit(tree->value)){
        return (tree->value);
    }
    if (tree->value == "!"){
        return ("!(" + leftNrightCheck(tree->left, worldTree) + ")");
    }
    else {
        checkOperations(tree, worldTree);
        string tmp = tree->value;
        if (tree->value == "-"){
        	tmp += ">";
        }
        return ("(" + leftNrightCheck(tree->left, worldTree) + ")" + tmp + "(" + leftNrightCheck(tree->right, worldTree) + ")");
    }

}

void checkOperations(Node* formTree, Node* worldTree){
    if (isBinaryOperation(formTree->value[0]) || formTree->value == "!"){
        string left = (leftNrightCheck(formTree->left, worldTree));
        string right = "";
        if (formTree->right != nullptr){
        	right = (leftNrightCheck(formTree->right, worldTree));
        }
        if (formTree->value == "!"){
        	left = "!(" + left + ")";
        }
        bool fLeft = false;
        bool fRight = false;
        int countLeft = 0;
        int countRight = 0;

        while (left[0] == '!'){
            left = left.substr(2, left.size() - 3);
            fLeft = true;
            countLeft++;
        }
        while (right[0] == '!'){
            right = right.substr(2, right.size() - 3);
            fRight = true;
            countRight++;
        }
        while (countLeft > 0){
            forceNot(worldTree, left);
            left = "!(" + left + ")";
            countLeft--;
        }
        while (countRight > 0){
            forceNot(worldTree, right);
            right = "!(" + right + ")";
            countRight--;
        }


        if (formTree->value[0] == '|'){
            forceOR(worldTree, left, right);
        }
        else {
            if (formTree->value[0] == '&'){
                forceAND(worldTree, left, right);
            }
            else{
                if (formTree->value[0] == '-'){
                    forceImpl(worldTree, left, right);
                }
            }
        }

    }
}

void fromSetToVector(){
    for (auto i: usedLetters){
        LettersVector.push_back(i);
    }
}

void clearForced(Node* tree){
    tree->forced.clear();
    if (tree->left != nullptr){
        clearForced(tree->left);
    }
    if (tree->right != nullptr){
        clearForced(tree->right);
    }
}

void doBase(Node* tree){
    subtreesRoots.push_back(tree);
    if (tree->left != nullptr){
        doBase(tree->left);
    }
    if (tree->right != nullptr){
        doBase(tree->right);
    }
}

void getSubtreesVertexesNumbers(Node* tree){
    tempVertexes.insert(tree->number);
    if (tree->left != nullptr){
        getSubtreesVertexesNumbers(tree->left);
    }
    if (tree->right != nullptr){
        getSubtreesVertexesNumbers(tree->right);
    }
}

void makeSubtreesVertexes(){
    subtreesVertexes.resize(subtreesRoots.size());
    for (int i = 0; i < subtreesRoots.size(); i++){
        getSubtreesVertexesNumbers(subtreesRoots[i]);

        subtreesVertexes[i].insert(tempVertexes.begin(), tempVertexes.end());
        tempVertexes.clear();
    }
}

bool isCorrect(Node* tree, string formula){
    return (tree->forced.count(formula) > 0);
}

vector<int> boolVectorGeneration(vector<int> curVector){
    for (int i = curVector.size() - 1; i >= 0; i--){
        if (curVector[i] == 1){
            curVector[i] = 0;
        }
        else{
            curVector[i]++;
            break;
        }
    }
    return curVector;
}

bool oneSetInAnother(set<int> first, set<int> second){
    for (auto i: first){
        if (!(second.count(i) > 0)){
            return false;
        }
    }
    return true;
}

void vertexesWhereVariableIsForced(Node* tree, string variable, int index){
    if (tree->forced.count(expression(variable)) > 0){
        whereVariablesAreForced[index].insert(tree->number);
    }
    if (tree->left != nullptr){
        vertexesWhereVariableIsForced(tree->left, variable, index);
    }
    if (tree->right != nullptr){
        vertexesWhereVariableIsForced(tree->right, variable, index);
    }
}

int main(){
    init();
    string inp, first = "";
    getline(cin, inp);
    for (size_t i = 0; i < inp.length(); i++){
        if (inp[i] != '\t' && inp[i] != ' ' && first[i] != '\r')
        first += inp[i];
    }
    string exprView = expression(first);
    exprToTree(exprView, FormulaTree);
    findUsedLetters(FormulaTree);
    fromSetToVector();


    for (int i = 0; i < 14; i++){
        while (true){
            forcingVariables(Trees[i], generationNumbers);
            checkOperations(FormulaTree, Trees[i]);
            if (isCorrect(Trees[i], exprView)){
                clearForced(Trees[i]);
                generationNumbers = genNextVector(generationNumbers);
                if (vecSum(generationNumbers) == 0){
                    break;
                }
            }
            else {

                doBase(Trees[i]);
                makeSubtreesVertexes();

                int n = subtreesVertexes.size();
                vector<int> binVector;
                binVector.resize(n);
                while (true){
                    set<int> tmp;
                    for (int j = 0; j < n; j++){
                        if (binVector[j] == 1){
                            tmp.insert(subtreesVertexes[j].begin(), subtreesVertexes[j].end());
                        }
                    }
                    topology.insert(tmp);
                    binVector = boolVectorGeneration(binVector);
                    if (vecSum(binVector) == 0){
                        break;
                    }
                }
                set<int> emptySet;
                topology.insert(emptySet);
                vector<set<int>> vectorTopology;
                for (auto i: topology){
                    vectorTopology.push_back(i);
                }
                vector<vector<int>> g;
                g.resize(vectorTopology.size());
                for (int j = 0; j < vectorTopology.size(); j++){
                    for (int k = 0; k < vectorTopology.size(); k++){
                        if (oneSetInAnother(vectorTopology[j], vectorTopology[k])){
                            g[j].push_back(k + 1);
                        }
                    }
                }
                cout << g.size() << endl;
                for (int j = 0; j < g.size(); j++){
                    for (int k = 0; k < g[j].size(); k++){
                        cout << g[j][k] << " ";
                    }
                    cout << endl;
                }

                whereVariablesAreForced.resize(LettersVector.size());
                for (int j = 0; j < LettersVector.size(); j++){
                    vertexesWhereVariableIsForced(Trees[i], LettersVector[j], j);
                }

                for (int j = 0; j < LettersVector.size(); j++){
                    cout << LettersVector[j] << "=";
                    for (int k = 0; k < vectorTopology.size(); k++){
                        if (whereVariablesAreForced[j] == vectorTopology[k]){
                            cout << k + 1;
                            break;
                        }
                    }
                    if (j + 1 < LettersVector.size()){
                		cout << ",";
                	}
                }
                return 0;
            }
        }
    }

    cout << "Формула общезначима" << endl;
    return 0;
}
