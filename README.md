# Meteorological Daily Records Management System

## Overview

This C program is a command-line application designed to **manage and analyze daily meteorological records** using a human-readable CSV file. Users can add, modify, logically delete, search, and list weather data. The program handles data such as temperature, humidity, pressure, wind speed/direction, and precipitation for each day, providing an organized way to keep and analyze local weather history.

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
  - Load data from CSV on demand (menu option 9)

## Data Structure

- **DailyRecord**: Structure containing all meteorological data for one day, including:
  - Date (DD/MM/YYYY)
  - Max/Min temperature
  - Humidity
  - Atmospheric pressure
  - Wind direction and speed
  - Precipitation
  - Logical deletion flag

- **DataArray**: Struct holding an array of `DailyRecord` records and its count, used for in-memory processing.

- **Linked lists (Node)**: Used for sorted reporting (temperature, wind speed, precipitation rankings).

## How It Works

- Data lives in memory while the app runs. Load an existing CSV via the menu when you need it.
- Records are never physically removed—**logical deletion** is tracked by a boolean flag and excluded on save.
- User interacts via a text-based menu to perform all actions.
- QuickSort is used internally by the reporting lists.
- On exit, the app asks whether to save changes back to the CSV file (using the most recently loaded filename).

## Usage

### Compile the program

```sh
gcc -Wall -Wextra -std=c11 -o weatherStation weatherStation.c
```

### Run the program

```sh
./weatherStation
```

At startup, the app begins with an empty in-memory dataset. Use the menu option “Load data from CSV file (9)” to load from a CSV (default sample provided as `weather.csv`). Dates must be entered as DD/MM/YYYY. On exit, you’ll be asked if you want to save changes back to the CSV file (using the most recently loaded filename).

Sample data:
- A ready-to-use sample CSV `weather.csv` with 10 records is included so you can try listing, searching, and reporting immediately.
