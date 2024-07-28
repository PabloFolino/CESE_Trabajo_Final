
// **************************************************************************************** -->
// ******************************* Java Script ******************************************** -->
// **************************************************************************************** --> */}
    var red = "rgba(255, 4, 10, 1)";
    var grey = "rgba(230, 230, 230, 1)";
    var naranja="rgba(255, 128, 0)";
    var verde="rgba( 127, 255, 0, 1 )";
    //document.getElementById('valor_us_cm').innerHTML="000";
    var buffer;


//****************************************************************************************
//***************************  manual.html ***********************************************
//****************************************************************************************

function setearPwm0(valor){
    console.log("Seteo PWM_0");  
    consultaGET("PWM_0?PWM_0="+valor);
}

function setearPwm1(valor){
    console.log("Seteo PWM_1");
    consultaGET("PWM_1?PWM_1="+valor);
}

function setearStop(){
    pwm1.value=pwm0.value=amount1.value=amount0.value=256;
    console.log("STOP");
    consultaGET("STOP");
}

function compara( data1, data2, numero){
    var flag_c = true;
    var i=0;
    for (i = 0; i < numero; i++){
    if (data1[i] != data2[i]){
        flag_c = false;
        break;
        }           
    }
    return flag_c;
}




//****************************************************************************************
//***************************  sensores.html *********************************************
//****************************************************************************************

    function laser_der_on(){
        console.log("Laser_der_on");  
        consultaGET("LASER_DER=ON");
    }
    
    function laser_der_off(){
        console.log("Laser_der_off");  
        consultaGET("LASER_DER=OFF");
    }

    function laser_izq_on(){
        console.log("Laser_izq_on");  
        consultaGET("LASER_IZQ=ON");
    }
    
    function laser_izq_off(){
        console.log("Laser_izq_off");  
        consultaGET("LASER_IZQ=OFF");
    }

    function disparo_ultrasonido(){
        console.log("Pulso de disparo");  
        consultaGET("DISP_ULTRAS");
    }

    function test_bumpers(){
        console.log("Test Bumpers");  
        consultaGET("TEST_BUMPERS");
    }

    function reset_bumpers(){
        console.log("Reset Bumpers");  
        bumpers_front.style.backgroundColor= grey; 
        bumpers_back.style.backgroundColor= grey;  
    }

    function test_gps(){
        console.log("Test del GPS");  
        consultaGET("TEST_GPS");
    }

    function test_mpu9250(){
        console.log("Test del mpu9250");  
        consultaGET("TEST_MPU9250");
    }

    function reset_mpu9250(){
        console.log("Reset del mpu9250");  
        document.getElementById('valor_gyro_x').innerHTML="000000";
        document.getElementById('valor_gyro_y').innerHTML="000000";
        document.getElementById('valor_gyro_z').innerHTML="000000";
        document.getElementById('valor_mag_x').innerHTML="000000";
        document.getElementById('valor_mag_y').innerHTML="000000";
        document.getElementById('valor_mag_z').innerHTML="000000";
        document.getElementById('valor_acc_x').innerHTML="000000";
        document.getElementById('valor_acc_y').innerHTML="000000";
        document.getElementById('valor_acc_z').innerHTML="000000";
        document.getElementById('valor_temp').innerHTML="000000";
    }

    function valor_mpu(buffer_data,buffer, inicial, final){
        console.log("Dato de entrada"+buffer_data);
        console.log("Inicial="+inicial);
        console.log("final="+final);
        for(var c=inicial;c<=final; c++){
            console.log("buffer["+c+"]="+buffer[c]);
            buffer[c-inicial]=buffer_data[c];
        }
        c++;
        buffer[c]="\0";
        console.log("Dato de salida="+buffer);
    }


