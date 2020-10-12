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
* This program is to print out all of the Permutations using depth-first recursion.
*/
#include <unistd.h>
#include <iostream>
#include <vector>
#include <deque>

using namespace std;

vector< deque<char> > mVector;
deque<char>           mDeque;

int recur_perm(vector<char>& aVec) {
    if( aVec.empty() ) {
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
    cout << "Please input Characters to permute: ";
    vector<char> characters;
    characters.reserve(10);
    char x = 0, index = 0;
    while ( cin >> x ) {
        characters.push_back(x);
    }


    int perm = recur_perm(characters);
    cout << "Number of Permutations: " << perm << endl;

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
 * A B C 
 * A C B 
 * B A C 
 * B C A 
 * C A B 
 * C B A 
 * Number of Permutations: 6
 */
