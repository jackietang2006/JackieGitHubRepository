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
        cout << characters[i] << " ";
    }
    cout << endl;

    int perm = recur_perm(characters);
    cout << "Number of Permutations: " << perm << endl;
    //cout << "Number of Permutations: " << mVector.size() << endl;
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
 * Number of Permutations: 120
 * A B C D E 
 * A B C E D 
 * A B D C E 
 * A B D E C 
 * A B E C D 
 * A B E D C 
 * A C B D E 
 * A C B E D 
 * A C D B E 
 * A C D E B 
 * A C E B D 
 * A C E D B 
 * A D B C E 
 * A D B E C 
 * A D C B E 
 * A D C E B 
 * A D E B C 
 * A D E C B 
 * A E B C D 
 * A E B D C 
 * A E C B D 
 * A E C D B 
 * A E D B C 
 * A E D C B 
 * B A C D E 
 * B A C E D 
 * B A D C E 
 * B A D E C 
 * B A E C D 
 * B A E D C 
 * B C A D E 
 * B C A E D 
 * B C D A E 
 * B C D E A 
 * B C E A D 
 * B C E D A 
 * B D A C E 
 * B D A E C 
 * B D C A E 
 * B D C E A 
 * B D E A C 
 * B D E C A 
 * B E A C D 
 * B E A D C 
 * B E C A D 
 * B E C D A 
 * B E D A C 
 * B E D C A 
 * C A B D E 
 * C A B E D 
 * C A D B E 
 * C A D E B 
 * C A E B D 
 * C A E D B 
 * C B A D E 
 * C B A E D 
 * C B D A E 
 * C B D E A 
 * C B E A D 
 * C B E D A 
 * C D A B E 
 * C D A E B 
 * C D B A E 
 * C D B E A 
 * C D E A B 
 * C D E B A 
 * C E A B D 
 * C E A D B 
 * C E B A D 
 * C E B D A 
 * C E D A B 
 * C E D B A 
 * D A B C E 
 * D A B E C 
 * D A C B E 
 * D A C E B 
 * D A E B C 
 * D A E C B 
 * D B A C E 
 * D B A E C 
 * D B C A E 
 * D B C E A 
 * D B E A C 
 * D B E C A 
 * D C A B E 
 * D C A E B 
 * D C B A E 
 * D C B E A 
 * D C E A B 
 * D C E B A 
 * D E A B C 
 * D E A C B 
 * D E B A C 
 * D E B C A 
 * D E C A B 
 * D E C B A 
 * E A B C D 
 * E A B D C 
 * E A C B D 
 * E A C D B 
 * E A D B C 
 * E A D C B 
 * E B A C D 
 * E B A D C 
 * E B C A D 
 * E B C D A 
 * E B D A C 
 * E B D C A 
 * E C A B D 
 * E C A D B 
 * E C B A D 
 * E C B D A 
 * E C D A B 
 * E C D B A 
 * E D A B C 
 * E D A C B 
 * E D B A C 
 * E D B C A 
 * E D C A B 
 * E D C B A 
 */
