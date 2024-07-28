/*
Autor: Pablo Daniel Folino  2022
Versión: 4
Nota:
  1) Compilar el proyecto
  2) Setear al ESP01 en modo programación(jumper)
  3) Luego hacer upload
  4) Setear nuevamente al ESP01 en modo programación(jumper)
  5) Compilar de nuevo o hacer desde "PlatformIO Core CLI" --> pio run -t upload
*/

#include <ESP8266WiFi.h> //Incluimos librerías
#include <ESPAsyncTCP.h>
#include <ESPAsyncWebServer.h>
#include <FS.h>
#include <ESP8266mDNS.h>
#include <string.h>
#include <time.h>

// Datos de nuestro WiFi
const char *ssid = "Robot_RL";
const char *password = "12345678";

// Configurar una dirección IP Estática
IPAddress ip(192, 168, 2, 1);
IPAddress gateway(192, 168, 2, 1);
IPAddress subnet(255, 255, 255, 0);

// Pin donde estará el led indicador
const int Rele_Pin = 2;

#define DELAY_TEST 1500 // tiempo que espera la contestación de un comando en forma manual
#define DELAY 5         // tiempo que espera la contestación de un comando (en milisegundos) Valor original 150
#define LONG_BUFFER_RX 100
#define LONG_BUFFER_SHORT 20
char data_rx[LONG_BUFFER_RX];
uint8_t flag_rx = false;  // Se usa para saber si tengo datos del EDU CIAA


time_t tiempo_viejo, tiempo_actual, delta_time;

char laser_der_on[] = "LASER_DER=ON";
char laser_der_off[] = "LASER_DER=OFF";
char laser_izq_on[] = "LASER_IZQ=ON";
char laser_izq_off[] = "LASER_IZQ=OFF";
char pwm_0[] = "PWM_0";
char pwm_1[] = "PWM_1";
char disp_ultras[]= "DISP_ULTRAS";
char test_bumpers[]="TEST_BUMPERS";
char test_gps[]="TEST_GPS";
char test_mpu9250[]="TEST_MPU9250";
char send_pid[]="SEND_PID";
char get_pid[]="GET_PID";
char check_pid[]="CHECK_PID";
char leer_prueba[]="LEER_PRUEBA";

// Variable String para mandar el estado del rele a nuestra página web
String Estado_Rele;
// Variables recibidas de la CIAA
String pwm_rx;
String disp_ultras_rx;
String test_bumpers_rx;
String test_gps_rx;
char test_mpu9250_rx[LONG_BUFFER_RX];
String send_pid_rx;
String get_pid_rx;
String check_pid_rx;
char leer_prueba_rx[LONG_BUFFER_RX];

// Variables de la página WEB
String LASER_IZQ, LASER_DER;
String PWM_0, PWM_1;
String DISP_ULTRAS;
String ESTADO_BUMPERS;
String TEST_GPS;
String VALOR_US_CM;
String TEST_MPU9250;
String SEND_PID;
String GET_PID;
String CHECK_PID;
String PROBAR_PWM;
String LEER_PRUEBA;

struct _estado
{
  uint8_t laser_izq = false;
  uint8_t laser_der = false;

} estado;

// Delay no bloqueante
void delay_ms(uint32_t retardo)
{
  uint32_t tiempo_cpu;
  tiempo_cpu = millis();
  // Serial.printf(" Tiempo_inic=%ld\n",tiempo_cpu);
  while (millis() < tiempo_cpu + retardo)
  {
    // espere [periodo] milisegundos
  }
  tiempo_cpu = millis();
  // Serial.printf(" Tiempo_final=%ld\n",tiempo_cpu);
}

// vPuerto TCP en donde se encuentra la WEB
AsyncWebServer server(80);

// Función Processor para regresar valores a página Web

String processor_dummy(const String &var)
{
  if (var == "ESTADO_RELE")
  {
    return "Vacio";
  }

  return "ERROR-DUMMY";
}

String processor_ultrasonido(const String &var)
{
  if (var == "DISP_ULTRAS")
  {
    return "Sin respuesta";
  }

  return "ERROR-ULTRASONIDO";
}

