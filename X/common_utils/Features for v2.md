Features for v2

1. Disk persistence — Done
2. Indexing changes — user can create an index on columns for better search
3. Transaction handling — ACID; should be available to end users
4. Multithreading: Batch Operations
5. No schema - vectors can be different dimensions per keyspace
6. Connecting to DB — client connects using host, port, username and password
7. Connection pooling (for future server mode)
8. HNSW index


Keyspace(Table) -->[metric] --> Block()
