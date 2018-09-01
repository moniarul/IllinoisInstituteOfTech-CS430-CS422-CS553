#include <set>
#include <vector>
#include <iostream>
#include <algorithm>

using namespace std;
int solution(vector<int> &A) {
    set<int> all;
    int duplicateCount = 0;
    int inputSize = A.size();
    pair<set<int>::iterator , bool> pair1;
    for(unsigned int i=0; i< inputSize  ; i++){
        pair1 = all.insert(A[i]);
        if(!pair1.second){
            duplicateCount++;
        }
    }
    return inputSize - duplicateCount;
}

int main(){
    vector<int> test = {1,3,7,3,32};
    cout << solution(test);
    return 0;
}