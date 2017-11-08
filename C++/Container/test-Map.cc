#include <iostream>
#include <map>
#include <string>
#include <iterator>
 
std::map<std::string, int>::iterator serachByValue(std::map<std::string, int> & mapOfWords, int val)
{
    // Iterate through all elements in std::map and search for the passed element
    std::map<std::string, int>::iterator it = mapOfWords.begin();
    while(it != mapOfWords.end())
    {
        if(it->second == val)
        return it;
        it++;
    }
}
int main()
{
    std::map<std::string, int> mapOfWords;
    // Inserting data in std::map
    mapOfWords.insert(std::make_pair("earth", 1));
    mapOfWords.insert(std::make_pair("moon", 2));
    mapOfWords["sun"] = 3;
 
    std::map<std::string, int>::iterator it = serachByValue(mapOfWords, 3);
    if(it != mapOfWords.end())
        std::cout<<it->first<<" :: "<<it->second<<std::endl;
 
return 0;
}
