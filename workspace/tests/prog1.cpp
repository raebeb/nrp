// constructing vectors
#include <iostream>
#include <vector>
#include <map>

using namespace std;

string diaSemana (int d);

int main ()
{
  // constructors used in the same order as described above:
  typedef vector<int> intVect;                        // empty vector of ints
  intVect first;
  typedef map<int,intVect> mapa;
  int i,numeroDia=1;
  mapa dia;
  first.push_back(numeroDia);
  first.push_back(14);
  first.push_back(7);
  first.push_back(7);
  dia[1]=first;
  for(i=1;i<30;i++){
    dia[i]=first;
    numeroDia++;
    if(numeroDia==8)
        numeroDia=1;
    first[0]=numeroDia;
  }
  cout << "ID\tDIA\tMANANA\tTARDE\tNOCHE"<<endl;
  mapa::iterator it;
  for (it = dia.begin(); it != dia.end(); ++it){
    cout << it->first << "\t";
    cout << diaSemana(it->second[0])<<"\t";
    cout << it->second[1]<<"\t"<< it->second[2]<<"\t"<< it->second[3]<<endl;

  }
  return 0;
}

string diaSemana (int d){
    if(d==1)
        return "LUN";
    if(d==2)
        return "MAR";
    if(d==3)
        return "MIE";
    if(d==4)
        return "JUE";
    if(d==5)
        return "VIE";
    if(d==6)
        return "SAB";
    if(d==7)
        return "DOM";
}