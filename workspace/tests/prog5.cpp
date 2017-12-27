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
intIntVectMap addFourthShift (intIntVectMap input,int id);
int formatErrorHandler(const string &buffer);
intStrVectMap readSettings(string filename, intIntVectMap &requirements);
string moveFilePointer(ifstream &input, int linesToMove);
strVect split(const char *str, char c);
void schedulerMonth(int month, int year,intIntVectMap auxReq,intStrVectMap auxShifts);

int main() {
    time_t t = time(NULL);                                                                          //se guarda fecha y hora actual en la variable t.
    tm *timePtr = localtime(&t);                                                                    //se transforma la variable a formato tm.
    intStringMap::iterator pos;                                                                     //se crea iterador con nombre pos.
    intIntVectMap reqNurses, reqAssistants, reqParamedics, reqKinesiology;                          //Requerimientos para cada servicio.
    intStrVectMap shiftNurses, shiftAssistants, shiftParamedics, shiftKinesiology;                  //Detalle de turnos para cada servicio.
    initNurses("/home/ubuntu/workspace/input/input.csv",nurses);                                       //Se llama a la funcion initMap para leer por archivo
    //initMap("/home/ubuntu/workspace/input/input.csv",assistants);
    //initMap("/home/ubuntu/workspace/input/input.csv",paramedics);
    //initMap("/home/ubuntu/workspace/input/input.csv",kinesiology);
    shiftNurses = readSettings("/home/ubuntu/workspace/input/settings", reqNurses);
    schedulerMonth(timePtr->tm_mon + 1, timePtr->tm_year + 1900,reqNurses,shiftNurses);                                   //Funcion para crear el horario
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
void schedulerMonth(int month, int year,intIntVectMap auxReq,intStrVectMap auxShifts) {
    int daysInMonth, i, dayNumber, x,idNurse,assingedDays;
    intVect first,cuarto_turno;
    intIntVectMap dayMap;
    intIntVectMap::iterator it;
    dayNumber = firstDayOfMonth(month,year);
    daysInMonth = daysInCurrentMonth(month,year);
    intIntVectMap horarioIndividual;
    first.push_back(dayNumber);
    for(it=auxReq.begin();it!=auxReq.end();it++){
        first.push_back(it->second[1]);
    }
    
    cuarto_turno.push_back(1);cuarto_turno.push_back(2);cuarto_turno.push_back(0);cuarto_turno.push_back(0);
    
    for(i = 1; i <= daysInMonth; i++){
        dayMap[i] = first;
        dayNumber++;
        if(dayNumber == 7)
            dayNumber = 0;
        first[0] = dayNumber;
    }
    cout << endl << "ID\tDIA\tMANANA\tTARDE\tNOCHE" << endl << endl;
   
    for(it = dayMap.begin(); it != dayMap.end(); it++){
        cout << it->first << "\t";
        cout << dayOfWeek(it->second[0]) << "\t";
        cout << it->second[1] << "\t" << it->second[2] << "\t" << it->second[3] << endl;
    }
    intStringMap::iterator it2;
    for(i= 1; i<31; i++){
        while(dayMap[i][1]>0){
            for(it2=nurses.begin();it2!=nurses.end();++it2){
                idNurse=it2->first;
                assingedDays=horarioIndividual[idNurse].size();
                if(assingedDays==0&&dayMap[i+1][2]>0){
                    horarioIndividual=addFourthShift(horarioIndividual,idNurse);
                    dayMap[i][1]--;
                    dayMap[i+1][2]--;
                    break;
                }
                if(assingedDays==0&&dayMap[i+1][2]==0){
                    horarioIndividual[idNurse].push_back(1);
                    dayMap[i][1]--;
                    break;
                }
                if(assingedDays<i){
                    if(dayMap[i+1][2]>0){
                        if(horarioIndividual[idNurse].size()<i&&horarioIndividual[idNurse][i-2]==0&&horarioIndividual[idNurse][i-2]!=1){
                            horarioIndividual=addFourthShift(horarioIndividual,idNurse);
                            dayMap[i][1]--;
                            dayMap[i+1][2]--;
                            break;
                        }
                    }
                    if(dayMap[i+1][2]==0){
                        if(assingedDays<i){
                            if(assingedDays<2&&(horarioIndividual[idNurse][i-2]==0||horarioIndividual[idNurse][i-2]==1)){
                               horarioIndividual[idNurse].push_back(1);
                               dayMap[i][1]--;
                               break;
                            }
                            if(horarioIndividual[idNurse][i-2]==1){
                                if(horarioIndividual[idNurse][i-3]==0){
                                    horarioIndividual[idNurse].push_back(1);
                                    dayMap[i][1]--;
                                    break;
                                }
                                horarioIndividual[idNurse].push_back(0);
                            }
                            if(horarioIndividual[idNurse][i-2]==0){
                                horarioIndividual[idNurse].push_back(1);
                                dayMap[i][1]--;
                                break;
                            }
                        }
                    }
                }
            }
        }
        cout << "Night Shift" << endl;
        while(dayMap[i][2]>0){
            for(it2=nurses.begin();it2!=nurses.end();++it2){
                idNurse=it2->first;
                assingedDays=horarioIndividual[idNurse].size();
                if(assingedDays==0){
                    horarioIndividual[idNurse].push_back(2);
                    dayMap[i][2]--;
                    break;
                }
                if(assingedDays<i){
                    if(dayMap[i][2]>0){
                        horarioIndividual[idNurse].push_back(2);
                        dayMap[i][2]--;
                        break;
                    }
                }
            }
        }
        cout << "Free Shift" << endl;
        for(it2=nurses.begin();it2!=nurses.end();++it2){
            idNurse=it2->first;
            assingedDays=horarioIndividual[idNurse].size();
            if(assingedDays<i){
                horarioIndividual[idNurse].push_back(0);
            }
            if(horarioIndividual[idNurse].size()==0){
                horarioIndividual[idNurse].push_back(0);
            }
        }
    }
    cout << "\n*************************************************\n";
    for(it = horarioIndividual.begin(); it != horarioIndividual.end(); ++it){
        cout << it->first << "\t" << nurses[it->first] << "\t\tHorario:|";
        x = it->second.size();
        for(i = 0; i < x; i++){
            cout << it->second[i] << "|";
        }
        cout<<endl;
    }
    for(it = dayMap.begin(); it != dayMap.end(); it++){
        cout << it->first << "\t";
        cout << dayOfWeek(it->second[0]) << "\t";
        cout << it->second[1] << "\t" << it->second[2] << "\t" << it->second[3] << endl;
    }
}

//Añade el cuarto turno a la enfermera id
intIntVectMap addFourthShift (intIntVectMap input,int id){
    input[id].push_back(1);
    input[id].push_back(2);
    input[id].push_back(0);
    input[id].push_back(0);
    return input;
}
//Añade el tercer turno a la enfermera id
intIntVectMap addThirdShift (intIntVectMap input,int id){
    input[id].push_back(1);
    input[id].push_back(2);
    input[id].push_back(0);
    return input;
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
//Lee el archivo con la configuración global.
intStrVectMap readSettings(string filename, intIntVectMap &requirements) {
    int i, j, ct=1, ct2=0, schedulesTypes=0;
    strVect composition;
    intVect maxReq;
    intStrVectMap shifts;
    string buffer;
    ifstream input(filename.c_str());
    if(input.is_open()){
        buffer = moveFilePointer(input,6);
        while(!(buffer = moveFilePointer(input,1)).empty()){
            composition = split(buffer.c_str(),',');
            composition.erase(composition.begin());
            shifts[ct] = composition;
            if(composition.size() == 1)
                schedulesTypes++;
            composition.clear();
            ct++;
        }
        buffer = moveFilePointer(input,2);
        for(i = 1; i <= schedulesTypes; i++){                                                           //Se leen los requisitos de cada turno atómico.
            buffer = moveFilePointer(input,1);
            composition = split(buffer.c_str(),',');
            for(j = 0; j < 2; j++){
                ct = formatErrorHandler(composition[j]);
                maxReq.push_back(ct);
            }
            requirements[i] = maxReq;
            maxReq.clear();
            composition.clear();
            ct2++;
        }
    }else{
        cout << "Error al abrir el archivo Settings. Terminando ejecución..." << endl;
        exit(EXIT_FAILURE);
    }
    input.close();
    return shifts;
}

//Función para separar un string según caractér especificado. 
//Extraído de https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
strVect split(const char *str, char c) {
    const char *begin = NULL;
    vector<string> result;
    do{
        begin = str;
        while(*str != c && *str)
            str++;
        result.push_back(string(begin, str));
    }while(*str++ != 0);
    return result;
}

//Mueve puntero del archivo la cantidad de líneas especificada en parámetros.
string moveFilePointer(ifstream &input, int linesToMove) {
    int i;
    string buffer;
    for(i = 0; i < linesToMove; i++){
        getline(input,buffer);
    }
    if(input.eof()){
        cout << "Error de formato. Terminando ejecuccion..." << endl;
        exit(EXIT_FAILURE);
    }
    return (string)buffer;
}

int formatErrorHandler(const string &buffer) {
    int x;
    string::const_iterator it = buffer.begin();
    while(it != buffer.end() && isdigit(*it))
        it++;
    if(!buffer.empty() && it == buffer.end()){
        x = atoi(buffer.c_str());
        return x;
    }else{
        cout << "Error de formato. Terminando ejecucion..." << endl;
        exit(EXIT_FAILURE);
    }
}