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
 * out of $1 coin, $2 coin, $5 coin & bill, $10 coin & bill.
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

    int a_n = 0;
    if ( (amount-1) >= 0 ) {
      mydeque.push_back((char*)"$1 ");
      a_n += recur(amount - 1);
      mydeque.pop_back();
    }
    if ( (amount-2) >= 0 ) {
      mydeque.push_back((char*)"$2 ");
      a_n += recur(amount - 2);
      mydeque.pop_back();
    }
    if ( (amount-5) >= 0 ) {
      mydeque.push_back((char*)"$5 ");
      a_n += recur(amount - 5);
      mydeque.pop_back();
      mydeque.push_back((char*)"#5 ");
      a_n += recur(amount - 5);
      mydeque.pop_back();
    }
    if ( (amount-10) >= 0 ) {
      mydeque.push_back((char*)"$10 ");
      a_n += recur(amount - 10);
      mydeque.pop_back();
      mydeque.push_back((char*)"#10 ");
      a_n += recur(amount - 10);
      mydeque.pop_back();
    }

    return a_n;
}

int main(int argc, char *argv[]) {
    if (argc < 2) {
        fprintf(stderr, "ERROR, please give the amount of bill to pay\n");
        exit(1);
    }

    int amount = atoi(argv[1]);
    printf("There are %d ways to pay the bill of $%d.\n", recur(amount), amount);
}
