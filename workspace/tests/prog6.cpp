#include <iostream>
#include <fstream>
#include <sstream>
#include <algorithm>
#include <string>
#include <cstdlib>
#include <vector>
#include <map>
#include <random>

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
typedef map<int, bool> intBoolMap;
typedef map<int, Settings> shiftMap;
typedef map<int, Employees> employeeMap;
typedef map<int, intVect> intIntVectMap;

/*                                                        Cabeceras de funciones                                                */
void readSettings(const string &filename, shiftMap &shift);
void readEmployees(const string &filename, employeeMap &employee);
void schedulerMonth(int month, int year, shiftMap &nurseReq, employeeMap &nurses);
int split(ifstream &input, strVect &result, const char *str, char c = ',', int lower = 1, int higher = 50);
int formatErrorHandler(ifstream &input, const string &buffer, int lower = 1, int higher = 100);
int employeeRandomizer(employeeMap &employee, intVect &employeeReady, int maxEmployeeID);
int consecutiveDays(intIntVectMap &personalSchedule, int employeeID);
int daysInCurrentMonth(int month, int year);
int firstDayOfMonth(int month, int year);
int highestID(employeeMap &employees);
int dayOfWeek(const string &weekDay);
int idCheck(shiftMap &shift, int x);
int idCheck(employeeMap &employee, int x);
string moveFilePointer(ifstream &input, int linesToMove, int posCheck = 0, const string &toCheck = string());
string dayOfWeek(int weekDay);
bool canSched(struct Settings &shiftData, intIntVectMap &personalSchedule, int employeeID, int dayNumber);
bool isWorkingDay(struct Employees &employeeData, int day, int dayNumber, int shiftID);
bool weekMax(intIntVectMap &personalSchedule, intIntVectMap &dayMap, int employeeID, int dayNumber, shiftMap &shift);
intIntVectMap schedulerGeneretor(shiftMap &shift, int daysInMonth, int currentDay);
intIntVectMap assingShift(employeeMap &nurses, shiftMap &nurseReq, intIntVectMap &dayMap, int daysInMonth);

//Variables Globales//
int maxHoursPerWeek;
int maxHoursPerDay;

int main (int argc,char *argv[]){
    time_t t = time(NULL);
    tm *timePtr = localtime(&t);
    shiftMap nurseReq;
    employeeMap nurses;
    readSettings(argv[1],nurseReq);
    readEmployees(argv[2],nurses);
    schedulerMonth(timePtr->tm_mon + 1, timePtr->tm_year + 1900, nurseReq, nurses);
    return 0;
}

intIntVectMap schedulerGeneretor(shiftMap &shift, int daysInMonth, int currentDay) {
    int auxDay;
    intIntVectMap dayMap;
    for(int i = 1; i <= daysInMonth; i++){
        dayMap[i].push_back(currentDay);
        for(shiftMap::iterator it = shift.begin(); it != shift.end(); it++){
            auxDay = currentDay - 1;
            if(auxDay == -1)
                auxDay = 6;
            dayMap[i].push_back(it->second.employeesRequired[auxDay]);
        }
        currentDay++;
        if(currentDay == 7)
            currentDay = 0;
    }
    return dayMap;
}

void schedulerMonth(int month, int year, shiftMap &shift, employeeMap &employees) {
    int daysInMonth, currentDay;
    intIntVectMap dayMap, personalSchedule;
    
    currentDay = firstDayOfMonth(month,year);
    daysInMonth = daysInCurrentMonth(month,year);
    dayMap = schedulerGeneretor(shift,daysInMonth,currentDay);
    
    for(intIntVectMap::iterator it = dayMap.begin(); it != dayMap.end(); it++){
        int x = it->second.size();
        cout << it->first << "\t";
        cout << dayOfWeek(it->second[0]) << "\t";
        for(int i = 1; i < x; i++)
            cout << it->second[i] << "\t";
        cout << endl;
    }
    personalSchedule = assingShift(employees,shift,dayMap,daysInMonth);
}

intIntVectMap assingShift(employeeMap &employees, shiftMap &shift, intIntVectMap &dayMap, int daysInMonth){
    int employeeID, assignedDays;
    intIntVectMap personalSchedule;
    for(int i = 1; i <= daysInMonth; i++){
        for(shiftMap::iterator it = shift.begin(); it != shift.end(); it++){
            int shiftID = it->first;
            intVect employeesReady;
            while(dayMap[i][shiftID] > 0){
                for(employeeMap::iterator it = employees.begin(); it != employees.end(); it++){
                    employeeID = it->first;
                    assignedDays = personalSchedule[employeeID].size();
                    if(isWorkingDay(employees[employeeID],dayMap[i][0],i,shiftID) && assignedDays < i){
                        if(assignedDays == 0){
                            personalSchedule[employeeID].push_back(shiftID);
                            dayMap[i][shiftID]--;
                            break;
                        }
                        if(canSched(shift[shiftID],personalSchedule,employeeID,i)){
                            if(assignedDays == 1){
                                personalSchedule[employeeID].push_back(shiftID);
                                dayMap[i][shiftID]--;
                                break;
                            }
                            if(consecutiveDays(personalSchedule, employeeID) < shift[shiftID].maxConsecutive && weekMax(personalSchedule,dayMap,employeeID,i,shift)){
                                personalSchedule[employeeID].push_back(shiftID);
                                dayMap[i][shiftID]--;
                                break;
                            }
                        }
                    }
                }
            }
        }
        for(employeeMap::iterator it = employees.begin(); it != employees.end(); it++){
            employeeID = it->first;
            assignedDays = personalSchedule[employeeID].size();
            if(assignedDays < i)
                personalSchedule[employeeID].push_back(0);
        }
    }
}

