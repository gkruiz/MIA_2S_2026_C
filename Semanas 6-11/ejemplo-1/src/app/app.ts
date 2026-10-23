import { Component, signal ,ChangeDetectorRef } from '@angular/core';
import { CommonModule } from '@angular/common';
import { ApiService } from './services/api';
import Swal from 'sweetalert2';


@Component({
  selector: 'app-root',
  imports: [CommonModule],
  templateUrl: './app.html',
  styleUrl: './app.css'
})


export class App {
  protected readonly title = signal('ejemplo-1');

 
  logs: string[] = [];
  discos: any[] = [];
  info: string='';

  constructor(private cd: ChangeDetectorRef, private api: ApiService , ) {}

  ngOnInit(): void {
     
  }



  analizaComandos(cadena:string){

    var limpia = cadena.toLowerCase();
    limpia.trim()
    //limpia = limpia.replace(/ /g,"")
    var comandos = limpia.split(" -")


    //valida los comandos 

    if(comandos.length>0){
      //ingreso un comando
      this.comandos(comandos)

    }else{
      //no se ingreso ningun comando
      Swal.fire({
        title: 'Error',
        text: 'No se ingreso comandos!',
        icon: 'error',
        confirmButtonText: 'Aceptar'
      });
    }


  }


//mkdisk 
// -Size=3000 
// -unit=K 
// -path= Disco-1.mia 
// ASDFASDF



  comandos(instrucciones:any[]){
    this.info=''
    //mkdisk -Size=3000 -unit=K -path= Disco1.mia  dfasdf
    var comandop = instrucciones[0];
    comandop = comandop.replace(/ /g,"")

    console.log(instrucciones)
    switch(comandop){
      case 'mkdisk':
        this.mkdisk(instrucciones)
      break
      case 'fdisk':
        this.fdisk(instrucciones)
      break
      case 'rep':
        this.rep(instrucciones)
      break
      default:  
        Swal.fire({
          title: 'Error',
          text: 'El comando es erroneo!',
          icon: 'error',
          confirmButtonText: 'Aceptar'
        });
      break
    }

  }




  mkdisk(opciones:any[]){

    var instruciones = ['',"m",""]
    var error = false;

/*mkdisk 
-path= Disco1.mia
-Size=3000 
-unit=K 
*/


    for (let index = 1; index < opciones.length; index++) {
      var element = opciones[index];

      element = element.replace(/ /g,"")
      var temp = element.split('=');
      var opcion = temp[0]
      var instr = temp[1]

      console.log(element)
      
      switch(opcion){
        case 'size':
          instruciones[0]=instr
        break
        case 'unit':
          instruciones[1]=instr
        break
        case 'path':
          instruciones[2]=instr
        break
        default:  
          Swal.fire({
            title: 'Error',
            text: 'La instruccion es incorrecta',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
        break
      }
      
    }

    //Valida que vengan todos los parametros para poder generar la instruccion 
    if ((instruciones[0].toString()).localeCompare("")==0
    || (instruciones[2].toString()).localeCompare("")==0 ){
          Swal.fire({
            title: 'Error',
            text: 'Hacen falta instrucciones para el comando mkdisk!',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
    }

    console.log(instruciones)

    if (!error){
      this.api.createDisk(instruciones[0],instruciones[1],instruciones[2])
      .subscribe(resp=>{
        this.info = resp
        this.cd.detectChanges();
        console.log(resp);
      });
    }


  }





  fdisk(opciones:any[]){

    var instruciones = ['',"k","","p",""]
    var error = false;

    for (let index = 1; index < opciones.length; index++) {
      var element = opciones[index];

      element = element.replace(/ /g,"")
      var temp = element.split('=');
      var opcion = temp[0]
      var instr = temp[1]
      
      switch(opcion){
        case 'size':
          instruciones[0]=instr
        break
        case 'unit':
          instruciones[1]=instr
        break
        case 'path':
          instruciones[2]=instr
        break
        case 'type':
          instruciones[3]=instr
        break
        case 'name':
          instruciones[4]=instr
        break
        default:  
          Swal.fire({
            title: 'Error',
            text: 'La instruccion es incorrecta',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
        break
      }


    }



    //Valida que vengan todos los parametros para poder generar la instruccion 
    if ((instruciones[0].toString()).localeCompare("")==0  
      || (instruciones[2].toString()).localeCompare("")==0 
      || (instruciones[4].toString()).localeCompare("")==0 
    ){
          Swal.fire({
            title: 'Error',
            text: 'Hacen falta instrucciones para el comando fdisk!',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
    }


    
    if (!error){
      this.api.createPartition(instruciones[0],instruciones[1],instruciones[2],instruciones[3],instruciones[4])
      .subscribe(resp=>{
        this.info = resp
        this.cd.detectChanges();
        console.log(resp);
      });
    }
      

  }



  rep(opciones:any[]){

    var instruciones = ["",""]
    var error = false;

    for (let index = 1; index < opciones.length; index++) {
      var element = opciones[index];

      element = element.replace(/ /g,"")
      var temp = element.split('=');
      var opcion = temp[0]
      var instr = temp[1]
      
      switch(opcion){
        case 'path':
          instruciones[0]=instr
        break
        case 'name':
          instruciones[1]=instr
        break
        default:  
          Swal.fire({
            title: 'Error',
            text: 'La instruccion es incorrecta',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
        break
      }
      
    }

    console.log(instruciones)


    //Valida que vengan todos los parametros para poder generar la instruccion 
    if ((instruciones[0].toString()).localeCompare("")==0 
      || (instruciones[1].toString()).localeCompare("")==0 
    ){
          Swal.fire({
            title: 'Error',
            text: 'Hacen falta instrucciones para el comando rep!',
            icon: 'error',
            confirmButtonText: 'Aceptar'
          });
          error=true;
    }


    if (!error){
      this.api.getReporte(instruciones[0],instruciones[1])
      .subscribe(resp=>{
        this.info = resp.split("\n").join("\n");
        this.cd.detectChanges();
        console.log(resp);
      });
    }


  }













}



 