String processor(const String &var)
{
  if (var == "ESTADO_RELE")
  {
    if (digitalRead(Rele_Pin) == 1)
    {
      Estado_Rele = "Encendido";
    }
    else
    {
      Estado_Rele = "Apagado";
    }
    return Estado_Rele;
  }
  return "ERROR-PROCESO";
}

String processor_sensor(const String &var)
{
  if (var == "LASER_IZQ")
  {
    printf("LASER_IZQ=%s\n",LASER_IZQ);
    if (estado.laser_izq == true) LASER_IZQ = "ON";
    if (estado.laser_izq == false) LASER_IZQ = "OFF";
    if (estado.laser_izq != false && estado.laser_izq != true) LASER_IZQ = "XX";
    estado.laser_izq=3;   // Estado xx
    return LASER_IZQ;
  }
  if (var == "LASER_DER")
  {
    if (estado.laser_der == true) LASER_DER = "ON";
    if (estado.laser_der == false) LASER_DER = "OFF";
    if (estado.laser_der != false && estado.laser_der != true) LASER_DER = "XX";
    estado.laser_der=3;     // Estado xx
    return LASER_DER;
  }
  if (var == "TEST_BUMPERS")     //ESTADO_BUMPERS
  {
   printf("var_TRUE=%s\n\r",var);
   ESTADO_BUMPERS=test_bumpers_rx;
   return ESTADO_BUMPERS;
  }
  if (var == "DISP_ULTRAS"){
    DISP_ULTRAS=disp_ultras_rx;
    disp_ultras_rx="Sin Rx";
    return DISP_ULTRAS;
    }
  if (var == "TEST_GPS"){
  return test_gps_rx; 
    }  
  if (var == "TEST_MPU9250"){
  return test_mpu9250_rx; 
    } 
 
  return "ERROR-SENSOR";
}

String processor_pwm(const String &var)
{
  if (var == "PWM_0")
  {
    PWM_0 = pwm_rx;
    pwm_rx="Sin Rx";
    return PWM_0;
  }
  if (var == "PWM_1")
  {
    PWM_1 = pwm_rx;
    pwm_rx="Sin Rx";
    return PWM_1;
  }

  return "ERROR-PWM";
}

String processor_bumpers(const String &var)
{
  if (var == "ESTADO_BUMPERS")
  {
   printf("var_TRUE=%s\n\r",var);
   test_bumpers_rx[4]='\0';
   return test_bumpers_rx;
  }
  printf("var_FALSE=%s\n\r",var);
  return "ERROR-BUMPERS";
}

String processor_pid(const String &var)
{
  if (var == "GET_PID")
  {
    GET_PID=get_pid_rx;
    return GET_PID;
  }

  if (var == "SEND_PID")
  {
    SEND_PID=send_pid_rx;
    return SEND_PID;
  }

  if (var == "CHECK_PID")
  {
    CHECK_PID=check_pid_rx;
    return CHECK_PID;
  }

  if (var == "PROBAR_PWM")
  {
    PROBAR_PWM="0";
    return PROBAR_PWM;
  }

  if (var == "LEER_PRUEBA")
  {
    //LEER_PRUEBA=(String) leer_prueba_rx;
    //strcpy(leer_prueba_rx,"Sin Rx");
    return leer_prueba_rx;
  }
  return "ERROR-PROCESO";
}

