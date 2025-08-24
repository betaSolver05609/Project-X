# Project-X

Project X is a lightweight, high-performance Vector Database designed for AI developers, researchers, and engineers who need to ingest, store, and search embeddings efficiently.

Unlike traditional, resource-heavy databases, Project X is written entirely in modern C++, giving it a low memory footprint and blazing-fast performance. This makes it ideal for rapid prototyping, experimentation, and deployment in environments where speed and efficiency are critical.

Key Highlights for AI Developers:

Efficient Embedding Storage – Seamlessly ingest word embeddings, sentence embeddings, or any numerical vector data.

Fast Similarity Search – Perform nearest-neighbor or similarity lookups without the overhead of large-scale database engines.

Lightweight & Portable – Minimal dependencies, easy to integrate into existing ML pipelines or deploy on resource-constrained systems.

Experimentation Ready – Perfect for testing new AI models, validating embedding quality, or building quick prototypes.

With Project X, AI developers can focus on building and experimenting with intelligent systems — from semantic search and recommendation engines to custom NLP applications — without being slowed down by heavy database infrastructure.

Current Capabilities:
1. Ingesting large volumes of data along with disk persistence
2. Supports Ingesting CSV and JSON format files
3. Search capabilities includes metrics like COSINE, EUCLIDEAN, MANHATTAN, CHEBYSHEV, JACCARD, MAHALANOBIS, HAMMING, JSD,MINKOWSKI and DOT
4. Easy CLI based access with simple commands

Future Capabilities:
1. gRPC and REST access
2. Centralized Server Features along with CLI versions
3. Index Optimization with ASN and HSNW

---

## ⚙️ System Requirements

- **Operating System:**  
  - Linux (Ubuntu/Debian recommended)  
  - Or Windows with **WSL2** (Ubuntu/Debian)  

- **Dependencies:**  
  - `g++` (C++17 or higher)  
  - `make`  
  - `libjsoncpp`  

**The installer will automatically check and install these dependencies if missing.** 

---

## 📦 Download

Download the latest release from the following link:  
👉 [Download Project-X](<INSERT_LINK_HERE>)  

**Installation Steps**
```bash
1. unzip Project-X.zip
2. cd Project-X/X
3. chmod +x install.sh
4. ./install.sh
```
The installer will install all the required dependency and build the Database Engine

**Start**
In your terminal run
```bash
projectx
```
it will start the database

**After installation if you get the error "command not found"
then add the following line in your bashrc or zshrc

```bash
export PATH="$HOME/.local/bin:$PATH"
```

and reload your shell

```bash
source ~/.bashrc
```

