#include <iostream>
#include <vector>
#include <deque>

using namespace std;

vector< deque<char> > mVector;
deque<char>           mDeque;

int recur_perm(vector<char>& aVec) {
    if( aVec.empty() && !mDeque.empty() ) {
        mVector.push_back(mDeque);
        return 1;
    }
    
    int perm = 0;
    vector<char> aVec2;
    vector<char>::iterator iter = aVec.begin();
    vector<char>::iterator iter2;
    for (int count = 0; iter!=aVec.end(); iter++, count++)
    {
        aVec2 = aVec;
        iter2 = aVec2.begin() + count;
        mDeque.push_back(*iter);
        aVec2.erase(iter2);
        perm += recur_perm(aVec2);
        mDeque.pop_back();
    }
    return perm;
}

int main() {
/*
    int x;
    while ( cin >> x ) {
        cout << x << " ";
    }
*/

    cout << "Please input Characters to permute: ";
    vector<char> characters;
    characters.reserve(10);
    char x = 0, index = 0;
    while ( cin >> x ) {
        characters.push_back(x);
    }

    vector<char>::iterator viter = characters.begin();
    //for(; viter != characters.end(); viter++)
    for(int i = 0; i < characters.size(); i++)
    {
        //cout << *viter << endl;
        cout << characters[i] << endl;
    }
    int perm = recur_perm(characters);
    cout << "Number of Permutations: " << perm << endl;
    cout << "Number of Permutations: " << mVector.size() << endl;
    deque<char>::iterator diter;
    for( int i = 0; i < mVector.size(); i++ )
    {
        diter = mVector[i].begin();
        for(; diter != mVector[i].end(); diter++ )
        {
            cout << *diter << " ";
        }
        cout << endl;
    }
}
