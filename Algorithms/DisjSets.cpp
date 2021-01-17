#include <vector>
using namespace std;

class DisjSets{
public:
    explicit DisjSets(int numElements):s(numElements,-1){};
    int find(int x){
        if(s[x] < 0){
            return x;
        }else{
            return s[x] = find(s[x]); //路径压缩
        }
    }
    void unionSets(int root1, int root2){
        if(s[root2] < s[root1]){ //高度低的合并到高度高的
            s[root1] = root2;
        }else{
            if(s[root1] == s[root2]){
                --s[root1];
            }
            s[root2] = root1;
        }
    }
private:
    vector<int> s;
};