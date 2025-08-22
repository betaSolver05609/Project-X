# Project-X VectorDB  

A lightweight, CLI-based vector database written in C++.  
Supports multiple keyspaces, configurable similarity metrics, persistence to disk, and simple record management.  

---

## ✨ Features
- Create and drop **keyspaces** (logical vector collections).  
- Configurable similarity metrics:
  - `COSINE`
  - `EUCLIDEAN`
  - `DOT`
- Insert and search vector records.  
- Change keyspace metrics (`ALTER KEYSPACE … SET METRIC …`).  
- Persistence: each keyspace is stored as `data/<KEYSPACE>.json`.  
- Show current records or metrics per keyspace.  

---

## 🔧 Build
```bash
make


$ ./db_start
Welcome to Project-X VectorDB CLI
>>

>> CREATE KEYSPACE USERS
Keyspace 'USERS' created with metric 'cosine'.

>> CREATE KEYSPACE VECTOR WITH METRIC DOT
Keyspace 'VECTOR' created with metric 'dot'.


>> LIST KEYSPACES
- USERS
- VECTOR


>> DROP KEYSPACE USERS
Keyspace 'USERS' dropped.


>> INSERT VECTOR 1,2,3
SUCCESS : Record inserted
>> INSERT VECTOR 2,3,4
SUCCESS : Record inserted


>> SHOW RECORDS VECTOR
Record 0: [ 1 2 3 ]
Record 1: [ 2 3 4 ]


>> SEARCH VECTOR 9,8,7 TOP 2
Record Index: 0 | Similarity: 0.93325
Record Index: 1 | Similarity: 0.680787


>> SHOW METRIC FOR VECTOR
Keyspace 'VECTOR' is using metric 'dot'.


>> ALTER KEYSPACE VECTOR SET METRIC COSINE
Keyspace 'VECTOR' metric changed to 'cosine'.

Persistence
{
  "metric": "dot",
  "records": [
    [ 1.0, 2.0, 3.0 ],
    [ 2.0, 3.0, 4.0 ]
  ]
}

Exit
>> EXIT
Bye 👋

Supported Commands (Cheat Sheet)

CREATE KEYSPACE <name> [WITH METRIC COSINE|EUCLIDEAN|DOT]
DROP KEYSPACE <name>
LIST KEYSPACES
INSERT <keyspace> v1,v2,v3,...
SHOW RECORDS <keyspace>
SEARCH <keyspace> v1,v2,v3,... TOP k
SHOW METRIC FOR <keyspace>
ALTER KEYSPACE <name> SET METRIC COSINE|EUCLIDEAN|DOT
EXIT