int consecutiveDays(intIntVectMap &personalSchedule, int employeeID){
    int count = 0;
    for(int i = personalSchedule[employeeID].size() -1 ; i >= 0; i--){
        if(personalSchedule[employeeID][i] != 0)
            count++;
        if(personalSchedule[employeeID][i] == 0)
            return count;
    }
    return count;
}

bool weekMax(intIntVectMap &personalSchedule, intIntVectMap &dayMap, int employeeID, int dayNumber, shiftMap &shift){
    int aux = dayNumber, horas = 0;
    while(dayMap[aux][0] != 0 && aux > 0)
        aux--;
    if(aux == dayNumber)
        return true;
    for(int i = aux; i < dayNumber; i++)
        if(personalSchedule[employeeID][i-2]!=0)
            horas = horas + shift[personalSchedule[employeeID][i-2]].lenght;
    if(maxHoursPerWeek <= horas)
        return false;
    else
        return true;
}

bool canSched(struct Settings &shiftData, intIntVectMap &personalSchedule, int employeeID, int dayNumber){
    int vectLenght = shiftData.invalidNextShift.size();
    for(int i = 0; i < vectLenght; i++)
        if(personalSchedule[employeeID][dayNumber-2] == shiftData.invalidNextShift[i])
            return false;
    return true;
}

bool isWorkingDay(struct Employees &employeeData, int day, int dayNumber, int shiftID){
    int totalFree, totalHolidays, totalDisable;
    day--;
    if(day == -1)
        day=6;
    if(shiftID == 3 && employeeData.shiftType == 0)
        return false;
    if(employeeData.workingDays[day] && (employeeData.shiftType == shiftID || employeeData.shiftType == 0 )){
        totalFree = employeeData.freeDays.size();
        totalDisable = employeeData.disability.size();
        if(employeeData.holidays[0] != -1 || employeeData.holidays[1] != -1)
            if(employeeData.holidays[0] < dayNumber && employeeData.holidays[1] > dayNumber)
                return false;
        if(totalFree == 0 && totalDisable == 0)
            return true;
        for(int i = 0; i < totalFree; i++)
            if(employeeData.freeDays[i] == dayNumber)
                return false;
        for(int i = 0; i < totalDisable; i += 2)
            if(employeeData.disability[i] <= dayNumber && employeeData.disability[i+1] >= dayNumber)
                return false;
        return true;
    }
    return false;
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
            id = formatErrorHandler(input,splitVect[0]);                  //FuncionarioID
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
    if(shift.count(x) == 0)
        return 0;
    else
        return 1;
}

int idCheck(employeeMap &employee, int x) {
    if(employee.count(x) == 0)
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

int dayOfWeek(const string &weekDay) {
    if(!weekDay.compare("LUN")){
        return 1;
    }else if(!weekDay.compare("MAR")){
        return 2;
    }else if(!weekDay.compare("MIE")){
        return 3;
    }else if(!weekDay.compare("JUE")){
        return 4;
    }else if(!weekDay.compare("VIE")){
        return 5;
    }else if(!weekDay.compare("SAB")){
        return 6;
    }else if(!weekDay.compare("DOM")){
        return 0;
    }else{
        return 7;
    }
}

//Retorna el día de la semana como string.
string dayOfWeek(int weekDay) {
    if(weekDay == 1){
        return "LUN";
    }else if(weekDay == 2){
        return "MAR";
    }else if(weekDay == 3){
        return "MIE";
    }else if(weekDay == 4){
        return "JUE";
    }else if(weekDay == 5){
        return "VIE";
    }else if(weekDay == 6){
        return "SAB";
    }else if(weekDay == 0){
        return "DOM";
    }else{
        cout << "Error de formato. Terminando ejecucion..." << endl;
        exit(EXIT_FAILURE);
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
    tm time_in = {0, 0, 0, 1, month - 1, year - 1900};
    time_t time_temp = mktime(&time_in);
    tm *time_out = localtime(&time_temp);
    return time_out->tm_wday;
}