# Weather program 
The aim of this program is to store the data a weather station would need, like max and minimum temperatures, humidity index, wind speed, among others. To effectively store the data, it uses a file where all those registers are saved. 

It offers these options:
- _Añadir nuevo registro_ (New register): this option will allow the user to input the weather data and save it in the file.
- _Suprimir registro diario_ (Delete register): it will delete a register from the file.
- _Modificar registro diario_ (Modify register): given a date, it will allow you to modify the data of the register with that date.
- _Mostrar registros diarios_ (Show registers): this function will show all the registers in the file.
- _Buscar registro_ (Search register): as the name suggests, it will look for a certain register using the date (passed to the function as a parameter).
- _Mostar dias de maxima temperatura_ (Show days of maximum temperature): it will show the dates where there was a maximum temperature.
- _Mostrar dias de maxima precipitacion_ (Show days of maximum rainfall): it will sort the days of maximum rainfall and show the dates.
- _Mostrar dias de maxima velocidad de viento_ (Show days of max windspeed): as the previous option, it will sort and show windspeed numbers and the dates.
- _Realizar copia de seguridad_ (Make a backup): it will backup all the active registers into a new file. 

To give this program a try:
- Install gcc with this command:
  ```
  sudo apt update
  sudo apt install build-essential
  ```
  or with this command if you are on mac
  ```
  brew install gcc
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
