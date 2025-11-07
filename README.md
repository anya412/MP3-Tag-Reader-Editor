# 🎵 MP3 Tag Editor & Viewer (C Project)

A **command-line utility** written in **C** to view and modify ID3 tags in MP3 files.  
This project highlights proficiency in **file I/O**, **binary parsing**, and **modular program design** — core skills for systems, firmware, and embedded engineering.

---

## 🧭 Overview

The MP3 Tag Editor provides a terminal-based interface for inspecting and updating **MP3 metadata**.  
It works directly with the **binary structure of ID3v1 tags**, without any external dependencies.

**Supported Metadata Fields**
- Title  
- Artist  
- Album  
- Year  
- Genre  
- Comment  

---

## ✨ Features

- 🔍 **Read Tags:** Extracts and displays MP3 metadata in a clean, readable format  
- ✏️ **Edit Tags:** Update specific fields (e.g., title, artist) via simple CLI arguments  
- 🧩 **Modular Architecture:** Separate modules for viewing, editing, and validation  
- ⚙️ **Binary File Handling:** Reads and writes directly to ID3 tag blocks  
- 🚫 **Error Management:** Detects missing files or invalid formats gracefully  

---

## 🛠️ Tech Stack

| Category | Details |
|-----------|----------|
| **Language** | C |
| **Libraries** | Standard C Libraries (`stdio.h`, `stdlib.h`, `string.h`) |
| **Platform** | Linux / Unix |
| **Tools** | GCC, Git, Command-Line Interface |

---

## 💻 Build Instructions

### 1. Compile
```bash
gcc main.c view.c edit.c -o mp3tag
```
---

## 🖥️ Run Instructions

### 2. Run
```bash
./mp3tag [option] [arguments]
```
---

### 🧪 Examples

**View all tags**
```bash
./mp3tag -v song.mp3
```
---

## 🧩 Supported Tag Codes

| **Tag Code** | **Field Name**     | **Description**                              |
|:-------------:|:-------------------|:---------------------------------------------|
| **TIT2**      | Title              | The title or name of the track               |
| **TPE1**      | Artist             | The artist or performer of the track         |
| **TALB**      | Album              | The album or collection the track belongs to |
| **TYER**      | Year               | The year the recording was released          |
| **TCON**      | Genre              | The genre classification of the track        |
| **COMM**      | Comment            | Additional notes or comments about the track |

## 👩‍💻 Author

**Ananya Jayaprakash**  
🎓 *B.Tech in Electrical & Electronics Engineering*  
💡 *Embedded Systems & Firmware Enthusiast*  
📧 **Email:** [ananyajayaprakash00@gmail.com](mailto:ananyajayaprakash00@gmail.com)

