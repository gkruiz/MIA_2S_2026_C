import { Injectable } from '@angular/core';
import { HttpClient } from '@angular/common/http';

@Injectable({
  providedIn: 'root',
})
export class ApiService {

  constructor(private http: HttpClient) {}


  getReporte(path:string , name:string) {
    return this.http.get(
      'http://localhost:8080/rep',
      {
        params: { path: path, name: name },
        responseType: 'text'
      }
    );
  }

  createPartition(size:string , unit:string, path:string, type:string, name:string) {
      //http://localhost:8080/fdisk?size=2&unit=m&path=miDisco.dk&type=p&name=part1

      return this.http.get(
      'http://localhost:8080/fdisk',
      {
        params: { size: size, unit: unit, path: path, type: type, name: name },
        responseType: 'text'
      }
    );
  }

  createDisk(size:string , unit:string, path:string) {
      //http://localhost:8080/mkdisk?size=10&unit=m&path=disco.dk

      return this.http.get(
      'http://localhost:8080/mkdisk',
      {
        params: { size: size, unit: unit, path: path},
        responseType: 'text'
      }
    );
  }




}
