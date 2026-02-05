# THE PADDOCK CLUB
### Exotic & Performance Car Rental Management System

<p align="center">
  <img src="Logo/The Paddock Club.png" alt="The Paddock Club Logo" width="400">
</p>

---

## 🏎️ Overview
**The Paddock Club** is a premium, high-performance car rental management system designed for automotive enthusiasts. This project was developed for the **CCP4213 Data Structure and Algorithms** course to demonstrate the effective implementation of various data structures in a real-world enthusiast application.

The fleet features iconic vehicles ranging from the **Mercedes SLS AMG Black Series** and **Ferrari 812 Competizione** to cult classics like the **Renault Clio V6** and **Proton Satria Neo R3 Lotus**.

---

## 🛠️ Data Structures & Algorithms
To meet (and exceed) the project requirements, the following structures were implemented:

1. **Linked List (Showroom):** Manages the primary fleet inventory, allowing for dynamic addition and removal of vehicles.
2. **Queue - Array Implementation (Wash Bay):** A First-In-First-Out (FIFO) system to handle returned cars that require servicing before becoming available again.
3. **Stack - Linked Implementation (Rental/Search Logs):** A Last-In-First-Out (LIFO) system that tracks recent activity and user search history.
4. **Hash Table (Key Box):** Provides $O(1)$ fast-lookup capabilities using car IDs to verify inventory instantly.
5. **Sorting (Selection Sort):** Allows users to sort the catalog by **Daily Rate** or **Horsepower** to find the perfect ride.

---

## ✨ Features
- **Elite Fleet:** 20 hand-picked performance cars with detailed specs (HP, Top Speed, Transmission).
- **Safety Check:** Automated "Insurance" logic that restricts high-horsepower vehicles (500+ HP) to drivers aged 25 and above.
- **Digital Receipts:** Beautifully formatted terminal receipts generated upon successful rental.
- **Admin Mode:** Secure interface to add or decommission vehicles from the fleet.
- **ANSI Color Interface:** A modern terminal UI with color-coded alerts and headers.
- **Input Protection:** Robust validation to prevent program crashes from invalid menu inputs.

---

## 🚀 How to Run
Ensure you have a C++ compiler installed (e.g., `g++`).

1. **Compile the project:**
   ```bash
   g++ "AdamAnwar(243DC245L4).cpp" -o PaddockClub
   ```

2. **Run the application:**
   ```bash
   ./PaddockClub
   ```

---

## 👤 Author
**Adam Anwar**
*(243DC245L4)*
Project for CCP4213 - Data Structure & Algorithms