void setup()
{
  // Iniciamos comunicación Serial y ponemos el pin del rele como salida
  Serial.setRxBufferSize(40);
  Serial.begin(115200);
  Serial.flush();

  Serial.println("");
  WiFi.softAPConfig(ip, gateway, subnet);
  WiFi.mode(WIFI_AP);
  while (!WiFi.softAP(ssid, password))
  {
    Serial.println(".");
    delay(100);
  }

  Serial.println("");
  Serial.print("Iniciado AP:\t");
  Serial.println(ssid);
  Serial.print("IP address:\t");
  Serial.println(WiFi.softAPIP());

  if (!MDNS.begin("ROBOT_RL"))
  {
    Serial.println("ERROR: MDNS NO responder");
    while (1)
    {
      delay(1000);
    }
  }
  Serial.println("MDNS responder started");
  MDNS.addService("http", "tcp", 80);

  // Iniciamos el sistema Flash
  if (!SPIFFS.begin())
  {
    Serial.println("An Error has occurred while mounting SPIFFS");
    return;
  }

  // A partir de aqui vamos a resolver las solicitudes del cliente

  // ======== Cuando ingresen a nuestra página principal ===============================
  server.on("/", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/index.html", String(), false, processor); });
  server.on("/index.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    request->send(SPIFFS, "/index.html", String(), false, processor); });
  //*************************  Acciones *****************************
  // Cuando pulsemos el botón de encendido
  server.on("/RELE=ON", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    digitalWrite(Rele_Pin, HIGH);    
    request->send(SPIFFS, "/index.html", String(), false, processor); });
  // Cuando pulsemos el botón de apagado
  server.on("/RELE=OFF", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    digitalWrite(Rele_Pin, LOW);    
    request->send(SPIFFS, "/index.html", String(), false, processor); });

  // ========  Cuando nuestro archivo Html solicite el archivo Css =====================
  server.on("/estilos.css", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/estilos.css", "text/css"); });
  //*************************  Acciones *****************************

  //  ======== Cuando ingresen a nuestra página manual =================================
  server.on("/manual.html", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/manual.html"); });
  //*************************  Acciones *****************************
  server.on("/STOP", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    uint8_t cont=0;  
    String mensaje="STOP_ROBOT";
    Serial.println("STOP_MOT_55"); 
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/PWM_1", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    uint8_t cont=0;  
    String dato_rx,mensaje;
    if(request->hasParam("PWM_1")){                 // Hay parámetros
      AsyncWebParameter* p = request->getParam(0);  // Obtengo el puntero del primer parámetro
        dato_rx=p->value().c_str();                 // le saco el valor
        Serial.printf("PWM_1=%s_55\n", dato_rx);
        }
    mensaje=processor_pwm("PWM_1");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/PWM_0", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    uint8_t cont=0;  
    String dato_rx,mensaje;
    if(request->hasParam("PWM_0")){
      AsyncWebParameter* p = request->getParam(0);  // Obtengo el puntero del primer parámetro
        dato_rx=p->value().c_str();                 // le saco el valor
        Serial.printf("PWM_0=%s_55\n", dato_rx);
        }
    mensaje=processor_pwm("PWM_0");
    request->send(200,"text/plain", ""+mensaje);
    });

  // ========  Cuando ingresen a nuestra página automática =============================
  server.on("/auto.html", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/auto.html"); });
  //*************************  Acciones *****************************

  //  ======== Cuando ingresen a nuestra página de Tansferencia de archivos ===========
  server.on("/transfer.html", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/transfer.html"); });
  //*************************  Acciones *****************************

  //  ======== Cuando ingresen a nuestra página sensores ===============================
  server.on("/sensores.html", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    estado.laser_izq=false;
    request->send(SPIFFS, "/sensores.html"); });
  //*************************  Acciones *****************************
  // Cuando pulsemos el botón de encendido
  server.on("/LASER_IZQ=ON", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    uint8_t cont=0;  
    String mensaje;
    Serial.println("LASER_IZQ=ON_55"); 
    mensaje=processor_sensor("LASER_IZQ");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/LASER_IZQ=OFF", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje;
    Serial.println("LASER_IZQ=OFF_55");   
    mensaje=processor_sensor("LASER_IZQ");
    request->send(200,"text/plain", ""+mensaje);
    });
  // Cuando pulsemos el botón de apagado
  server.on("/LASER_DER=ON", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje;
    Serial.println("LASER_DER=ON_55"); 
    mensaje=processor_sensor("LASER_DER");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/LASER_DER=OFF", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje;    
    Serial.println("LASER_DER=OFF_55");   
    mensaje=processor_sensor("LASER_DER");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/DISP_ULTRAS", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("DISP_ULTRAS_55");   
    mensaje=processor_sensor("DISP_ULTRAS");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/TEST_BUMPERS", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("TEST_BUMPERS_55");   
    mensaje=processor_sensor("TEST_BUMPERS");
    request->send(200,"text/plain", ""+mensaje);
    });
    server.on("/DISP_ULTRAS", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("DISP_ULTRAS_55");   
    mensaje=processor_sensor("DISP_ULTRAS");
    request->send(200,"text/plain", ""+mensaje);});
  server.on("/TEST_GPS", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("TEST_GPS_55");   
    mensaje=processor_sensor("TEST_GPS");
    request->send(200,"text/plain", ""+mensaje);
    });
  server.on("/TEST_MPU9250", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("TEST_MPU9250_55");   
    mensaje=processor_sensor("TEST_MPU9250");
    request->send(200,"text/plain", ""+mensaje);
    });

  //  ======== Cuando ingresen a nuestra página de control PID   =======================
  server.on("/pid.html", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/pid.html"); });
  //*************************  Acciones *****************************
  server.on("/GET_PID", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String str_mensaje; 
    Serial.println("GET_PID_55");   
    str_mensaje=processor_pid("GET_PID");
    request->send(200,"text/plain", ""+str_mensaje);
    });
  server.on("/SEND_PID", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    int params = request->params();
    String kp_1,ki_1,kd_1,N_1,kp_2,ki_2,kd_2,N_2,str_mensaje;
    char mensaje[LONG_BUFFER_RX];
    char str_dato[LONG_BUFFER_RX];
    if(params==8){                                  // Pregunto si tiene parámetros
      AsyncWebParameter* p = request->getParam(0);  // Obtengo el puntero del primer parámetro
        kp_1=p->value().c_str();                    // le saco el valor
      AsyncWebParameter* p1 = request->getParam(1);  // Obtengo el puntero del segundo parámetro
        ki_1=p1->value().c_str();                    // le saco el valor
      AsyncWebParameter* p2 = request->getParam(2);  // Obtengo el puntero del tercero parámetro
        kd_1=p2->value().c_str();                    // le saco el valor
      AsyncWebParameter* p3 = request->getParam(3);  // Obtengo el puntero del cuarto parámetro
        N_1=p3->value().c_str();                    // le saco el valor
      AsyncWebParameter* p4 = request->getParam(4);  // Obtengo el puntero del quinto parámetro
        kp_2=p4->value().c_str();                    // le saco el valor
      AsyncWebParameter* p5 = request->getParam(5);  // Obtengo el puntero del sexto parámetro
        ki_2=p5->value().c_str();                    // le saco el valor
      AsyncWebParameter* p6 = request->getParam(6);  // Obtengo el puntero del septimo parámetro
        kd_2=p6->value().c_str();                    // le saco el valor
      AsyncWebParameter* p7 = request->getParam(7);  // Obtengo el puntero del octavo parámetro
        N_2=p7->value().c_str();                    // le saco el valor

      // se envían las constantes Kp, luego las ki, 
      // luego las kd y por último las N
      // El formato es:
      //    SKP=xxxx_yyyy_55
      //    SKI=xxxx_yyyy_55
      //    SKD=xxxx_yyyy_55
      //    SN=xxxx_yyyy_55
      // Siendo las "x" el primer motor(derecho)
      // y las "y" el segundo motor(izquierdo)

      strcpy(mensaje,"SKP=");
      snprintf(str_dato, 5, "%4s",kp_1);
      strcat(str_dato,"_");
      strcat(mensaje,str_dato);
      snprintf(str_dato, 5, "%4s",kp_2);
	    strcat(str_dato,"_55");
      strcat(mensaje,str_dato);
      Serial.println(mensaje);
      delay_ms(200);

      strcpy(mensaje,"SKI=");
      snprintf(str_dato, 5, "%4s",ki_1);
      strcat(str_dato,"_");
      strcat(mensaje,str_dato);
      snprintf(str_dato, 5, "%4s",ki_2);
	    strcat(str_dato,"_55");
      strcat(mensaje,str_dato);
      Serial.println(mensaje);
      delay_ms(200);

      strcpy(mensaje,"SKD=");
      snprintf(str_dato, 5, "%4s",kd_1);
      strcat(str_dato,"_");
      strcat(mensaje,str_dato);
      snprintf(str_dato, 5, "%4s",kd_2);
	    strcat(str_dato,"_55");
      strcat(mensaje,str_dato);
      Serial.println(mensaje);
      delay_ms(200);

      strcpy(mensaje,"SN=");
      snprintf(str_dato, 5, "%4s",N_1);
      strcat(str_dato,"_");
      strcat(mensaje,str_dato);
      snprintf(str_dato, 5, "%4s",N_2);
	    strcat(str_dato,"_55");
      strcat(mensaje,str_dato);
      Serial.println(mensaje);
    } 
    str_mensaje=processor_pid("SEND_PID");
    request->send(200,"text/plain", ""+str_mensaje);
    });
  server.on("/CHECK_PID", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String mensaje; 
    Serial.println("CHECK_PID_55");   
    mensaje=processor_pid("CHECK_PID");
    request->send(200,"text/plain", ""+mensaje);
    });
