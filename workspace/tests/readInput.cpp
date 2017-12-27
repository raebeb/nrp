#include <iostream>
#include <string>
#include <map>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <vector>
#include <ctime>

using namespace std;

struct Employees {                          //Estructura de los funcionarios.
    string name;                            //Nombre del funcionario.
    int shiftType;                          //Tipo de turno(s) que prefiere. Por defecto, puede ser cualquiera.
    int holidays[2] = {-1,-1};              //Día de inicio y término de sus vacaciones.
    bool workingDays[7];                    //Días de la semana en que trabaja (boolean).
    vector<int> freeDays;                   //Días del mes que funcionario solicitó libre.
    vector<int> disability;                 //Días de inicio y término de discapacidad por licencia.
};

struct Settings {                           //Estructura de los turnos.
    string name;                            //Nombre del turno.
    int lenght;                             //Duración del turno en horas.
    int maxConsecutive;                     //Número de veces que se puede asignar este turno consecutivamente.
    int employeesRequired[7];               //Cantidad de funcionarios requeridos para cada día de la semana.
    vector<int> invalidNextShift;           //Turnos que no le pueden seguir a este.
};

typedef vector<string> strVect;
typedef vector<int> intVect;
typedef map<int, Settings> shiftMap;
typedef map<int, Employees> employeeMap;

void readSettings(const string &filename, shiftMap &shift);
void readEmployees(const string &filename, employeeMap &employee);
int formatErrorHandler(ifstream &input, const string &buffer, int lower = 1, int higher = 100);
int split(ifstream &input, strVect &result, const char *str, char c = ',', int lower = 1, int higher = 50);
int daysInCurrentMonth(int month, int year);
int idCheck(shiftMap &shift, int x);
int idCheck(employeeMap &employee, int x);
string moveFilePointer(ifstream &input, int linesToMove, int posCheck = 0, const string &toCheck = string());

int maxHoursPerWeek;
int maxHoursPerDay;

int main() {
    employeeMap nurses;
    shiftMap nurseReq;
    shiftMap::iterator it;
    employeeMap::iterator it2;
    readSettings("/home/ubuntu/workspace/input/settings",nurseReq);
    cout << "Horas máximas: \tDía[" << maxHoursPerDay << "]\tSemanal[" << maxHoursPerWeek << "]" << endl;
    for(it = nurseReq.begin(); it != nurseReq.end(); it++){
        int vectLenght;
        cout << "SHIFT " << it->first << ". " << it->second.name << " | " << it->second.lenght << " | " << it->second.maxConsecutive << endl;
        for(int i = 0; i < 7; i++)
            cout << "\tREQUIRED ON DAY " << i << " = " << it->second.employeesRequired[i] << endl;
        vectLenght = it->second.invalidNextShift.size();
        cout << "\tINVALID NEXT SHIFT: ";
        for(int i = 0; i < vectLenght; i++)
            cout << it->second.invalidNextShift[i] << " | ";
        cout << endl << endl;
    }
    readEmployees("/home/ubuntu/workspace/input/nurses",nurses);
    for(it2 = nurses.begin(); it2 != nurses.end(); it2++){
        int vectLenght;
        cout << "NURSE " <<it2->first << ". " << it2->second.name << " | " << it2->second.shiftType << endl;
        for(int i = 0; i < 7; i++) 
            cout << "\tWORKING DAY " << i << " = " << it2->second.workingDays[i] << endl;
        cout << "\tHOLIDAY: START | FINISH = " << it2->second.holidays[0] << " | " << it2->second.holidays[1] << endl;
        vectLenght = it2->second.disability.size();
        for(int i = 0; i < vectLenght; i += 2)
            cout << "\tDISABILITY: START | FINISH = " << it2->second.disability[i] << " | " << it2->second.disability[i+1] << endl;
        vectLenght = it2->second.freeDays.size();
        cout << "\tFREE DAY: ";
        for(int i = 0; i < vectLenght; i++)
            cout << it2->second.freeDays[i] << " | ";
        cout << endl;
    }
    return 0;
}

