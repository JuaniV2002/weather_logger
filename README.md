# Weather program 
The aim of this program is to store the data a weather station would need, like max and minimum temperatures, humidity index, wind speed, among others. To effectively store the data, it uses a file where all those registers are saved. 

It offers these options:
- _Añadir nuevo registro_ (New register): This option will allow the user to input the weather data and save them in the file.
- _Suprimir registro diario_ (Delete register): It will erase a register from the file.
- _Modificar registro diario_ (Modify register): Given a date, it will allow you to modify the data of the register with that date.
- _Mostrar registros diarios_ (Show registers): This function will show all the registers in the file.
- _Buscar registro_ (Search register): As the name suggests, it will look for a certain register using the date (passed to the function as a parameter).
- _Mostar dias de maxima temperatura_ (Show days of maximum temperature): It will show the dates where there was a maximum temperature.
- _Mostrar dias de maxima precipitacion_ (Show days of maximum rainfall): It will sort the days of maximum rainfall and show the dates.
- _Mostrar dias de maxima velocidad de viento_ (Show days of max windspeed): As the previous option, it will sort and show windspeed numbers and the dates.
- _Realizar copia de seguridad_ (Make a backup): It will backup all the active registers into another file. 

To give this program a try on Linux:
- Install gcc with this command:
  ```
  sudo apt update
  sudo apt install build-essential
  ```
- Check C compiler version to verify a successful installation:
  ```
  gcc --version
  ```
- Compile the program:
  ```
  gcc -o weatherStation weatherStation.c
  ```
- Run the program:
  ```
  ./weatherStation
  ``` 
  ---
# Programa del clima 
El objetivo de este programa es guardar los datos que una estacion climatica necesitaria, como temperaturas maximas y minimas, indices de humedad, velocidad del viento, entre otros. Para realmente guardar los datos, el programa usa un archivo en donde todos los registros son guardados. 

Ofrece las siguientes opciones:
-  _Añadir nuevo registro_: Esta opcion perimte al usuario cargar los datos y guardarlos en el archivo.
- _Suprimir registro diario_: Borrara un regsitro del archivo.
- _Modificar registro diario_: Dado una fecha, perimitira modificar los datos del registro con esa fecha.
- _Mostrar registros diarios_: Esta opcion mostrara todos los registros del archivo
- _Buscar registro_: Como el nombre sugiere, buscara a un cierto registro usando su fecha (que es pasada como parametro a la funcion).
- _Mostrar dias de maxima temperatura_: Mostrara los dias en donde hubieron temperaturas maximas.
- _Mostrar dias de maxima precipitacion_: Ordenara los dias de maxima precipitacion y mostrara las fechas correspondientes.
- _Mostrar dias de maxima velocidad de viento_: Como la opcion anterior, ordenara y mostrara los dias con maxima velocidad de viento.
- _Realizar copia de seguridad_: Realizara una copia de seguridad de todos los registros no borrados a otro archivo. 

Para probar este programa en Linux:
- Instalar gcc con este comando:
  ```
  sudo apt update
  sudo apt install build-essential
  ```
- Chequear version de gcc para verificar su correcta instalacion:
  ```
  gcc --version
  ```
- Compilar el programa:
  ```
  gcc -o weatherStation weatherStation.c
  ```
- Correr el programa:
  ```
  ./weatherStation
  ``` 