server.on("/PROBAR_PWM", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    int params = request->params();
    String prueba_pwm,prueba_time,str_mensaje;
    char mensaje[LONG_BUFFER_RX];
    char str_dato[LONG_BUFFER_SHORT];
    if(params==2){                                  // Pregunto si tiene parámetros
      AsyncWebParameter* p = request->getParam(0);  // Obtengo el puntero del primer parámetro
        prueba_pwm=p->value().c_str();              // le saco el valor
      AsyncWebParameter* p1 = request->getParam(1);  // Obtengo el puntero del segundo parámetro
        prueba_time=p1->value().c_str();             // le saco el valor

      // El formato que se envía es
      //                  P_PWM?ttt_vvv_55
      // siendo ttt el valo de tiempo en segundos
      // y vvv el valor de la velocidad de los motores
      strcpy(mensaje,"P_PWM?");
      snprintf(str_dato, 4, "%3s",prueba_pwm);
	    strcat(str_dato,"_");
	    strcat(mensaje,str_dato);
  	  snprintf(str_dato, 4, "%3s",prueba_time);
      strcat(str_dato,"_55");
      strcat(mensaje,str_dato);
      str_mensaje= (String) mensaje;
      Serial.println(str_mensaje);
    } 
    str_mensaje=processor_pid("PROBAR_PWM");
    request->send(200,"text/plain", ""+str_mensaje);
    });
