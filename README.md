# Meteorological Daily Records Management System

## Overview

This C program is a command-line application designed to **manage, analyze, and back up daily meteorological records**. Users can add, modify, delete, search, and list weather data stored in binary files. The program handles data such as temperature, humidity, pressure, wind speed/direction, and precipitation for each day, providing an organized way to keep and analyze local weather history.

## Features

- **Add new daily records** with meteorological data (date, temperature, humidity, pressure, wind, precipitation)
- **Logical deletion** of records (without physical removal from file)
- **Modify existing records** based on date
- **Display all stored records**
- **Search for a record** by date and display its details
- **List days with:**
  - Maximum temperature
  - Maximum precipitation
  - Highest wind speed (top 10 days)
- **Create a backup** of the current year’s data (excluding deleted records)

## Data Structure

- **regDiario**: Structure containing all meteorological data for one day, including:
  - Date (`ddmmyyyy`)
  - Max/Min temperature
  - Humidity
  - Atmospheric pressure
  - Wind direction and speed
  - Precipitation
  - Logical deletion flag

- **TData**: Struct holding an array of `regDiario` records and its count, used for in-memory processing.

- **Linked lists**: Used for sorted reporting (temperature, wind speed, precipitation rankings).

## How It Works

- All data is stored in a **binary file** specified by the user at program start.
- Records are never physically deleted—**logical deletion** is performed by a boolean flag.
- User interacts via a text-based menu to perform all actions.
- QuickSort is used internally to sort records for reports.
- Backup files are created by copying valid (not deleted) records to a new file prefixed with `copia_seguridad_`.

## Usage

### Compile the program

```sh
gcc -o weatherStation weatherStation.c
```

### Run the program

```sh
./weatherStation
```
