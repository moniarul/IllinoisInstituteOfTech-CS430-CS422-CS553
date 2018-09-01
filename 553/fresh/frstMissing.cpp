#include <set>
#include <vector>
#include <iostream>
using namespace std;

#include <set>
#include <vector>
#include <iostream>
using namespace std;
int solution(vector<int> &A) {
    vector<int> missing;
    vector<int> range(A.size());
    for(int i=1; i<= A.size(); i++) range[i-1] = i;

    set_difference(range.begin(), range.end(), A.begin(), A.end(), std::inserter(missing, missing.begin()));
    return missing[0]; // Missing contains 2,4,5 where as it should had contain just 5
}

int main(){
    vector<int> A = {1, 3, 6, 4, 1, 2};
    cout << solution(A);
    return 0;
}
//int solution(vector<int> &A) {
//    int size = A.size()+1;
//    vector<int> range(size,1);
//    for (int j = 0; j < size -1; ++j)
//        if(A[j] < size && A[j] > 0) range[A[j]] = 0;
//
//    for (int j = 1; j < size; ++j) {
//        if(range[j]) return j;
//    }
//    return size;
//}


// Is permutation solution . Different Problem
//int solution(vector<int> &A) {
//
//    vector<int> range(A.size(),1);
//
//    for(unsigned int i=0; i < A.size(); i++){
//        if(A[i] <= A.size() && A[i] > 0)
//            range[A[i]-1] = 0;
//    }
//
//    for(unsigned int i=0; i < A.size(); i++)
//        if(range[i]) return 0;
//
//    return 1;
//}
//
//int main(){
//    vector<int> A = {4, 1, 3, 2};
//    cout << solution(A);
//   return 0;
//}