server.on("/LEER_PRUEBA", HTTP_GET, [](AsyncWebServerRequest *request)
    {
    String str_mensaje; 
    Serial.println("LEER_PRUEBA_55");   
    str_mensaje=processor_pid("LEER_PRUEBA");
    request->send(200,"text/plain", ""+str_mensaje);
    });

  //  ======== Cuando ingresen a nuestra página de configuración =======================
  server.on("/config.html", HTTP_GET, [](AsyncWebServerRequest *request)
    { request->send(SPIFFS, "/config.html"); });
  //*************************  Acciones *****************************

  //  ==================== Cuando no encuentra el mensaje ===============================
  server.onNotFound([](AsyncWebServerRequest *request)
    { request->send(404, "text/html", "<h1>ROBOT-RL: ERROR-404-</h1>Pagina NO encontrada</br>Intenta otra pagina</br>"); });
  //*************************  Acciones *****************************

  // Iniciamos servidor WEB
  server.begin();
}

// Lista todos los archivos salvos na flash.
void listAllFiles()
{
  String str = "";
  Dir dir = SPIFFS.openDir("/");
  while (dir.next())
  {
    str += dir.fileName();
    str += " / ";
    str += dir.fileSize();
    str += "\r\n";
  }
  Serial.print(str);
}

void clean_data_rx(){
  for (uint8_t i=0;i<strlen(data_rx);i++)
    data_rx[i]='\0';
}

bool compara(char data1[LONG_BUFFER_RX], char data2[LONG_BUFFER_RX], uint8_t numero)
{
  bool flag_c = true;
  for (uint8_t i = 0; i < numero; i++)
  {
    if (data1[i] != data2[i])
    {
      flag_c = false;
      break;
    }
  }
  // Serial.printf("-------------------- Compara --------------------------------\n");
  // Serial.printf("El valor recibido es data1:%s con longitud:%d\n", data1,strlen(data1));
  // Serial.printf("El valor recibido es data2:%s con longitud:%d\n", data2,strlen(data2));
  // Serial.printf("El valor recibido de numero es:%d\n", numero);
  // Serial.printf("El flag devuelto es:%d\n",flag_c);
  // Serial.printf("-------------------------------------------------------------\n");

  return flag_c;
}

