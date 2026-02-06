# THE PADDOCK CLUB
### Exotic & Performance Car Rental Management System

<p align="center">
  <img src="assets/Logo/The Paddock Club.png" alt="The Paddock Club Logo" width="400">
</p>

---

## 🏎️ Overview
**The Paddock Club** is a premium, high-performance car rental management system designed for automotive enthusiasts. This project was developed for the **CCP4213 Data Structure and Algorithms** course to demonstrate the effective implementation of various data structures in a real-world enthusiast application.

The fleet features a hand-picked collection of 26 iconic vehicles, ranging from the legendary **McLaren F1** and **Bugatti EB110** to the high-tech **Ferrari 812 Competizione** and cult classics like the **Proton Satria Neo R3 Lotus**.

---

## 🛠️ Data Structures & Algorithms
This project implements a comprehensive set of data structures to manage the business logic:

1. **Linked List (Showroom Inventory):** Manages the primary fleet, allowing for dynamic addition and removal of vehicles.
2. **Queue - Circular Array (Wash Bay):** A FIFO system to handle returned cars that require servicing before becoming available again.
3. **Stack - Linked Implementation (Activity Logs):** A LIFO system tracking rental history and search logs.
4. **Hash Table (Quick Lookup):** Provides $O(1)$ fast-lookup capabilities using car IDs to verify inventory instantly.
5. **Enhanced Sorting:** Allows users to sort the catalog alphabetically by **Brand**, or by performance metrics like **Daily Rate** and **Horsepower**.

---

## ✨ Features
- **Ultra-Rare Fleet:** 26 performance cars with rarity-based stock management (Iconic cars are limited to 1 unit).
- **Insurance Logic:** Automated safety checks restricting 500+ HP vehicles to drivers aged 25 and above.
- **Modern UI:** High-end terminal interface with ANSI color-coded status indicators (Green for Available, Red for Out).
- **Persistence:** Full data persistence across sessions via CSV data engines for fleet, customers, and revenue.
- **Admin Console:** Secure interface for fleet management, analytics, and credential updates.

---

## 🚀 How to Run
Ensure you have a C++ compiler installed (e.g., `g++`).

1. **Compile the project:**
   ```bash
   g++ -o paddock_club "src/AdamAnwar(243DC245L4).cpp"
   ```

2. **Run the application:**
   ```bash
   ./paddock_club
   ```

---

## 📂 Project Structure
- `src/`: Main C++ source code.
- `data/`: CSV databases for fleet, customers, and logs.
- `assets/`: Branding and UI assets.
- `docs/`: Academic documentation and rubrics.

---

## 👤 Author
**Adam Anwar**
*(243DC245L4)*
Project for CCP4213 - Data Structure & Algorithms
