#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "httplib.h"

using namespace std;

/*------------- ESTRUCTURAS -------------*/

struct Partition {
    char name[16];
    int start;
    int size;
    char type;
};

struct MBR {
    int size;
    Partition partitions[4];
};

/*------------- FUNCIONES -------------*/

int convertSize(int size, char unit) {
    if (unit == 'm' || unit == 'M')
        return size * 1024 * 1024;
    if (unit == 'k' || unit == 'K')
        return size * 1024;
    return size;
}

/*------------- MKDISK -------------*/

string mkdisk(int size, char unit, string path) {

    int bytes = convertSize(size, unit);

    ofstream file(path, ios::binary);
    if (!file) return "Error creando disco";

    file.seekp(bytes - 1);
    file.write("", 1);

    MBR mbr;
    mbr.size = bytes;

    for (int i = 0; i < 4; i++) {
        strcpy(mbr.partitions[i].name, "");
        mbr.partitions[i].start = -1;
        mbr.partitions[i].size = 0;
        mbr.partitions[i].type = '0';
    }

    file.seekp(0);
    file.write((char*)&mbr, sizeof(MBR));
    file.close();

    return "Disco creado";
}

/*------------- FDISK -------------*/

string fdisk(int size, char unit, string path, char type, string name) {

    fstream file(path, ios::in | ios::out | ios::binary);
    if (!file) return "Disco no encontrado";

    int bytes = convertSize(size, unit);

    MBR mbr;
    file.read((char*)&mbr, sizeof(MBR));

    for (int i = 0; i < 4; i++) {

        if (mbr.partitions[i].start == -1) {

            int start = sizeof(MBR);

            if (i > 0)
                start = mbr.partitions[i-1].start + mbr.partitions[i-1].size;

            mbr.partitions[i].start = start;
            mbr.partitions[i].size = bytes;
            mbr.partitions[i].type = type;

            strncpy(mbr.partitions[i].name, name.c_str(), 15);

            file.seekp(0);
            file.write((char*)&mbr, sizeof(MBR));

            file.close();
            return "Particion creada";
        }
    }

    file.close();
    return "No hay espacio para mas particiones";
}

/*------------- REPORTE -------------*/

string rep(string path, string name) {

    ifstream file(path, ios::binary);
    if (!file) return "Disco no encontrado";

    MBR mbr;
    file.read((char*)&mbr, sizeof(MBR));

    string result = "";

    if (name == "*") {

        result += "Tamaño disco: " + to_string(mbr.size) + "\n";

        for (int i = 0; i < 4; i++) {

            if (mbr.partitions[i].start != -1) {

                result += "Particion: ";
                result += mbr.partitions[i].name;
                result += " Size: " + to_string(mbr.partitions[i].size);
                result += "\n";
            }
        }

    } else {

        for (int i = 0; i < 4; i++) {

            if (strcmp(mbr.partitions[i].name, name.c_str()) == 0) {

                result += "Particion encontrada\n";
                result += "Nombre: ";
                result += mbr.partitions[i].name;
                result += "\nSize: " + to_string(mbr.partitions[i].size);
                result += "\nStart: " + to_string(mbr.partitions[i].start);
            }
        }
    }

    file.close();
    return result;
}

/*------------- API REST -------------*/

int main() {

    httplib::Server svr;

    /* MKDISK */
    svr.Post("/mkdisk", [](const httplib::Request& req, httplib::Response& res) {

        int size = stoi(req.get_param_value("size"));
        char unit = req.get_param_value("unit")[0];
        string path = req.get_param_value("path");

        res.set_content(mkdisk(size, unit, path), "text/plain");
    });

    /* FDISK */
    svr.Post("/fdisk", [](const httplib::Request& req, httplib::Response& res) {

        int size = stoi(req.get_param_value("size"));
        char unit = req.get_param_value("unit")[0];
        string path = req.get_param_value("path");
        char type = req.get_param_value("type")[0];
        string name = req.get_param_value("name");

        res.set_content(fdisk(size, unit, path, type, name), "text/plain");
    });

    /* REP */
    svr.Get("/rep", [](const httplib::Request& req, httplib::Response& res) {

        string path = req.get_param_value("path");
        string name = req.get_param_value("name");

        res.set_content(rep(path, name), "text/plain");
    });

    cout << "Servidor corriendo en puerto 8080\n";

    svr.listen("0.0.0.0", 8080);
}