// En el parámetro nÚmero se pasan las cantidad de caracteres a sacar
String sacar_valor(char data1[LONG_BUFFER_RX],uint8_t numero)
{
  uint8_t i;
  // Serial.printf("\nEl valor recibido es data 1:%s\nCon longitud :%d\n", data1,strlen(data1));
  char data_temp[LONG_BUFFER_RX];
  memset(data_temp,'\0',LONG_BUFFER_RX);
  
  for(i=strlen(data1)-numero;i<strlen(data1);i++){
    data_temp[i-strlen(data1)+numero]=(char) data1[i];
  }
  return (String)data_temp;
}

void scan_respuesta(void)
{
  char data_temp[LONG_BUFFER_RX];
  int count;
  //printf("Data_rx=%s\n",data_rx);
// ********  LASER_DER=ON ************************
  if (compara(data_rx, laser_der_on, strlen(laser_der_on))){
    //printf("Sale por laser_der_on\n");
    estado.laser_der = true;
     }
// ********  LASER_DER=OFF ***********************
  if (compara(data_rx, laser_der_off, strlen(laser_der_off))){
    //printf("Sale por laser_der_off\n");
    estado.laser_der = false;
     }
// ********  LASER_IZQ=ON ************************
  if (compara(data_rx, laser_izq_on, strlen(laser_izq_on))){
    //printf("Sale por laser_izq_on\n");
    estado.laser_izq = true;
     }
// ********  LASER_IZQ=OFF ***********************
  if (compara(data_rx, laser_izq_off, strlen(laser_izq_off))){
    //printf("Sale por laser_izq_off\n");
    estado.laser_izq = false;
     }
// ********  PWM_0=XXX ***************************
  if (compara(data_rx,pwm_0, strlen(pwm_0)))
  {
    // Obtiene el número de data_rx
    pwm_rx = sacar_valor(data_rx,3);
  
    //printf("Sale por PWM_0\n");
    //printf("pwm_rx=%s\n\r",pwm_rx);
  }
// ********  PWM_1=XXX ***************************
  if (compara(data_rx,pwm_1, strlen(pwm_1)))
  {
    // Obtiene el número de data_rx
    pwm_rx = sacar_valor(data_rx,3);
    // pwm_rx[0] = data_rx[6];
    // pwm_rx[1] = data_rx[7];
    // pwm_rx[2] = data_rx[8];
    // pwm_rx[3] = data_rx[9];  
    printf("Sale por PWM_1\n");
    printf("pwm_rx=%s\n\r",pwm_rx);
  }
  // ********  DISP_ULTRAS=XXX *********************
  if (compara(data_rx, disp_ultras, strlen(disp_ultras))){
    // Obtiene el número de disp_ultras
    disp_ultras_rx = sacar_valor(data_rx,3);  
    //printf("Sale por disp_ultras\n");
    }
  // ************** TEST ***************************
  // Los valores que puede tomar X son:
  //          0 ---> bumpers sin presionar
  //          1 ---> bumpers delanteros presionado
  //          2 ---> bumpers traseros presionados
  //          3 ---> bumpers traseros y delanteros presionados
  if (compara(data_rx, test_bumpers, strlen(test_bumpers))){
    // Obtiene el número de data_rx
    test_bumpers_rx=sacar_valor(data_rx,1);
    //printf("Sale por test_bumpers\n");
    //printf("test_bumpers_rx=%s\n\r", test_bumpers_rx);
    }
  if (compara(data_rx, test_gps, strlen(test_gps))){
    // Obtiene el número de data_rx
    test_gps_rx=sacar_valor(data_rx,1);           // TBD ver la loggitud de datos
    //printf("Sale por test_gps\n");
    //printf("test_gps_rx=%s\n\r", test_gps_rx);
    }
  if (compara(data_rx, test_mpu9250, strlen(test_mpu9250))){
    // Obtiene el número de data_rx
    printf("strlen(test_mpu9250)=%d\n",strlen(test_mpu9250));
    for(count=strlen(test_mpu9250); count < LONG_BUFFER_RX; count++)
      test_mpu9250_rx[(count-strlen(test_mpu9250))]=data_rx[count+1];
      test_mpu9250_rx[count]='\0';
    printf("Sale por test_mpu9250\n");
    printf("test_mpu9250_rx=%s\n\r", test_mpu9250_rx);
    printf("test_mpu9250_dato=%s\n\r", data_rx);
    }
  // ************* SEND_PID ***************************
  // SEND_PID?kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&
  //          kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx
  // **************************************************
  if (compara(data_rx,send_pid, strlen(send_pid)))
  {
    uint8_t long_send_pid=strlen("kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx");
    //printf("Cantidad de caracteres de datos=%d\n",long_send_pid);
    // Obtiene los valores de las costantes de data_rx
    send_pid_rx =  sacar_valor(data_rx,long_send_pid);   

    // printf("Sale por SEND_PID\n");

    // printf("send_pid_rx=%s\n\r",send_pid_rx);
    // for(int cc=0;cc<long_send_pid;cc++){
    //   printf("send_pid_rx[cc]=%c\n",send_pid_rx[cc]);
    // }
  }
  // ***************** GET_PID ************************
  //  GET_PID?kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&
  //          kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx
  // **************************************************
  if (compara(data_rx,get_pid, strlen(get_pid)))
  {
    uint8_t long_get_pid=strlen("kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx");
    // Obtiene los valores de las costantes de data_rx
    get_pid_rx = sacar_valor(data_rx,long_get_pid);   // <--- Hay que trabajarlo
    //printf("Sale por GET_PID\n");
  }
  // *************** CHECK_PID ************************
  // CHECK_PID?kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&
  //           kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx
  // **************************************************
  if (compara(data_rx,check_pid, strlen(check_pid)))
  {
    uint8_t long_check_pid=strlen("kp_1=xxxx&ki_1=xxxx&kd_1=xxxx&N_1=xxxx&kp_2=xxxx&ki_2=xxxx&kd_2=xxxx&N_2=xxxx");
    // Obtiene los valores de las costantes de data_rx
    check_pid_rx = sacar_valor(data_rx,long_check_pid);   // <--- Hay que trabajarlo

    printf("Sale por CHECK__PID\n");
  }
  // **************  LEER_PRUEBA **********************
  // El formato que devuelve es:
	//    LEER_PRUEBA=XXXXXX_YYYYYY_ZZZZZZ=
  //                XXXXXX_YYYYYY_ZZZZZZ
	// El primer grupo de XYZ es la sumatoria del acelerómetro,
	// el segundo grupo de XYZ es la sumatoria del giróscos
  if (compara(data_rx, leer_prueba, strlen(leer_prueba))){
    // Obtiene el número de data_rx
    for(count=strlen(leer_prueba); count < LONG_BUFFER_RX; count++)
      leer_prueba_rx[(count-strlen(leer_prueba))]=data_rx[count+1];
      leer_prueba_rx[count]='\0';
    //printf("Sale por leer_prueba\n");
    //printf("leer_prueba_rx=%s\n\r", leer_prueba_rx);
    //printf("leer_prueba_dato=%s\n\r", data_rx);
    }

  clean_data_rx();
  }

void serialEvent()
{
  char caracter;
  static uint8_t rx_index = 0;

  while (Serial.available() > 0)
  {
    caracter = Serial.read();
    if (caracter != '\n')
    {
      data_rx[rx_index] = caracter;
      rx_index++;
    }
    else
    {
      for (uint8_t i = rx_index; i < strlen(data_rx); i++)
      {
        data_rx[i] = '\0';
      }
      flag_rx = true;
      rx_index = 0;
      Serial.flush();
    }
  }
}

void loop()
{
  MDNS.update();
  // serialEvent( );
  if (flag_rx == true)
  {
    flag_rx = false;
    scan_respuesta();
  }
}
