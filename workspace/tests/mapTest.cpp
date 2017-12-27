#include <iostream>
#include <map>
#include <string>
using namespace std;

int main(){
    typedef map<int, string> nurses;
    nurses id;
    id[1] = "Andrea";
    id[2] = "Pelito";
    id[3] = "Jamon";
    id[4] = "HOLA";
    nurses::iterator pos;
    for(pos = id.begin();pos !=id.end() ; ++pos){
        cout << "ID \" "<<pos->first<< "\" "<<"value: "<< pos->second << endl;
    }
    cout << "--------------------------"<<endl;
    id.erase(2);
    for(pos = id.begin();pos !=id.end() ; ++pos){
        cout << "ID \" "<<pos->first<< "\" "<<"value: "<< pos->second << endl;
    }
    cout << "--------------------------"<<endl;
    pos=id.begin();
    ++pos;
    id.erase(pos);
    for(pos = id.begin();pos !=id.end() ; ++pos){
        cout << "ID \" "<<pos->first<< "\" "<<"value: "<< pos->second << endl;
    }
    cout << "--------------------------"<<endl;
    id[2] = "Pelito";
    id[3] = "Jamon";
    id[5] = "Ceci";
    for(pos = id.begin();pos !=id.end() ; ++pos){
        cout << "ID \" "<<pos->first<< "\" "<<"value: "<< pos->second << endl;
    }
    cout << "--------------------------"<<endl;
    pos=id.begin();
    ++pos;
    id.erase(pos,id.end());
    for(pos = id.begin();pos !=id.end() ; ++pos){
        cout << "ID \" "<<pos->first<< "\" "<<"value: "<< pos->second << endl;
    }
    cout << "--------------------------"<<endl;
    
    return 0;
}
