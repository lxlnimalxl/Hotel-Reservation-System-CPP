# 🏨 Luxury Hotel Reservation CLI (C++)

A colorful, cross‑platform **terminal hotel reservation system** written in modern **C++17**. It features animated loaders, ANSI‑colored UI, persistent storage to text files, input validation, search, and a friendly step‑by‑step workflow.

---

## ✨ Features

* **Rich TUI**: ANSI colors, bold/underline, ASCII art banner, spinners, and loading dots.
* **Persistent data**: Saves **rooms** to `rooms.txt` and **reservations** to `reservations.txt`.
* **Safe parsing**: Ignores corrupted lines, creates files when missing.
* **End‑to‑end flows**: View/reserve/check‑out/search rooms with confirmations and previews.
* **Keyboard hints**: Press **`b`** at most prompts to go back; **Enter** to continue on info screens.
* **Validation**: Dates use `YYYY-MM-DD` format and require `year >= 2025`.
* **Cross‑platform**: Works on Linux/macOS/Windows (UTF‑8 codepage switch on Windows).

---

## 📦 Requirements

* A C++17 compiler (GCC ≥ 7, Clang ≥ 6, or MSVC ≥ 19.14 / VS 2017).
* A terminal that supports ANSI escape sequences (Linux/macOS, Windows 10+).
  On Windows, the program switches to **UTF‑8** via `chcp 65001`.

---

## 🛠 Build & Run

### Linux / macOS (g++ or clang++)

```bash
# Using g++
g++ -std=c++17 -O2 -Wall -Wextra -o hotel main.cpp
./hotel

# Using clang++
clang++ -std=c++17 -O2 -Wall -Wextra -o hotel main.cpp
./hotel
```

### Windows (MinGW / MSYS2)

```bash
g++ -std=c++17 -O2 -Wall -Wextra -o hotel.exe main.cpp
hotel.exe
```

### Windows (MSVC / Developer Command Prompt)

```bat
cl /std:c++17 /EHsc /W4 /O2 main.cpp /Fe:hotel.exe
hotel.exe
```

> **Note:** If colors don’t render on Windows, enable **Virtual Terminal** in your terminal or run in Windows Terminal/PowerShell.

---

## 🗂 Data Files & Formats

The app stores data next to the executable. If files are missing, they’re created automatically.

### `rooms.txt`

Whitespace‑separated fields:

```
<number> <type> <price> <availableInt>
```

Example:

```
101 Single 100 1
102 Double 150 1
103 Suite 300 0
```

* `availableInt`: `1` = available, `0` = occupied.

### `reservations.txt`

Pipe‑delimited fields (guest spaces are stored as underscores):

```
<guest_name_with_underscores>|<room_number>|<check_in>|<check_out>
```

Example:

```
John_Doe|103|2025-10-05|2025-10-08
```

---

## 🚀 Usage Overview

After launch you’ll see an ASCII banner, the current time, and a summary of rooms/reservations.

**Main Menu**

1. **View Available Rooms** — list all rooms with `Yes/No` availability.
2. **Reserve a Room** — choose room #, enter guest name, and check‑in/out dates; confirm.
3. **View All Reservations** — table of all bookings.
4. **Check Out a Guest** — frees the room and removes its reservation after confirmation.
5. **Add New Room** — add a new room (unique number, type, price).
6. **Search Reservation** — search by partial **guest name** or exact **room number**.
7. **Help** — quick guide and tips.
8. **Exit** — saves data and exits with a spinner.

**Nav Tips**

* Enter `b` or `B` to return to the previous screen on most prompts.
* Information screens wait for **Enter**.

---

## 🔐 Validation Rules

* **Dates**: `YYYY-MM-DD`, e.g., `2025-07-01`; year must be `≥ 2025`.
* **Room number**: positive integer; must be unique when adding.
* **Price**: positive number.
* **Guest name**: non‑empty (stored with spaces replaced by `_`).

---

## 🧰 Code Structure (High‑Level)

* `Room` — number, type, price, `isAvailable`.
* `Reservation` — guest name, room number, check‑in/out.
* `Hotel` — orchestrates UI, persistence, validation, and all flows:

  * **Storage**: `loadRooms/saveRooms`, `loadReservations/saveReservations`.
  * **UI**: `displayASCIIArt`, `loadingAnimation`, `spinnerAnimation`.
  * **Actions**: `viewAvailableRooms`, `reserveRoom`, `viewReservations`, `checkOut`, `addNewRoom`, `searchReservation`.

---

## 🧪 Quick Test Script (Manual)

1. Start the app — confirm default rooms were created.
2. Reserve room **103** for **Jane Roe**, `2025-12-02 → 2025-12-05`.
3. Verify it disappears from *Available Rooms* and appears in *View Reservations*.
4. Use *Search Reservation* by typing `jane` and by `103`.
5. *Check Out* room **103** and verify it’s available again.

---

## 🐞 Troubleshooting

* **Colors look weird on Windows**: Use Windows Terminal/PowerShell; ensure VT sequences are enabled.
* **Garbled characters**: Confirm the console is in UTF‑8 (the app calls `chcp 65001`).
* **Files don’t update**: Ensure you have write permissions in the working directory.
* **Invalid date**: Format must be `YYYY-MM-DD` and year `≥ 2025`.

---

## 🗺 Roadmap / Ideas

* Date range conflict checks (overlapping reservations per room).
* Persist guest names with original spaces (escape/quote instead of underscores).
* Edit reservation details (dates, guest name).
* Export reports (CSV/JSON) and daily summaries.
* Unit tests for parsing/validation.

---

## 📄 License

MIT — free to use, modify, and distribute. A star ⭐ on the repo is always appreciated!
