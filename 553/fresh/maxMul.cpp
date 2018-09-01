#include<limits>
#include <vector>
#include <iostream>
#include <algorithm>
using namespace std;
int solution(vector<int> &A) {
    //Keep the absolute value for max 2 -ve num . As their mul will be +ve
    int abs1 = numeric_limits<int>::min();
    int abs2 = numeric_limits<int>::min();

    //Keep the max three num irrespective of sign
    int max1 = numeric_limits<int>::min();
    int max2 = numeric_limits<int>::min();
    int max3 = numeric_limits<int>::min();

    unsigned int size = A.size()-1;

    for (unsigned int i = 0; i <= size ; ++i) {
        if(A[i] > 0 ){

        } else if(abs(A[i]) >= abs1 ) {
            abs2 = abs1;
            abs1 = abs(A[i]);
        }else if(abs(A[i]) >= abs2 ) {
            abs2 = abs(A[i]);
        }

        if(A[i] >= max1 ){
            //Push max1s prev value to max2 and max2's prev val to max3
            max3 = max2;
            max2 = max1;
            max1 = A[i];
        } else if(A[i] >= max2 ) {
            max3 = max2;
            max2 = A[i];
        }else if(A[i] > max3 ) {
            max3 = A[i];
        }
    }
    // Either max 3 multiplication , or Max 2 negative num whose mul is +ve and the regular max
    return max(max1 * max2 * max3, abs1 * abs2 * max1);
}


int main(){
    vector<int> test = {-3, 1, 2, -2, 5, 6};
    cout << solution(test);
    return 0;
}