//Lee el archivo con la configuración para un servicio dado.
void readSettings(const string &filename, shiftMap &shift) {
    int id, vectLenght, totalShifts = 0;
    string buffer;
    ifstream input(filename.c_str());
    if(input.is_open()){
        moveFilePointer(input,5);
        for(int i = 0; !(buffer = moveFilePointer(input,1)).empty(); i++){
            strVect splitVect;
            split(input,splitVect,buffer.c_str(),'=',2,2);
            if(!i)
                maxHoursPerDay = formatErrorHandler(input,splitVect[1]);
            else
                maxHoursPerWeek = formatErrorHandler(input,splitVect[1]);
        }
        moveFilePointer(input,2,11,"Nombre");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            strVect splitVect;
            split(input,splitVect,buffer.c_str(),',',4,4);
            id = formatErrorHandler(input,splitVect[0]);
            if(idCheck(shift,id)){
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            shift[id].name = splitVect[1];
            shift[id].lenght = formatErrorHandler(input,splitVect[2],1,12);
            shift[id].maxConsecutive = formatErrorHandler(input,splitVect[3],1,7);
            totalShifts++;
        }
        moveFilePointer(input,2,11,"TurnoID(s)");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            strVect splitVect;
            int vectLenght = split(input,splitVect,buffer.c_str(),',',1,totalShifts+1);
            id = formatErrorHandler(input,splitVect[0]);
            if(!idCheck(shift,id)){
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            for(int i = 1; i < vectLenght; i++){
                int x = formatErrorHandler(input,splitVect[i]);
                if(idCheck(shift,x)){
                    shift[id].invalidNextShift.push_back(x);
                }else{
                    cout << "Error de formato. Terminando ejecucion..." << endl;
                    input.close();
                    exit(EXIT_FAILURE);
                }
            }
        }
        moveFilePointer(input,2,10,"separados");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            
        }
        moveFilePointer(input,2,11,"Cantidad");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            strVect splitVect;
            split(input,splitVect,buffer.c_str(),',',8,8);
            id = formatErrorHandler(input,splitVect[0]);
            if(!idCheck(shift,id)){
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            for(int i = 0; i < 7; i++)
                shift[id].employeesRequired[i] = formatErrorHandler(input,splitVect[i+1],0,30);
        }
    }else{
        cout << "Error en apertura del archivo '" << filename << "'. Terminando ejecucion..." << endl;
        exit(EXIT_FAILURE);
    }
    input.close();
}

