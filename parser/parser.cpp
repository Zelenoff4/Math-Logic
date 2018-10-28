#include <bits/stdc++.h>

using namespace std;

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
//a->(a|b)
string disjunction(string s){
    //cout << s << " <= today im here with " << endl;
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
            //cout << s.substr(0, i) << " opa " << s.substr(i + 2, s.length() - i - 2) << endl;
            return "(->," + disjunction(s.substr(0, i)) + "," + expression(s.substr(i + 2, s.length() - i - 2)) + ")";
        }
    }
    //cout << "im goin in baby" << endl;
    return disjunction(s);
}

int main(){
    freopen("input.txt", "r", stdin);
    freopen("output.txt", "w", stdout);
    string s, ans = "", inp;
    while (getline(cin, inp)){
        s += inp;
    }
    for (auto i : s){
        if (i != '\t' && i != '\n' && i != ' '){
            ans += i;
        }
    }
    cout << expression(ans);
    return 0;
}
