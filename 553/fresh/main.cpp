//overlapping circles
#include <vector>
#include <iostream>

using namespace std;
int solution(vector<int> &A){
    int sum = 0, active = 0;
    vector<int> start(A.size());
    vector<int> end(A.size());
    for(int i=0; i < A.size(); i++){
        if(A[i] > i){
            start[0]++;
        } else{
            start[i - A[i]]++;
        }
        if(A[i] + i > A.size()){
            end[A.size() -1]++;
        } else {
            end[A[i] + i]++;
        }
    }

    for(int i=0; i < A.size(); i++){
        sum += active * start[i] + (start[i] * (start[i]-1))/2;
        if(sum > 10000000){
            return -1;
        }
        active += start[i] - end[i];
    }
    return sum;
}

int main(){
    vector<int> source = {1,5,2,1,4,0};
    cout << solution(source);
    return 0;
}