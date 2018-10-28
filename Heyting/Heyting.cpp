#include <bits/stdc++.h>
using namespace std;

struct Node{
    vector<Node*> kids;
    Node* parent = nullptr;
    int number = 0;
    string value = "";
    set<string> forced;

    Node(Node* parent_, string message, int num, set<string> forced_){
        this->parent = parent_;
        this->value = message;
        this->number = num;
        this->forced = forced_;
    }

    Node(string message, set<string> forced_){
        this->value = message;
        this->forced = forced_;
    }

    Node(string message){
        this->value = message;
    }



    Node(){
    }

};

Node* FormulaTree;
string alph = "QWERTYUIOPASDFGHJKLZXCVBNM7894561230";
set<string> usedLetters;
vector<string> LettersVector;
vector<Node*> kripke;
vector<Node*> realKripke;
vector<Node*> subtreesRoots;
vector<set<int>> subtreesVertexes, massiveSubtreesVertexes;
set<int> tempVertexes;
set<set<int>> topology;
vector<set<int>> whereVariablesAreForced;
vector<vector<set<int>>> ForestOfSubtreesVertexes;
int totalKidsAmount = 1;
vector<int> generationNumbers(9);


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


void exprToTree(string s, Node* tree){
    //cout << s << " <= expression" << endl;
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
            tree->kids.push_back(new Node("left"));
            tree->kids.push_back(new Node("right"));
            //tree->left = new Node();
            //tree->right = new Node();
            exprToTree(s.substr(firstComInd + 1, secondComInd - firstComInd - 1), tree->kids[tree->kids.size() - 2]);
            exprToTree(s.substr(secondComInd + 1, s.length() - 2 - secondComInd), tree->kids[tree->kids.size() - 1]);
            break;
        }
        else{
            if (s[i] == '!'){
                tree->value = s[i];
                tree->kids.push_back(new Node("left"));
                tree->kids.push_back(nullptr);
                //tree->right = nullptr;
                //tree->left = new Node();
                exprToTree(s.substr(i + 1, s.length() - 2 - i), tree->kids[tree->kids.size() - 2]);
                break;
            }
            string tmp = "";
            while (i < s.size() && !isBinaryOperation(s[i]) && s[i] != '(' && s[i] != ')') {
                tmp += s[i];
                i++;
            }
            tree->value = tmp;
            tree->kids.push_back(nullptr);
            tree->kids.push_back(nullptr);
            //tree->left = nullptr;
            //tree->right = nullptr;
            }
        i++;
    }
}

