// #include "parquet.h"
// #include <arrow/api.h>
// #include <parquet/arrow/reader.h>
// #include <iostream>

// std::vector<std::vector<float>> parseParquetFile(const std::string& filepath)
// {
//     std::vector<std::vector<float>> result;

//     std::shared_ptr<arrow::io::ReadableFile> infile;
//     auto status = arrow::io::ReadableFile::Open(filepath, arrow::default_memory_pool(), &infile);
//     if (!status.ok()) {
//         std::cerr << "Failed to open Parquet file: " << filepath << "\n";
//         return result;
//     }

//     std::unique_ptr<parquet::arrow::FileReader> reader;
//     status = parquet::arrow::OpenFile(infile, arrow::default_memory_pool(), &reader);
//     if (!status.ok()) {
//         std::cerr << "Failed to create Parquet reader\n";
//         return result;
//     }

//     std::shared_ptr<arrow::Table> table;
//     status = reader->ReadTable(&table);
//     if (!status.ok()) {
//         std::cerr << "Failed to read Parquet table\n";
//         return result;
//     }

//     for (int64_t r = 0; r < table->num_rows(); ++r) {
//         std::vector<float> row;
//         for (int c = 0; c < table->num_columns(); ++c) {
//             auto column = table->column(c);
//             auto chunk = std::static_pointer_cast<arrow::FloatArray>(column->chunk(0));
//             row.push_back(chunk->Value(r));
//         }
//         result.push_back(row);
//     }

//     return result;
// }
