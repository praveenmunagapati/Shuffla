#include <cstdio>

#include "query_dispatcher.hpp"
#include "../engine/search_result/search_result.hpp"

QueryDispatcher::QueryDispatcher(SearchEngine* search_engine) : search_engine(search_engine)
{
    // right now  nothing
    // but it will change in the future
}

QueryDispatcher::~QueryDispatcher()
{
    // intensionally nothing
}

std::string QueryDispatcher::process_query(const std::string& request)
{
    // request contains request parameters
    // so if user requests localhost:3000/this/is/cool
    // request is equal "/this/is/cool"

    printf("%s\n", request.c_str());

    Query* query = query_parser.parse_query(request);
    if (query == NULL) {
        return "Error: Unknown request format. Unable to parse " + request;
    }

    SearchResult* result = search_engine->process_query(query);
    std::string ret = result->to_string();

    delete result;
    return ret;
}