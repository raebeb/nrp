#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>

using namespace std;

//Mapa para cada funcionario a planificar.
typedef vector<string> strVect;
typedef vector<int> intVect;
typedef map<int, string> intStringMap;
typedef map<int, strVect> intStrVectMap;
typedef map<int, intVect> intIntVectMap;
typedef map<intStringMap,intVect> horarioMapType;

intStringMap nurses;
intStringMap assistants;
intStringMap paramedics;
intStringMap kinesiology;

void initNurses(string filename, map<int, string>& theMap);
void schedulerMonth(int month, int year);
//void schedulerWeek();
//void schedulerDay();
int daysInCurrentMonth(int month, int year);
int firstDayOfMonth(int month, int year);
string dayOfWeek(int dayWeek);

int main() {
    time_t t = time(NULL);                                                                          //se guarda fecha y hora actual en la variable t.
    tm *timePtr = localtime(&t);                                                                    //se transforma la variable a formato tm.
    intStringMap::iterator pos;                                                                     //se crea iterador con nombre pos.
    initNurses("/home/ubuntu/workspace/input/input.csv",nurses);                                       //Se llama a la funcion initMap para leer por archivo
    //initMap("/home/ubuntu/workspace/input/input.csv",assistants);
    //initMap("/home/ubuntu/workspace/input/input.csv",paramedics);
    //initMap("/home/ubuntu/workspace/input/input.csv",kinesiology);
    for(pos = nurses.begin(); pos != nurses.end(); ++pos)                                           //Print de lo leido (BORRAR luego)
        cout << "ID: " << pos->first << "\t\tValue: " << pos->second << endl;
    schedulerMonth(timePtr->tm_mon + 1, timePtr->tm_year + 1900);                                   //Funcion para crear el horario
    return 0;
}

//Carga cada mapa con los funcionarios correspondientes según archivo de entrada.
void initNurses(string filename, map<int, string>& theMap) {
    int i;
    string buffer,token;                                                                                  //BUFFER donde se guardara lo que se leera de archivo
    ifstream input;                                                                                 //variable input tipo ifstream para abrir archivo 
    input.open(filename.c_str());                                                                   //Apertura de archivo if condicional para verificar que no se tienen errores.
    if(input.is_open()){
        while(!input.eof()){
            input >> buffer;
            istringstream iss(buffer);                                                              //Separacion del archivo en tokens (debido al formato csv)
            getline(iss,token,',');                                                                  
            i = atoi(token.c_str());
            std::getline(iss,token,',');
            theMap[i]=token;                                                                        //asignacion al map de funcionarios (de cierta area)
        }
    }else{
        cout << "Error al abrir archivo" << endl;
        return;
    }
    input.close();
}

//Planificador mensual para cada funcionario.
void schedulerMonth(int month, int year) {
    int daysInMonth, i, dayNumber;                                              // Declaracion de variables //
    intVect dayRequirements;                                                    //                          //
    intIntVectMap dayMap;                                                       //                          //
    horarioMapType horarioIndividual;                                           //////////////////////////////          
    dayNumber = firstDayOfMonth(month,year);
    daysInMonth = daysInCurrentMonth(month,year);
    dayRequirements.push_back(dayNumber);
    /*for(i=0; i<schedulesTypes;i++){
        dayRequirements.push_back();
    }*/
    for(i = 1; i <= daysInMonth; i++){
        dayMap[i] = dayRequirements;
        dayNumber++;
        if(dayNumber == 7)
            dayNumber = 0;
        dayRequirements[0] = dayNumber;
    }
    cout << endl << "ID\tDIA\tMANANA\tTARDE\tNOCHE" << endl << endl;
    intIntVectMap::iterator it;
    for(it = dayMap.begin(); it != dayMap.end(); it++){
        cout << it->first << "\t";
        cout << dayOfWeek(it->second[0]) << "\t";
        cout << it->second[1] << "\t" << it->second[2] << "\t" << it->second[3] << endl;
    }
}

//Retorna la cantidad de días que hay en el mes actual.
int daysInCurrentMonth(int month, int year) {
    if(month==4 || month==6 || month==9 || month==11){
        return 30;
    }else if(month == 2){
        if(year % 4 == 0 && (year % 100 != 0 || year % 400 == 0))       //Año bisiesto
            return 29;
        else
            return 28;
    }else{
        return 31;
    }
}

//Retorna el día de la semana correspondiente al primer día del mes.
int firstDayOfMonth(int month, int year) {
    tm time_in = {0, 0, 0, 1, month-1, year - 1900};
    time_t time_temp = mktime(&time_in);
    tm *time_out = localtime(&time_temp);
    return time_out->tm_wday;
}

//Retorna el día de la semana.
string dayOfWeek(int dayWeek) {
    if(dayWeek == 1)
        return "LUN";
    else if(dayWeek == 2)
        return "MAR";
    else if(dayWeek == 3)
        return "MIE";
    else if(dayWeek == 4)
        return "JUE";
    else if(dayWeek == 5)
        return "VIE";
    else if(dayWeek == 6)
        return "SAB";
    else
        return "DOM";
}