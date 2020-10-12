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
 * Recursively calculating the number of ways to pay $17
 * out of $1 coin(#), $2 coin(#), $5 coin(#) & bill($), $10 coin(#) & bill($).
 */
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <deque>
std::deque<char*> mydeque;

int recur(int amount) {
    if (amount == 0) {
        for(std::deque<char*>::iterator it = mydeque.begin(); it != mydeque.end(); it++) {
            std::cout << *it;
        }
        std::cout << std::endl;
        return 1;
    }
    else if (amount < 0) {
        return 0;
    }

    int nWays = 0;
    if ( (amount-1) >= 0 ) {
      mydeque.push_back((char*)"#1 ");
      nWays += recur(amount - 1);
      mydeque.pop_back();
    }
    if ( (amount-2) >= 0 ) {
      mydeque.push_back((char*)"#2 ");
      nWays += recur(amount - 2);
      mydeque.pop_back();
    }
    if ( (amount-5) >= 0 ) {
      mydeque.push_back((char*)"#5 ");
      nWays += recur(amount - 5);
      mydeque.pop_back();
      mydeque.push_back((char*)"$5 ");
      nWays += recur(amount - 5);
      mydeque.pop_back();
    }
    if ( (amount-10) >= 0 ) {
      mydeque.push_back((char*)"#10 ");
      nWays += recur(amount - 10);
      mydeque.pop_back();
      mydeque.push_back((char*)"$10 ");
      nWays += recur(amount - 10);
      mydeque.pop_back();
    }

    return nWays;
}

int main(int argc, char *argv[]) {
    int amount = 0;
    std::cout << "Please input the amount of bill to pay: ";
    std::cin >> amount;

    printf("There are %d ways to pay the bill of $%d.\n", recur(amount), amount);
}
