/*
 * Copyright (c) 2017
 *
 * You may not use this file except in communication with jackietang2006@gmail.com.
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * This program is to print out all of the Dearrangements using depth-first recursion.
 * I use the associative map to remember the original ordinal of each element
 */
#include <unistd.h>
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
    map<char, int>::iterator iter = aMap.begin();
    map<char, int> aMap2;
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
    cout << "Please input Characters to permute: ";
    map<char, int> characters;
    char x = 0, ordinal = 0;
    while ( cin >> x ) {
        characters[x] = ordinal++;
    }

    int perm = recur_perm(characters);
    cout << "Number of Dearrangements: " << perm << endl;
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
 * Please input Characters to permute: A B C
 * Number of Dearrangements: 2
 * B C A 
 * C A B 
 */