//****************************************************************************************
//***************************  pid.html **************************************************
//****************************************************************************************
 
 
  //************************  Leo Archivo ************************************************
  // Info --> https://usefulangle.com/post/193/javascript-read-local-file
  //      --> https://betterprogramming.pub/handling-file-inputs-with-javascript-9f2d3a007f05

  const fileInput = document.getElementById('file-input');
  const buttonInput = document('pid.html').getElementById('button-input');


  buttonInput.onclick = () => {
  fileInput.click();
  }
  fileInput.onchange = () => {
  const selectedFiles = [...fileInput.files];
  console.log(selectedFiles);
  }

  document.querySelector("#read-button").addEventListener('click', function() {
		
    // Si la longitud es cero no se puede abrir
    if(document.querySelector("#file-input").files.length == 0) {
	  	alert('Error : No file selected');
		  return;
	  } 
    // Archivo selecionado por el usuario
    let file = document.querySelector("#file-input").files[0];
		// Nombre del archivo
    let file_name = file.name;
    document.getElementById('valor_file_name').value=file_name;
    console.log("Archivo="+file_name);
    // Tipo de Archivo
    let file_type = file.type;
    document.getElementById('valor_file_type').value=file_type;
    console.log("Tipo de archivo="+file_type);
    // Longitud de Archivo
    let file_size = file.size;
    document.getElementById('valor_file_size').value=file_size;
    console.log("Tamaño de archivo="+file_size);

    // Leo el archivo
    let reader = new FileReader();
    // Cuando finaliza la tectura del archivo
		reader.addEventListener('load', function(e) {
      // Recupero el cotenido del archivo
  		let text = e.target.result;
      let data = JSON.parse(text);
      console.log('string', text);
      console.log('json', data);

      // Muestro los valores en la página WEB
      document.getElementById("valor_kp1").value=data.kp_1;
      document.getElementById("valor_kp2").value=data.kp_2;
      document.getElementById("valor_ki1").value=data.ki_1;
      document.getElementById("valor_N1").value=data.N_1;
      document.getElementById("valor_ki2").value=data.ki_2;
      document.getElementById("valor_kd1").value=data.kd_1;
      document.getElementById("valor_kd2").value=data.kd_2;
      document.getElementById("valor_N2").value=data.N_2;
		});

   	// Cuando falla la lectra del archivo
	  reader.addEventListener('error', function() {
	    alert('Error : Failed to read file');
	  });

		reader.readAsText(file);
	})
  //****************************************************************************************

  //******************************  Escribo archivo ****************************************
  function file_write(valor){
    var mensaje={};
    mensaje.kp_1=document.getElementById("valor_kp1").value;
    mensaje.ki_1=document.getElementById("valor_ki1").value;
    mensaje.kd_1=document.getElementById("valor_kd1").value;
    mensaje.N_1=document.getElementById("valor_N1").value;
    mensaje.kp_2=document.getElementById("valor_kp2").value;
    mensaje.ki_2=document.getElementById("valor_ki2").value;
    mensaje.kd_2=document.getElementById("valor_kd2").value;
    mensaje.N_2=document.getElementById("valor_N2").value;
    download('pid.json', JSON.stringify(mensaje));
  }

  function download(filename, text) {
  var pom = document.createElement('a');
  pom.setAttribute('href', 'data:text/plain;charset=utf-8,' + encodeURIComponent(text));
  pom.setAttribute('download', filename);

  if (document.createEvent) {
    var event = document.createEvent('MouseEvents');
    event.initEvent('click', true, true);
    pom.dispatchEvent(event);
    }
    else {
      pom.click();
    }
  }
  //****************************************************************************************

  function get_pid(){
    console.log("Get PID");  
    consultaGET("GET_PID");
  }

  function check_pid(){
    console.log("Chequeo de PID");  
    consultaGET("CHECK_PID");
  }

  function send_pid(){
    console.log("Send PID");  
    mensaje="kp_1="+document.getElementById("valor_kp1").value+"&"+
            "ki_1="+document.getElementById("valor_ki1").value+"&"+
            "kd_1="+document.getElementById("valor_kd1").value+"&"+
            "N_1="+document.getElementById("valor_N1").value+"&"+
            "kp_2="+document.getElementById("valor_kp2").value+"&"+
            "ki_2="+document.getElementById("valor_ki2").value+"&"+
            "kd_2="+document.getElementById("valor_kd2").value+"&"+
            "N_2="+document.getElementById("valor_N2").value;        
    consultaGET("SEND_PID?"+mensaje);
  }




