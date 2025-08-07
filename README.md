# 📦 Fantom Chat - P2P IPv6 Chat

**Fantom Chat - P2P IPv6 Chat** is a lightweight peer-to-peer and client-server messenger focused on privacy, autonomy, and security.  
Built in C++ using Qt.

---

## ⚖️ Disclaimer

This software is provided in goodwill and for constructive purposes only.  
The author is not responsible for any misuse, abuse, or harm resulting from its deployment.  

🧠 Use responsibly.

---

## 🔧 Features

- ✅ P2P IPv6 support  
- ✅ Multi-chat capability  
- ✅ Server and client run in separate threads  
- ✅ Custom binary protocol with length-prefix framing  
- ✅ Signal-slot architecture (Qt style)  
- ✅ UI built with Qt6 and FontAwesome Free  
- ⏳ In-memory chat list and messages  
- ⏳ End-to-end encryption and PGP key exchange *(planned)*  
- ⏳ Message history and storage options *(planned)*  
- ⏳ Pluggable storage strategies: RAM / Encrypted Files / Database *(planned)*

---

## 📂 Dependencies

- **Qt 6**  
- **FontAwesome Free** via `qtawesome`  
- **cURL** — for fetching external IP  
- **OpenSSL** — cryptographic routines  
- **zlib** — compression utilities  
- **libsodium** - cryptography
- Win64 / OSX 15+ / Linux - in future

---

## 🧠 How to build?

- Use Qt Creator with Qt 6.9 or more
- For Win64
- - MSYS2 UCRT64
- - Pacman install: ZLIB, Lib Sodium, OpenSSL, CURL 4
- - Ensure, that PATHs are correlate with .pro file
- For OSX pretty same, but with homebrew
- Or <a href="mailto:emilerofeevskij@gmail.com">email me</a>

## 🛡️ License

- Project code — [P2P IPv6 Chat License – Fair Source, v1.0 (Emil (Emilian) Erofeevskiy Edition)](./LICENSE.md)  
- FontAwesome Free — [CC BY 4.0 License](https://fontawesome.com/license/free)  
- Qt — [GNU LGPL v3](https://doc.qt.io/qt-6/lgpl.html)

---

### Usage Terms

| Use Case               | License Requirement         |
|------------------------|-----------------------------|
| Private / Personal Use | ✅ Free                     |
| Commercial / Enterprise Use | 💼 Requires license |

---

## ✨ About

Created by **Emilianissimo** *(Emil (Emilian) Erofeevskiy)*  
Open-source. Transparent by design.  
Pull requests, issues, and discussions are welcome.
