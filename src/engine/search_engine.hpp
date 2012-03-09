#ifndef SEARCHENGINE_H
#define SEARCHENGINE_H

#include <string>

#include "query/query.hpp"
#include "query/insert/query_insert.hpp"
#include "query/search/query_search.hpp"
#include "query/create_table/query_create_table.hpp"
#include "search_result/search_result.hpp"

class SearchEngine
{
public:
    SearchEngine();
    virtual ~SearchEngine();
    SearchResult* process_query(const Query* query);
protected:
private:
    void processing_query_begin(const Query* query);
    void processing_query_end(const Query* query);

    SearchResult* process_insert(const QueryInsert* query);
    SearchResult* process_search(const QuerySearch* query);
    SearchResult* process_create_table(const QueryCreateTable* query);
};

#endif // SEARCHENGINE_H