//Lee los datos de cada funcionario para un servicio dado.
void readEmployees(const string &filename, employeeMap &employee) {
    time_t t = time(NULL);
    tm *timePtr = localtime(&t);
    ifstream input(filename.c_str());
    string buffer;
    int id, daysInMonth = daysInCurrentMonth(timePtr->tm_mon, timePtr->tm_year);
    if(input.is_open()){
        moveFilePointer(input,5,17,"Nombre");
        while(!(buffer = moveFilePointer(input,1)).empty()){        //Lee la primera sección del archivo, que corresponde
            strVect splitVect, week;                                //a la ID, nombre y preferencias de cada funcionario
            split(input,splitVect,buffer.c_str(),',',4,4);
            split(input,week,splitVect[2].c_str(),'|',7,7);
            id = formatErrorHandler(input,splitVect[0]);            //FuncionarioID
            if(idCheck(employee,id)){                               //Se chequea que no se agregue ID repetida
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            employee[id].name = splitVect[1];                       //Nombre funcionario
            for(int i = 0; i < 7; i++)                              //Días de la semana que el funcionario trabaja.
                employee[id].workingDays[i] = (bool)formatErrorHandler(input,week[i],0,1);
            employee[id].shiftType = formatErrorHandler(input,splitVect[3],0,10);
        }
        moveFilePointer(input,2,17,"Día");
        while(!(buffer = moveFilePointer(input,1)).empty()){        //Sección correspondiente a días libres por solicitud.
            strVect splitVect;
            int vectLenght = split(input,splitVect,buffer.c_str(),',',2,daysInMonth);
            id = formatErrorHandler(input,splitVect[0]);
            if(!idCheck(employee,id)){                              //Se chequea que FuncionarioID exista previamente.
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            for(int i = 1; i < vectLenght; i++)                     //Se agregan los días al vector de la estrucutura.
                employee[id].freeDays.push_back(formatErrorHandler(input,splitVect[i],1,daysInMonth));
        }
        moveFilePointer(input,2,17,"Día");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            strVect splitVect;
            split(input,splitVect,buffer.c_str(),',',3,3);
            id = formatErrorHandler(input,splitVect[0]);
            if(!idCheck(employee,id)){
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            for(int i = 1; i < 3; i++)
                employee[id].holidays[i-1] = formatErrorHandler(input,splitVect[i],0,daysInMonth);
        }
        moveFilePointer(input,2,17,"Día");
        while(!(buffer = moveFilePointer(input,1)).empty()){
            strVect splitVect;
            split(input,splitVect,buffer.c_str(),',',3,3);
            id = formatErrorHandler(input,splitVect[0]);
            if(!idCheck(employee,id)){
                cout << "Error de formato. Terminando ejecucion..." << endl;
                input.close();
                exit(EXIT_FAILURE);
            }
            for(int i = 1; i < 3; i++)
                employee[id].disability.push_back(formatErrorHandler(input,splitVect[i],0,daysInMonth));
        }
    }else{
        cout << "Error en apertura del archivo '" << filename << "'. Terminando ejecucion..." << endl;
        exit(EXIT_FAILURE);
    }
    input.close();
}

int idCheck(shiftMap &shift, int x) {
    if(shift.find(x) == shift.end())
        return 0;
    else
        return 1;
}

int idCheck(employeeMap &employee, int x) {
    if(employee.find(x) == employee.end())
        return 0;
    else
        return 1;
}

//Función para separar un string según caractér especificado. 
//Extraído de https://stackoverflow.com/questions/53849/how-do-i-tokenize-a-string-in-c
int split(ifstream &input, strVect &result, const char *str, char c, int lower, int higher) {
    int vectLenght;
    const char *begin = NULL;
    do{
        begin = str;
        while(*str != c && *str)
            str++;
        result.push_back(string(begin, str));
    }while(*str++ != 0);
    if((vectLenght = result.size()) >= lower && vectLenght <= higher){
        return vectLenght;
    }else{
        cout << "Error de formato. Terminando ejecucion..." << endl;
        input.close();
        exit(EXIT_FAILURE);
    }
}

//Mueve puntero del archivo la cantidad de líneas especificada en parámetros.
string moveFilePointer(ifstream &input, int linesToMove, int posCheck, const string &toCheck) {
    int i;
    string buffer;
    for(i = 0; i < linesToMove; i++)
        getline(input,buffer);
    if(!toCheck.empty() && buffer.find(toCheck,posCheck) == string::npos){
        cout << "Error de formato. Terminando ejecucion..." << endl;
        input.close();
        exit(EXIT_FAILURE);
    }
    return (string)buffer;
}

int formatErrorHandler(ifstream &input, const string &buffer, int lower, int higher) {
    int x;
    string::const_iterator it = buffer.begin();
    while(it != buffer.end() && isdigit(*it))
        it++;
    if(!buffer.empty() && it == buffer.end()){
        x = atoi(buffer.c_str());
        if(x >= lower && x <= higher)
            return x;
    }
    cout << "Error de formato. Terminando ejecucion..." << endl;
    input.close();
    exit(EXIT_FAILURE);
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