//****************************************************************************************
//***************************  Hace la consulta ******************************************
//****************************************************************************************


  function consultaGET(consulta){
            const Http = new XMLHttpRequest();
            console.log(`Consultando  ${consulta}`)
            Http.open("GET", consulta);
            Http.send();
            // Se debe enviar 2 veces es por cómo está hecho
            // el servidor WEB del ESP01 200ms
            setTimeout(() => {
                console.log("Esperado 200mseg");
                Http.open("GET", consulta);
                Http.send();
                }, 200);


            Http.onreadystatechange = (e) => {
            console.log(Http.status );
            //console.log(Http.statusText);
            console.log(Http.getResponseHeader('content-length'));
            //console.log(Http.getAllResponseHeaders());
            //console.log(Http.responseType);
            console.log("Dato_RX="+Http.response);

//****************************************************************************************
//***************************  manual.html ***********************************************
//****************************************************************************************

            if(consulta=="STOP"){
                document.getElementById('pwm_0').innerHTML="000";
                document.getElementById('pwm_1').innerHTML="000";
            }

            if(compara(consulta, "PWM_0", 5)){
                document.getElementById('pwm_0').innerHTML=Http.response;
            }

            if(compara(consulta, "PWM_1", 5)){
                document.getElementById('pwm_1').innerHTML=Http.response;
            }


//****************************************************************************************
//***************************  sensores.html *********************************************
//****************************************************************************************

            if(consulta=="TEST_BUMPERS"){
                data=Http.response;
                switch(data){
                    case "0":
                        bumpers_front.style.backgroundColor= grey; 
                        bumpers_back.style.backgroundColor= grey; 
                        break;
                    case "1":
                        bumpers_front.style.backgroundColor= red; 
                        bumpers_back.style.backgroundColor= grey; 
                        break;
                    case "2":
                        bumpers_front.style.backgroundColor= grey; 
                        bumpers_back.style.backgroundColor= red; 
                        break;
                    case "3":
                        bumpers_front.style.backgroundColor= red; 
                        bumpers_back.style.backgroundColor= red; 
                        break;
                    default:
                        bumpers_front.style.backgroundColor= naranja; 
                        bumpers_back.style.backgroundColor= naranja; 
                        break;
                }
            }
            if(consulta=="LASER_IZQ=OFF"||consulta=="LASER_IZQ=ON" ){
                data=Http.response;
                switch(data){
                    case "ON":
                        laser_izq.style.backgroundColor=red;
                        break;
                    case "OFF":
                        laser_izq.style.backgroundColor=grey;
                        break;   
                    default:
                        laser_izq.style.backgroundColor= naranja; 
                        break;                 
                    }
                }   
            if(consulta=="LASER_DER=OFF"||consulta=="LASER_DER=ON" ){
                data=Http.response;
                switch(data){
                    case "ON":
                        laser_der.style.backgroundColor=red;
                        break;
                    case "OFF":
                        laser_der.style.backgroundColor=grey;
                        break;   
                    default:
                        laser_der.style.backgroundColor= naranja; 
                        break;                 
                    }
                }     
            if(consulta=="DISP_ULTRAS"){
                if(Http.getResponseHeader('content-length')=="0"){
                    document.getElementById('valor_us_cm').innerHTML="Sin Rx";
                    } 
                else{
                    document.getElementById('valor_us_cm').innerHTML=Http.response;
                    }
            }
            if(consulta=="TEST_GPS"){
                document.valor_gps.value=Http.response;
                }
            if(consulta=="TEST_MPU9250"){
                console.log("TEST_MPU9250");
                if(Http.getResponseHeader('content-length')=="0"){
                    console.log("True");
                    document.getElementById('valor_gyro_x').innerHTML="ERROR";
                    document.getElementById('valor_gyro_y').innerHTML="ERROR";
                    document.getElementById('valor_gyro_z').innerHTML="ERROR";
                    document.getElementById('valor_mag_x').innerHTML="ERROR";
                    document.getElementById('valor_mag_y').innerHTML="ERROR";
                    document.getElementById('valor_mag_z').innerHTML="ERROR";
                    document.getElementById('valor_acc_x').innerHTML="ERROR";
                    document.getElementById('valor_acc_y').innerHTML="ERROR";
                    document.getElementById('valor_acc_z').innerHTML="ERROR";
                    document.getElementById('valor_temp').innerHTML="ERROR";
                } 
                else{
                    console.log("Else");
                    //***********************************************
		            // El formato que devuelve es:
		            //    TEST_MPU9250=XXXXXX_YYYYYY_ZZZZZZ=
		            //				   XXXXXX_YYYYYY_ZZZZZZ=
		            //		           XXXXXX_YYYYYY_ZZZZZZ=TTTTTT
		            // El primer grupo de XYZ es para el giróscopo,
		            // el segundo grupo para el acelerómetro,
		            // el tercero para el magnetómetro, y las T
		            // es para la temperatura.);
                    document.getElementById('valor_gyro_x').innerHTML=Http.response.slice(0,6);
                    document.getElementById('valor_gyro_y').innerHTML=Http.response.slice(7,13);
                    document.getElementById('valor_gyro_z').innerHTML=Http.response.slice(14,20);
                    document.getElementById('valor_mag_x').innerHTML=Http.response.slice(21,27);
                    document.getElementById('valor_mag_y').innerHTML=Http.response.slice(28,34);
                    document.getElementById('valor_mag_z').innerHTML=Http.response.slice(35,41);
                    document.getElementById('valor_acc_x').innerHTML=Http.response.slice(42,48);
                    document.getElementById('valor_acc_y').innerHTML=Http.response.slice(49,55);
                    document.getElementById('valor_acc_z').innerHTML=Http.response.slice(58,62);
                    document.getElementById('valor_temp').innerHTML=Http.response.slice(63,68);
                }
            }



//****************************************************************************************
//***************************  pid.html **************************************************
//****************************************************************************************
            if(consulta=="SEND_PID"){e;
                }
            
            if(consulta=="GET_PID"){
                document.getElementById("valor_kp1").value=Http.response.slice(0,6);;
                document.getElementById("valor_ki1").value=Http.response.slice(0,6);;
                document.getElementById("valor_kd1").value=Http.response.slice(0,6);;
                document.getElementById("valor_N1").value=Http.response.slice(0,6);;
                document.getElementById("valor_kp2").value=Http.response.slice(0,6);;
                document.getElementById("valor_ki2").value=Http.response.slice(0,6);;
                document.getElementById("valor_kd2").value=Http.response.slice(0,6);;
                document.getElementById("valor_N2").value=Http.response.slice(0,6);;
                }

            if(consulta=="CHECK_PID"){
                data=Http.response;
                check_pid.style.backgroundColor= verde;
            }

            if(consulta=="TEST_MPU9250"){
                console.log("TEST_MPU9250");
                if(Http.getResponseHeader('content-length')=="0"){
                    console.log("True");
                    document.getElementById('valor_gyro_x').innerHTML="ERROR";
                    document.getElementById('valor_gyro_y').innerHTML="ERROR";
                    document.getElementById('valor_gyro_z').innerHTML="ERROR";
                    document.getElementById('valor_mag_x').innerHTML="ERROR";
                    document.getElementById('valor_mag_y').innerHTML="ERROR";
                    document.getElementById('valor_mag_z').innerHTML="ERROR";
                    document.getElementById('valor_acc_x').innerHTML="ERROR";
                    document.getElementById('valor_acc_y').innerHTML="ERROR";
                    document.getElementById('valor_acc_z').innerHTML="ERROR";
                    document.getElementById('valor_temp').innerHTML="ERROR";
                } 
                else{
                    console.log("Else");
                    //***********************************************
		              // El formato que devuelve es:
		              //    TEST_MPU9250=XXXXXX_YYYYYY_ZZZZZZ=
		              //				   XXXXXX_YYYYYY_ZZZZZZ=
		              //		           XXXXXX_YYYYYY_ZZZZZZ=TTTTTT
		              // El primer grupo de XYZ es para el giróscopo,
		              // el segundo grupo para el acelerómetro,
		              // el tercero para el magnetómetro, y las T
		              // es para la temperatura.);
                    document.getElementById('valor_gyro_x').innerHTML=Http.response.slice(0,6);
                    document.getElementById('valor_gyro_y').innerHTML=Http.response.slice(7,13);
                    document.getElementById('valor_gyro_z').innerHTML=Http.response.slice(14,20);
                    document.getElementById('valor_mag_x').innerHTML=Http.response.slice(21,27);
                    document.getElementById('valor_mag_y').innerHTML=Http.response.slice(28,34);
                    document.getElementById('valor_mag_z').innerHTML=Http.response.slice(35,41);
                    document.getElementById('valor_acc_x').innerHTML=Http.response.slice(42,48);
                    document.getElementById('valor_acc_y').innerHTML=Http.response.slice(49,55);
                    document.getElementById('valor_acc_z').innerHTML=Http.response.slice(58,62);
                    document.getElementById('valor_temp').innerHTML=Http.response.slice(63,68);
                }
            }
        }
        }