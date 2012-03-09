#include <boost/algorithm/string/predicate.hpp>

#include "query_search.hpp"

const std::string QuerySearch::HTTP_PREFIX = "/search/";

QuerySearch::QuerySearch()
{
    //ctor
}

QuerySearch::~QuerySearch()
{
    //dtor
}

bool QuerySearch::set(const std::string& request)
{
    return true;
}

Query* QuerySearch::parse(const std::string& request) const
{
    if (boost::starts_with(request, HTTP_PREFIX)) {
        QuerySearch* result = new QuerySearch();
        if (result->set(request)) {
          return result;
        }
        delete result;
    }

    return NULL;
}

bool QuerySearch::is_modyfing_data() const
{
    return false;
}