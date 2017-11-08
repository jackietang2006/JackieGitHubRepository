#include "utilities.h"
 
int main()
{
    FIFO<char*> myFifo;
    myFifo.enqueue((char*)"Aackie");
    myFifo.enqueue((char*)"Backie");
    myFifo.enqueue((char*)"Cackie");
    myFifo.enqueue((char*)"Dackie");
    myFifo.enqueue((char*)"Eackie");
    myFifo.enqueue((char*)"Fackie");
    myFifo.enqueue((char*)"Gackie");

    FIFO<char*>::Node* node = NULL;
    while( node = myFifo.dequeue() )
    {
      cout << ".dequeue: " << node->value << endl;
    }
}
