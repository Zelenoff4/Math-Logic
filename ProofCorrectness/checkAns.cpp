#include <bits/stdc++.h>
using namespace std;

struct Node{
    Node* left = nullptr;
    Node* right = nullptr;
    string value = "";

};

vector<string> expressions, leftPrt, rightPrt;
string axioms[10] = {"A->(B->A)", "(A->B)->(A->(B->C))->(A->C)","A->B->(A&B)", "(A&B)->A", "(A&B)->B", "A->(A|B)", "B->(A|B)", "(A->C)->(B->C)->((A|B)->C)", "(A->B)->(A->!B)->!A", "!!A->A"};
string expression(string s);
map<string, int> hypothesis;
string alph = "QWERTYUIOPASDFGHJKLZXCVBNM7894561230";
Node* axTrees[10];

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

void makeAxTrees(){
    for (int i = 0; i < 10; i++){
        axTrees[i] = new Node();
        exprToTree(expression(axioms[i]), axTrees[i]);
    }
}

void printTree(Node* tree);

bool checkTres(Node* r1, Node* r2){
    if (r1 == nullptr && r2 == nullptr) {
        return true;
    }
    if ((r1 == nullptr && r2 != nullptr) || (r1 != nullptr && r2 == nullptr)) {
        return false;
    }
    if (r1->value == r2->value) {
        return checkTres(r1->left, r2->left) & checkTres(r1->right, r2->right);
    } else {
        return false;
    }
}

map<string, Node*> *saved;

bool checkAx(Node* strTree, Node* axTree){
    if (axTree == nullptr && strTree == nullptr){
        return true;
    }
    if ((axTree == nullptr && strTree != nullptr) || (axTree != nullptr && strTree == nullptr)){
        return false;
    }
    if (isLit(axTree->value)) {
        if (saved->count(axTree->value) == 0){
            Node* tmp = new Node();
            tmp->value = strTree->value;
            tmp->left = strTree->left;
            tmp->right = strTree->right;
            saved->insert(make_pair(axTree->value, tmp));
            return true;
        }
        else {
            Node* tmp = new Node();
            tmp->value = saved->at(axTree->value)->value;
            tmp->left = saved->at(axTree->value)->left;
            tmp->right = saved->at(axTree->value)->right;
            Node* tmp2 = new Node();
            tmp2->value = strTree->value;
            tmp2->left = strTree->left;
            tmp2->right = strTree->right;
            return checkTres(tmp2, tmp);
        }
    }
    else{
        if (axTree->value.size() > 0 && isBinaryOperation(axTree->value[0]) && strTree->value == axTree->value){
            return checkAx(strTree->left, axTree->left) & checkAx(strTree->right, axTree->right);
        }
        else{
            if (strTree->value == axTree->value){
                return checkAx(strTree->left, axTree->left);
            }
            else{
                return false;
            }
        }
    }
}

void printTree(Node* tree){
    cout << tree->value << endl;;
    if (tree->left != nullptr){
        printTree(tree->left);
    }
    if (tree->right != nullptr){
        printTree(tree->right);
    }
}

int main(){
    freopen("in.txt", "r", stdin);
    freopen("out.txt", "w", stdout);
    string first, wcurr;
    getline(cin, first);
    size_t i = 0;
    size_t indPr = 0;
    size_t numeration = 0;
    while (first[i] != '-'){
        string tmp = "";
        while (first[i] != ',' && first[i] != '|'){
            tmp += first[i];
            i++;
        }
        hypothesis.insert(make_pair(expression(tmp), ++indPr));
        i += 1;
    }
    makeAxTrees();
    while (getline(cin, wcurr)){
        string curr = "";
        for (size_t i = 0; i < wcurr.length(); i++){
            if (wcurr[i] != '\t' && wcurr[i] != ' ')
            curr += wcurr[i];
        }
        string exprView = expression(curr);
        expressions.push_back(exprView);
        size_t k = 0;
        bool flag = false, isAx = false, isMP = false;
        while (k < exprView.length()){
            if (exprView[k] == '-'){
                int balance = 0;
                size_t firstComInd = 0, secondComInd = 0;
                size_t j = k + 2;
                firstComInd = j;
                for (size_t w = j + 1; w < exprView.size(); w++){
                    if (exprView[w] == '('){
                        balance += 1;
                    }
                    if (exprView[w] == ')'){
                        balance -= 1;
                    }
                    if (exprView[w] == ',' && balance == 0){
                        secondComInd = w;
                        break;
                    }
                }
                leftPrt.push_back(exprView.substr(firstComInd + 1, secondComInd - firstComInd - 1));
                rightPrt.push_back(exprView.substr(secondComInd + 1, exprView.length() - 2 - secondComInd));
                flag = true;
                break;
            }
        k += 1;
        }
        if (!flag){
            leftPrt.push_back("");
            rightPrt.push_back("");
        }
        if (hypothesis.count(exprView) > 0){
            cout << "(" << ++numeration << ") " << curr << " (Preposition. " << hypothesis[exprView] << ")" << endl;
            continue;
        }
        Node* curTree = new Node();
        exprToTree(exprView, curTree);
        for (size_t i = 0; i < 10; i++){
            saved = new map<string, Node*>;
            if (checkAx(curTree, axTrees[i])) {
                cout << "(" << ++numeration << ") " << curr << " (Scheme numb " << i + 1 << ")" << endl;
                isAx = true;
                break;
            }
        }
        if (isAx) {
            continue;
        }
        for (int i = rightPrt.size() - 2; i >= 0; i--){
            if (exprView == rightPrt[i]){
                for (size_t j = 0; j < expressions.size() - 1; j++){
                    if (expressions[j] == leftPrt[i]){
                        cout << "(" << ++numeration << ") " << curr << " (M.P. " << i + 1 << ", " << j + 1 << ")" << endl;
                        isMP = true;
                        break;
                    }
                }
            }
            if (isMP){
                break;
            }
        }
        if (isMP){
            continue;
        }
        cout << "(" << ++numeration << ") " << curr << " (Not stated)" << endl;
    }
}
