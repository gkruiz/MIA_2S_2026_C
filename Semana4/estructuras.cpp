#include <iostream>
#include <fstream>
#include <cstring>

using namespace std;

const int MAX_PARTICIONES = 4;
const char ARCHIVO[] = "disco_ejemplo.bin";



struct Particion {
    int tipo;               // 0 = libre, 1 = Primaria, 2 = Extendida
    char tipoNombre[10];
    char nombre[20];
    int tamanoDiscoKB;
    int tamanoParticionKB;
    int numLogicas;
    bool usada;
};



void inicializarArchivo() {
    fstream file(ARCHIVO, ios::binary | ios::in);
    if (file.good()) {
        file.close();
        return;
    }
    file.close();

    fstream nuevo(ARCHIVO, ios::binary | ios::out);
    nuevo.close();
}



bool discoExiste(int &tamanoDisco) {
    fstream file(ARCHIVO, ios::binary | ios::in);
    Particion p;

    file.read(reinterpret_cast<char*>(&p), sizeof(Particion));
    file.close();

    if (p.tamanoDiscoKB > 0) {
        tamanoDisco = p.tamanoDiscoKB;
        return true;
    }
    return false;
}


void crearDisco() {
    int tamanoDiscoKB;
    int dummy;

    /*if (discoExiste(dummy)) {
        cout << "El disco ya fue creado.\n";
        return;
    }*/

    cout << "Ingrese el tamano del disco (KB): ";
    cin >> tamanoDiscoKB;

    fstream file(ARCHIVO, ios::binary | ios::out);

    char cero = 0;
    for (int i = 0; i < tamanoDiscoKB * 1024; i++) {
        file.write(&cero, 1);
    }
    file.close();

    fstream file2(ARCHIVO, ios::binary | ios::in | ios::out);
    Particion p = {};
    p.tipo = 0;
    p.usada = false;
    p.tamanoDiscoKB = tamanoDiscoKB;

    for (int i = 0; i < MAX_PARTICIONES; i++) {
        file2.seekp(i * sizeof(Particion));
        file2.write(reinterpret_cast<char*>(&p), sizeof(Particion));
    }
    file2.close();

    cout << "Disco creado correctamente.\n";
}

//| 1| 2| 3| 4|LIBRE               |1MB

void contarParticiones(int &primarias, int &extendidas, int &espacioUsado) {
    fstream file(ARCHIVO, ios::binary | ios::in);
    Particion p;

    primarias = extendidas = espacioUsado = 0;

    while (file.read(reinterpret_cast<char*>(&p), sizeof(Particion))) {
        if (p.usada) {
            espacioUsado += p.tamanoParticionKB;
            if (p.tipo == 1) primarias++;
            if (p.tipo == 2) extendidas++;
        }
    }
    file.close();
}



void crearParticion() {
    int tamanoDisco;
    if (!discoExiste(tamanoDisco)) {
        cout << "Debe crear el disco primero.\n";
        return;
    }

    Particion nueva = {};
    int primarias, extendidas, espacioUsado;

    contarParticiones(primarias, extendidas, espacioUsado);

    cout << "Tipo de particion (1=Primaria, 2=Extendida): ";
    cin >> nueva.tipo;

    if (nueva.tipo == 1 && primarias >= 1) {
        cout << "Solo se permite una particion primaria.\n";
        return;
    }

    if (nueva.tipo == 2 && extendidas >= 3) {
        cout << "Maximo 3 particiones extendidas.\n";
        return;
    }

    cout << "Nombre de la particion: ";
    cin >> nueva.nombre;

    cout << "Tamano de la particion (KB): ";
    cin >> nueva.tamanoParticionKB;

    if (espacioUsado + nueva.tamanoParticionKB > tamanoDisco) {
        cout << "No hay espacio suficiente en el disco.\n";
        return;
    }

    nueva.tamanoDiscoKB = tamanoDisco;

    if (nueva.tipo == 1) {
        strcpy(nueva.tipoNombre, "Primaria");
        nueva.numLogicas = 0;
    } else {
        strcpy(nueva.tipoNombre, "Extendida");
        cout << "Numero de particiones logicas: ";
        cin >> nueva.numLogicas;
    }

    nueva.usada = true;

    fstream file(ARCHIVO, ios::binary | ios::in | ios::out);
    Particion temp;

    for (int i = 0; i < MAX_PARTICIONES; i++) {
        file.seekg(i * sizeof(Particion));
        file.read(reinterpret_cast<char*>(&temp), sizeof(Particion));

        if (!temp.usada) {
            file.seekp(i * sizeof(Particion));
            file.write(reinterpret_cast<char*>(&nueva), sizeof(Particion));
            cout << "Particion creada correctamente.\n";
            file.close();
            return;
        }
    }

    cout << "No hay espacios disponibles.\n";
    file.close();
}



void mostrarParticion() {
    int num;
    cout << "Ingrese numero de particion (1-4): ";
    cin >> num;

    if (num < 1 || num > 4) {
        cout << "Numero invalido.\n";
        return;
    }

    fstream file(ARCHIVO, ios::binary | ios::in);
    Particion p;


    file.seekg((num - 1) * sizeof(Particion));
    file.read(reinterpret_cast<char*>(&p), sizeof(Particion));
    file.close();

    if (!p.usada) {
        cout << "Particion vacia.\n";
        return;
    }

    cout << "\nTipo: " << p.tipoNombre << endl;
    cout << "Nombre: " << p.nombre << endl;
    cout << "Tamano del disco (KB): " << p.tamanoDiscoKB << endl;
    cout << "Tamano de la particion (KB): " << p.tamanoParticionKB << endl;

    if (p.tipo == 2) {
        cout << "Particiones logicas: " << p.numLogicas << endl;
    }
}




int main() {
    inicializarArchivo();
    int opcion;

    do {
        cout << "\n===== MENU =====\n";
        cout << "1. Crear disco\n";
        cout << "2. Crear particion\n";
        cout << "3. Ver particion\n";
        cout << "4. Salir\n";
        cout << "Opcion: ";
        cin >> opcion;

        switch (opcion) {
            case 1:
                crearDisco();
                break;
            case 2:
                crearParticion();
                break;
            case 3:
                mostrarParticion();
                break;
            case 4:
                cout << "Saliendo...\n";
                break;
            default:
                cout << "Opcion invalida.\n";
        }
    } while (opcion != 4);

    return 0;
}
