/*
 * This program is to print out all of the Dearrangements using depth-first recursion.
 * I use the associative map to remember the original ordinal of each element
 */
#include <iostream>
#include <vector>
#include <map>
#include <deque>

using namespace std;

vector< deque<char> > mVector;
deque<char>           mDeque;

int recur_perm(map<char, int>& aMap) {
    if( aMap.empty() && !mDeque.empty() ) {
        mVector.push_back(mDeque);
        return 1;
    }
    
    int perm = 0;
    map<char, int> aMap2;
    map<char, int>::iterator iter = aMap.begin();
    map<char, int>::iterator iter2;
    for (; iter!=aMap.end(); iter++)
    {
        //Skip the case where iter will be at the original position
        if( iter->second == mDeque.size() ) {
            continue;
        }
        aMap2 = aMap;
        iter2 = aMap2.find(iter->first);
        mDeque.push_back(iter->first);
        aMap2.erase(iter2);
        perm += recur_perm(aMap2);
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
    map<char, int> characters;
    char x = 0, ordinal = 0;
    while ( cin >> x ) {
        characters[x] = ordinal++;
    }

    map<char, int>::iterator iter = characters.begin();
    for( ; iter != characters.end(); iter++)
    {
        cout << iter->first << " ";
    }
    cout << endl;

    int perm = recur_perm(characters);
    cout << "Number of Dearrangements: " << perm << endl;
    //cout << "Number of Dearrangements: " << mVector.size() << endl;
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

/*
 * How to Run:
 * Please input Characters to permute: A B C D E
 * A B C D E 
 * Number of Dearrangements: 44
 * B A D E C 
 * B A E C D 
 * B C A E D 
 * B C D E A 
 * B C E A D 
 * B D A E C 
 * B D E A C 
 * B D E C A 
 * B E A C D 
 * B E D A C 
 * B E D C A 
 * C A B E D 
 * C A D E B 
 * C A E B D 
 * C D A E B 
 * C D B E A 
 * C D E A B 
 * C D E B A 
 * C E A B D 
 * C E B A D 
 * C E D A B 
 * C E D B A 
 * D A B E C 
 * D A E B C 
 * D A E C B 
 * D C A E B 
 * D C B E A 
 * D C E A B 
 * D C E B A 
 * D E A B C 
 * D E A C B 
 * D E B A C 
 * D E B C A 
 * E A B C D 
 * E A D B C 
 * E A D C B 
 * E C A B D 
 * E C B A D 
 * E C D A B 
 * E C D B A 
 * E D A B C 
 * E D A C B 
 * E D B A C 
 * E D B C A 
 */
