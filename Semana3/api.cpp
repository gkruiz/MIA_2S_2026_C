#include <iostream>
#include <fstream>
#include <vector>
#include <cstring>
#include "httplib.h"

using namespace std;

struct Partition {
    char name[16];
    int size;
    char type; // p o e
};

struct MBR {
    int disk_size;
    int partition_count;
    Partition partitions[4];
};



string mkdisk(int size, char unit, string path) {

    int bytes = size;

    if (unit == 'm' || unit == 'M') bytes *= 1024 * 1024;
    if (unit == 'k' || unit == 'K') bytes *= 1024;

    ofstream file(path, ios::binary);

    if (!file.is_open()) {
        return "Error creando disco";
    }

    MBR mbr;
    mbr.disk_size = bytes;
    mbr.partition_count = 0;
    memset(mbr.partitions, 0, sizeof(mbr.partitions));

    file.write((char*)&mbr, sizeof(MBR));

    file.seekp(bytes - 1);
    file.write("", 1);

    file.close();

    return "Disco creado correctamente";
}

string fdisk(int size, char unit, string path, char type, string name) {

    fstream file(path, ios::in | ios::out | ios::binary);

    if (!file.is_open()) {
        return "No se pudo abrir el disco";
    }

    MBR mbr;

    file.read((char*)&mbr, sizeof(MBR));

    if (mbr.partition_count >= 4) {
        return "Limite de particiones alcanzado";
    }

    int bytes = size;

    if (unit == 'm' || unit == 'M') bytes *= 1024 * 1024;
    if (unit == 'k' || unit == 'K') bytes *= 1024;

    Partition p;

    memset(&p, 0, sizeof(Partition));

    strncpy(p.name, name.c_str(), 15);
    p.size = bytes;
    p.type = type;

    mbr.partitions[mbr.partition_count] = p;
    mbr.partition_count++;

    file.seekp(0);
    file.write((char*)&mbr, sizeof(MBR));

    file.close();

    return "Particion creada correctamente";
}

string rep(string path, string name) {

    ifstream file(path, ios::binary);

    if (!file.is_open()) {
        return "No se pudo abrir el disco";
    }

    MBR mbr;

    file.read((char*)&mbr, sizeof(MBR));

    string result = "";

    if (name == "*") {

        result += "Tamano disco: " + to_string(mbr.disk_size) + " bytes\n";
        result += "Numero particiones: " + to_string(mbr.partition_count) + "\n\n";

        for (int i = 0; i < mbr.partition_count; i++) {
            result += "Particion: ";
            result += mbr.partitions[i].name;
            result += " | size: ";
            result += to_string(mbr.partitions[i].size);
            result += " | type: ";
            result += mbr.partitions[i].type;
            result += "\n";
        }

    } else {

        for (int i = 0; i < mbr.partition_count; i++) {

            if (name == mbr.partitions[i].name) {

                result += "Particion encontrada\n";
                result += "Nombre: ";
                result += mbr.partitions[i].name;
                result += "\nTamano: ";
                result += to_string(mbr.partitions[i].size);
                result += "\nTipo: ";
                result += mbr.partitions[i].type;

                file.close();
                return result;
            }
        }

        result = "Particion no encontrada";
    }

    file.close();

    return result;
}

int main() {

    httplib::Server svr;

    svr.set_pre_routing_handler([](const httplib::Request& req, httplib::Response& res) {

        res.set_header("Access-Control-Allow-Origin", "*");
        res.set_header("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
        res.set_header("Access-Control-Allow-Headers", "Content-Type");

        if (req.method == "OPTIONS") {
            res.status = 200;
            return httplib::Server::HandlerResponse::Handled;
        }

        return httplib::Server::HandlerResponse::Unhandled;
    });

    // CREAR DISCO
    svr.Get("/mkdisk", [](const httplib::Request& req, httplib::Response& res) {

        int size = stoi(req.get_param_value("size"));
        char unit = req.get_param_value("unit")[0];
        string path = req.get_param_value("path");

        res.set_content(mkdisk(size, unit, path), "text/plain");
    });

    // CREAR PARTICION
    svr.Get("/fdisk", [](const httplib::Request& req, httplib::Response& res) {

        int size = stoi(req.get_param_value("size"));
        char unit = req.get_param_value("unit")[0];
        string path = req.get_param_value("path");
        char type = req.get_param_value("type")[0];
        string name = req.get_param_value("name");

        res.set_content(fdisk(size, unit, path, type, name), "text/plain");
    });

    // REPORTE
    svr.Get("/rep", [](const httplib::Request& req, httplib::Response& res) {

        string path = req.get_param_value("path");
        string name = req.get_param_value("name");

        res.set_content(rep(path, name), "text/plain");
    });

    cout << "Servidor corriendo en http://localhost:8080" << endl;

    svr.listen("0.0.0.0", 8090);
}