void printTree(Node* tree){
    cout << tree->value << " " << tree->number << endl;
    for (auto i: tree->forced){
        cout << i << " ";
    }
    cout << " <- set of forced variables " << endl;
    /*if (tree->left != nullptr){
        cout << "=========Left child is below=======" << endl;
        printTree(tree->left);
    }
    if (tree->right != nullptr){
        cout << "=========Right child is below=======" << endl;
        printTree(tree->right);
    }*/
    for (auto i: tree->kids){
        if (i != nullptr){
            cout << tree->kids.size() << " =============Child is below=============" << endl;
            printTree(i);
        } else {
            //cout << " nullptr " << endl;
        }
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

void findUsedLetters(Node* tree){
    for (auto i: tree->kids){
        if (i != nullptr){
            findUsedLetters(i);
        }
    }
    if (isLit(tree->value)) {
        usedLetters.insert(tree->value);
    }
}

void fromSetToVector(){
    for (auto i: usedLetters){
        LettersVector.push_back(i);
    }
}

void forceKid(Node* tree){
    tree->forced.insert(tree->parent->forced.begin(), tree->parent->forced.end());
    /*if (tree->left != nullptr){
        forceKid(tree->left);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
    }*/
    for (auto i: tree->kids){
        forceKid(i);
    }
}


void forcingVariables(Node* tree, vector<int> curNumbers){
    if (curNumbers[tree->number] != 0){
        tree->forced.insert(LettersVector[curNumbers[tree->number] - 1]);
        /*if (tree->left != nullptr){
            forceKid(tree->left);
        }
        if (tree->right != nullptr){
            forceKid(tree->right);
        }*/
        for (auto i: tree->kids){
            forceKid(i);
            forcingVariables(i, curNumbers);
        }
    }
    /*if (tree->left != nullptr){
        forcingVariables(tree->left, curNumbers);
    }
    if (tree->right != nullptr){
        forcingVariables(tree->right, curNumbers);
    }*/
    //printTree(tree);
}

void forceOR(Node* tree, string left, string right){
    /*for (auto i: tree->forced){
        if (i == left || i == right){
            tree->forced.insert(expression(left + '|' + right));
            break;
        }
    }*/
    /*for (auto i: tree->forced){
        cout << i << " ";
    }
    cout << " <= SUP IS REDI" << endl;*/
    if (tree->forced.count(expression(left)) > 0 || tree->forced.count(expression(right)) > 0){
        tree->forced.insert(expression("(" + left + ")|(" + right + ")"));
    }
    /*if (tree->left != nullptr){
        forceKid(tree->left);
        forceOR(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceOR(tree->right, left, right);
    }*/
    for (auto i: tree->kids){
        forceKid(i);
        forceOR(i, left, right);
    }
}

void forceAND(Node* tree, string left, string right){
    /*for (auto i: tree->forced){
        cout << i << " ";
    }
    cout << " HI IGOR SUKA ITS ME VADIM" << endl;*/
    if (tree->forced.count(expression(left)) > 0 && tree->forced.count(expression(right)) > 0){
        tree->forced.insert(expression("(" + left + ")&(" + right + ")"));
    }
    /*if (tree->left != nullptr){
        forceKid(tree->left);
        forceAND(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceAND(tree->right, left, right);
    }*/
    for (auto i: tree->kids){
        forceKid(i);
        forceAND(i, left, right);
    }
}

bool childForcedNot(Node* tree, string left){
    if (tree->forced.count(expression(left)) > 0){
        return false;
    }
    else {
        bool flagKids = true;
        for (auto i: tree->kids){
            flagKids &= childForcedNot(i, left);
        }
        return flagKids;
        /*if (tree->left != nullptr){
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
        }*/
    }

}

bool childForcedImpl(Node* tree, string left, string right){
    bool flagKids = true;
    for (auto i: tree->kids){
        flagKids &= childForcedImpl(i, left, right);
    }
    if (tree->forced.count(expression(left)) && !tree->forced.count(expression(right))){
        return false;
    }
    else{
        return flagKids;
    }
    /*if (tree->forced.count(expression(left))){
        if (tree->forced.count(expression(right))){
            bool flagKids = true;
            for (auto i: tree->kids){
                flagKids &= childForcedImpl(i, left, right);
            }
            return flagKids;
            /*if (tree->left != nullptr){
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
        bool flagKids = true;
        for (auto i: tree->kids){
            flagKids &= childForcedImpl(i, left, right);
        }
        return flagKids;
        /*if (tree->left != nullptr){
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
    }*/
}

void forceImpl(Node* tree, string left, string right){
    if (childForcedImpl(tree, left, right)){
        tree->forced.insert(expression("(" + left + ")->(" + right + ")"));
    }
    /*if (tree->left != nullptr){
        forceKid(tree->left);
        forceImpl(tree->left, left, right);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceImpl(tree->right, left, right);
    }*/
    for (auto i: tree->kids){
        forceKid(i);
        forceImpl(i, left, right);
    }
}

void forceNot(Node* tree, string left){
    if (childForcedNot(tree, left)){
        tree->forced.insert(expression("!(" + left + ")"));
    }
    /*if (tree->left != nullptr){
        forceKid(tree->left);
        forceNot(tree->left, left);
    }
    if (tree->right != nullptr){
        forceKid(tree->right);
        forceNot(tree->right, left);
    }*/
    for (auto i: tree->kids){
        forceKid(i);
        forceNot(i, left);
    }

}
void checkOperations(Node* formTree, Node* worldTree);

string leftNrightCheck(Node* tree, Node* worldTree){
    if (isLit(tree->value)){
        return (tree->value);
    }
    if (tree->value == "!"){
        return ("!(" + leftNrightCheck(tree->kids[tree->kids.size() - 2], worldTree) + ")");
    }
    else {
        checkOperations(tree, worldTree);
        string tmp = tree->value;
        if (tree->value == "-"){
            tmp += ">";
        }
        return ("(" + leftNrightCheck(tree->kids[tree->kids.size() - 2], worldTree) + ")" + tmp + "(" + leftNrightCheck(tree->kids[tree->kids.size() - 1], worldTree) + ")");
    }

}

void checkOperations(Node* formTree, Node* worldTree){
    //cout << "tree->value at the beginning => " << formTree->value << endl;
    if (isBinaryOperation(formTree->value[0]) || formTree->value == "!"){
        string left = (leftNrightCheck(formTree->kids[formTree->kids.size() - 2], worldTree));
        string right = "";
        if (formTree->kids[formTree->kids.size() - 1] != nullptr){
            right = (leftNrightCheck(formTree->kids[formTree->kids.size() - 1], worldTree));
        }
        if (formTree->value == "!"){
            left = "!(" + left + ")";
        }
        //cout << left << " " << right << " <- left 'n right's are here and tree->value is there => " << formTree->value << endl;
        bool fLeft = false;
        bool fRight = false;
        int countLeft = 0;
        int countRight = 0;
        
        while (left[0] == '!'){
            left = left.substr(2, left.size() - 3);
            fLeft = true;
            countLeft++;
            //cout << left << " <= left before going forceNot" << endl;
            //forceNot(worldTree, left);
        }
        while (right[0] == '!'){
            right = right.substr(2, right.size() - 3);
            fRight = true;
            countRight++;
            //cout << right << " <= right before going forceNot" << endl;
            //forceNot(worldTree, right);
        }
        while (countLeft > 0){
            //cout << left << " <= left before going forceNot" << endl;
            forceNot(worldTree, left);
            left = "!(" + left + ")";
            countLeft--;
        }
        while (countRight > 0){
            //cout << right << " <= right before going forceNot" << endl;
            forceNot(worldTree, right);
            right = "!(" + right + ")";
            countRight--;
        }


        if (formTree->value[0] == '|'){
            //cout << left << " " << right << " ISSUP REDI?" << endl;
            forceOR(worldTree, left, right);
        }
        else {
            if (formTree->value[0] == '&'){
                //cout << "HI SUKA ITS ME IGOR " << left << " " << right << endl;
                forceAND(worldTree, left, right);
            }
            else{
                if (formTree->value[0] == '-'){
                    //cout << "IS SUP READY " << left << " " << right << endl;
                    forceImpl(worldTree, left, right);
                    /*cout << "----------------------------" << endl;
                    printTree(worldTree);
                    cout << "----------------------------" << endl;*/
                }
            }
        }

    }
}

int vecSum(vector<int> v){
    int ans = 0;
    for (auto i: v){
        ans += i;
    }
    return ans;
}


string noice(string s){
    string ans = "";
    for (size_t i = 0; i < s.length(); i++){
        if (s[i] != '\t' && s[i] != ' ' && s[i] != '\r'){
            ans += s[i];
        }
    }
    return ans;
}


void makeKids(Node* tree, int depth, int childLevel, set<string> variables){
    size_t lastKid = tree->kids.size() - 1;
    if (depth != 1){
        makeKids(tree->kids[lastKid], depth - 1, childLevel, variables);
    } 
    else {
        Node* newKid = new Node();
        newKid->parent = tree;
        newKid->forced = variables;
        newKid->value = "child";
        newKid->number = childLevel;
        tree->kids.push_back(newKid);
    }

}

void forceKids(Node* tree){
    tree->forced.insert(tree->parent->forced.begin(), tree->parent->forced.end());
    for (auto i: tree->kids){
        forceKids(i);
    }
}

/*bool isCorrectModel(Node* wannabe, Node* original, bool answer){
    answer &= (wannabe->forced == original->forced);
    answer &= (wannabe->kids.size() == original->kids.size());
    if (!answer){
        return false;
    }
    for (int i = 0; i < wannabe->kids.size(); i++){
        answer &= isCorrectModel(wannabe->kids[i], original->kids[i], answer);
    }
    return answer;
}

bool oneSetInAnother(set<string> first, set<string> second){
    for (auto i: first){
        if (!(second.count(i) > 0)){
            return false;
        }
    }
    return true;
}

bool isCorrectModel(Node* wannabe, Node* original){
    bool ans = true;
    if (oneSetInAnother(wannabe->forced, original->forced) && oneSetInAnother(original->forced, wannabe->forced)){
        for (int i = 0; i < wannabe->kids.size(); i++){
            ans &= isCorrectModel(wannabe->kids[i], original->kids[i]);
        }
        return ans;
    }
    else {
        return false;
    }

}*/

bool comparator(Node* tree){
    if (tree->parent == nullptr){
        return true;
    }
    for (auto i: tree->parent->forced){
        //cout << i << " <= current variable " << endl;
        if (!(tree->forced.count(expression(i)) > 0)){
            return false;
        }
    }
    return true;
}

bool flag = false;
bool isCorrectModel(Node* tree){
    for (auto i: tree->kids){
        isCorrectModel(i);
    }
    if (flag){
        return false;
    }
    if (!comparator(tree)){
        flag = true;
        return false;
    }
    return true;
}

void doBase(Node* tree){
    subtreesRoots.push_back(tree);
    /*if (tree->left != nullptr){
        //subtreesRoots.push_back(tree->left);
        doBase(tree->left);
    }
    if (tree->right != nullptr){
        //subtreesRoots.push_back(tree->right);
        doBase(tree->right);
    }*/
    for (auto i:tree->kids){
        doBase(i);
    }
}

void getSubtreesVertexesNumbers(Node* tree){
    tempVertexes.insert(tree->number);
    /*if (tree->left != nullptr){
        getSubtreesVertexesNumbers(tree->left);
    }
    if (tree->right != nullptr){
        getSubtreesVertexesNumbers(tree->right);
    }*/
    for (auto i: tree->kids){
        getSubtreesVertexesNumbers(i);
    }
}

void makeSubtreesVertexes(){
    subtreesVertexes.resize(subtreesRoots.size());
    for (int i = 0; i < subtreesRoots.size(); i++){
        getSubtreesVertexesNumbers(subtreesRoots[i]);
        /*for (int j = 0; j < tempVertexes.size(); j++){
            subtreesVertexes[i].push_back(tempVertexes[j]);
        }*/
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
    /*if (tree->left != nullptr){
        vertexesWhereVariableIsForced(tree->left, variable, index);
    }
    if (tree->right != nullptr){
        vertexesWhereVariableIsForced(tree->right, variable, index);
    }*/
    for (auto i: tree->kids){
        vertexesWhereVariableIsForced(i, variable, index);
    }
}

void enumerateVertexes(Node* tree){
    tree->number = totalKidsAmount++;
    for (auto i: tree->kids){
        enumerateVertexes(i);
    }
}


int main(){
    freopen("input.txt", "r", stdin);
    FormulaTree = new Node();
    string inp, first = "";
    getline(cin, inp);
    first = noice(inp);
    string exprView = expression(first);
    //cout << first << " " << exprView << endl;
    exprToTree(exprView, FormulaTree);
    //printTree(FormulaTree);
    string rline;
    findUsedLetters(FormulaTree);
    fromSetToVector();
    vector<pair<int, set<string>>> read;
    while (getline(cin, rline)){
        if (rline == "stop"){
            break;
        }
        int spaces = 0;
        while (rline[spaces] == '\t'){
            spaces++;
        }
        int starPos = rline.find('*');
        set<string> vars;
        int j = spaces;
        string endOfTheRow = rline.substr(spaces, rline.size() - spaces);
        
        if (endOfTheRow.size() > 1){
            endOfTheRow = endOfTheRow.substr(2, endOfTheRow.size() - 2);
        }
        else {
            endOfTheRow = "";
        }
        //cout << endOfTheRow << " <= endoftherow" << endl;
        j = 0;
        while (j < endOfTheRow.size()){
            string tmp = "";
            while (endOfTheRow[j] != ',' && endOfTheRow[j] != ' ' && endOfTheRow[j] != '\t' && endOfTheRow[j] != '\r' && endOfTheRow[j] != '\n' && endOfTheRow[j] != '\0'){
                tmp += endOfTheRow[j];
                j++;
            }
            //cout << tmp << " HERE " << endl;
            vars.insert(expression(tmp));
            j++;
        }
        read.push_back(make_pair(starPos, vars));
    }
    /*for (int i = 0; i < read.size(); i++){
        cout << read[i].first << " ";
        for (auto j: read[i].second){
            cout << j << " ";
        }
        cout << endl;
    }*/
    Node* current;
    for (int i = 0; i < read.size(); i++){    
        if (read[i].first == 0){
            current = new Node("Root", read[i].second);
            kripke.push_back(current);
            current = new Node("Root", read[i].second);
            realKripke.push_back(current);
        }
        else {
            makeKids(kripke[kripke.size() - 1], read[i].first, read[i].first, read[i].second);
            makeKids(realKripke[realKripke.size() - 1], read[i].first, read[i].first, read[i].second);
        }
    }

    /*for (int i = 0; i < kripke.size(); i++){
        cout << "=====================NEW INPUTED TREE=====================" << endl;
        printTree(kripke[i]);
    }*/

    for (int i = 0; i < realKripke.size(); i++){
        for (auto j: realKripke[i]->kids){
            forceKids(j);
        }
    }
    //cout << "KRIPKE TREES AFTER REALKRIPKE MAKING" << endl;
    for (int i = 0; i < kripke.size(); i++){
        enumerateVertexes(kripke[i]);
    }
    //printTree(FormulaTree);

    
    
    /*cout << "REALKRIPKE TREES" << endl;

    for (int i = 0; i < realKripke.size(); i++){
        cout << "=====================NEW INPUTED TREE=====================" << endl;
        //checkOperations(FormulaTree, realKripke[i]);
        printTree(realKripke[i]);
    }*/
    

    /*for (int i = 0; i < kripke.size(); i++){
        cout << "=====================NEW INPUTED TREE=====================" << endl;
        //checkOperations(FormulaTree, kripke[i]);
        printTree(kripke[i]);
    } */   




    for (int i = 0; i < realKripke.size(); i++){
        //if (!isCorrectModel(kripke[i], realKripke[i])){
        flag = false;
        if (!isCorrectModel(kripke[i])){
            cout << "Не модель Крипке" << endl;
            return 0;
        }
    }

    for (int i = 0; i < kripke.size(); i++){
        checkOperations(FormulaTree, kripke[i]);
        //checkOperations(FormulaTree, realKripke[i]);
    }

    for (int i = 0; i < realKripke.size(); i++){
        if (kripke[i]->forced.count(exprView) > 0){
            cout << "Не опровергает формулу" << endl;
            return 0;
        }
    }

    for (int i = 0; i < kripke.size(); i++){
        doBase(kripke[i]);
        makeSubtreesVertexes();
        ForestOfSubtreesVertexes.push_back(subtreesVertexes);
        subtreesVertexes.clear();
        subtreesRoots.clear();
        tempVertexes.clear();
    }
    for (int i = 0; i < ForestOfSubtreesVertexes.size(); i++){
        for (auto j: ForestOfSubtreesVertexes[i]){
            massiveSubtreesVertexes.push_back(j);
        }
    }
    //cout << massiveSubtreesVertexes.size() << endl;
    int n = massiveSubtreesVertexes.size();
    /*for (int i = 0; i < n; i++){
        cout << i + 1 << ": ";
        for (auto j: massiveSubtreesVertexes[i]){
            cout << j << " ";
        }
        cout << endl;
    }*/
    vector<int> binVector;
    binVector.resize(n);
    for (int i = 0; i < n; i++){
        binVector[i] = 0;
    }
    //cout << " hi " << endl;
    while (true){
        set<int> tmp;
        for (int j = 0; j < n; j++){
            if (binVector[j] == 1){
                tmp.insert(massiveSubtreesVertexes[j].begin(), massiveSubtreesVertexes[j].end());
            }
        }
        topology.insert(tmp);
        binVector = boolVectorGeneration(binVector);
        if (vecSum(binVector) == 0){
            break;
        }
    }
    //cout << "hello" << endl;
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
    /*for (int j = 0; j < LettersVector.size(); j++){
        vertexesWhereVariableIsForced(kripke[i], LettersVector[j], j);
    }*/
    for (int j = 0; j < LettersVector.size(); j++){
        for (int i = 0; i < kripke.size(); i++){
            vertexesWhereVariableIsForced(kripke[i], LettersVector[j], j);
        }
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
    cout << endl;
    return 0;

}

