#include "kd_vertice.hpp"

KDVertice::KDVertice(const TableIndexInfo& table_index_info, Boundary boundary) : table_index_info_(table_index_info), boundary_(boundary)
{
    left_ = NULL;
    right_ = NULL;
}

KDVertice::KDVertice(const TableIndexInfo& table_index_info) : table_index_info_(table_index_info), boundary_(Boundary(table_index_info))
{
    left_ = NULL;
    right_ = NULL;
}

KDVertice::~KDVertice()
{
    delete left_;
    delete right_;
}


void KDVertice::clear()
{
    rows_.clear();
    delete left_;
    delete right_;
}

void KDVertice::dump_all_rows(DumpSaver& dump_saver) const
{
    dump_saver.append(Misc::int_to_string(rows_.size()) + "\n");
    for(std::multiset<const Row*>::iterator it = rows_.begin(); it!=rows_.end(); it++) {
        dump_saver.append((*it)->to_string());
    }
}

void KDVertice::add_collection(std::vector<const Row*> rows)
{
    rows_.insert(rows.begin(), rows.end());
    rebuild();
}

bool KDVertice::contains_row(const Row* row) const
{
    return rows_.find(row) != rows_.end();
}


bool KDVertice::insert_row(const Row* row, int k)
{
    if (!boundary_.is_point_inside(row)) return false;
    if (contains_row(row)) return false;

    rows_.insert(row);

    if (left_ == NULL) {
        rebuild();
        return true;
    }

    if (left_ != NULL) left_->insert_row(row, k+1);
    if (right_ != NULL) right_->insert_row(row, k+1);


    return true;
}

bool KDVertice::delete_row(const Row* row)
{
    if (!boundary_.is_point_inside(row)) return false;
    if (!contains_row(row)) return false;

    rows_.erase(rows_.find(row));
    if (left_ != NULL) left_->delete_row(row);
    if (right_ != NULL) right_->delete_row(row);
    return true;
}

std::vector<const Row*>  KDVertice::search(const QueryBoundary& query_boundary) const
{
    if (query_boundary.contains(boundary_)) {
        if (!query_boundary.are_there_extra_requiremens()) return std::vector<const Row*>(rows_.begin(), rows_.end());
        return filter_non_index_conditions(query_boundary);
    }

    if (query_boundary.disjoint(boundary_)) {
        return std::vector<const Row*>();
    }

    if (left_ == NULL) return linear_filter(query_boundary);

    std::vector<const Row*> result = left_->search(query_boundary);
    std::vector<const Row*> result2 = right_->search(query_boundary);
    result.insert(result.end(), result2.begin(), result2.end());

    return result;
}

void KDVertice::rebuild()
{
    for(std::multiset<const Row*>::iterator it = rows_.begin(); it !=rows_.end(); it++) {
        assert(boundary_.is_point_inside(*it));
    }

    if (rows_.size() > 50 && left_ == NULL) {
        Limiter limit = find_good_limiter();

        Boundary left_boundary(boundary_);
        left_boundary.add_limiter(limit);
        Boundary right_boundary(boundary_);
        right_boundary.add_limiter(limit.createReverseLimiter());

        left_ = new KDVertice(table_index_info_, left_boundary);
        right_ = new KDVertice(table_index_info_, right_boundary);
        std::vector<const Row*> left_collection, right_collection;

        std::string property = limit.get_property_name();
        for(std::multiset<const Row*>::iterator it = rows_.begin(); it !=rows_.end(); it++) {
            if (limit.is_value_matching((*it)->get_value(property))) {
                left_collection.push_back(*it);
            } else {
                right_collection.push_back(*it);
            }
        }

        left_->add_collection(left_collection);
        right_->add_collection(right_collection);
    }
}

Limiter KDVertice::find_good_limiter() const
{
    std::vector<std::string> props = table_index_info_.get_table_definition()->get_property_names();
    std::vector<Limiter> limiters;

    for(std::multiset<const Row*>::iterator it = rows_.begin(); it !=rows_.end(); it++) {
        for(int mask = 0; mask < 4; mask++) {
            if (rand() % (rows_.size() / 5 + 1) == 0) {
                std::string property = props[rand() % props.size()];
                limiters.push_back(Limiter(property, (*it)->get_value(property), mask/2, mask&1));
            }
        }
    }

    int best_index = -1;
    int res = INT_MAX;
    for(std::size_t i=0; i<limiters.size(); i++) {
        int local_res = calculate_limiter_efficiency(limiters[i]);
        if (local_res < res) {
            best_index = i;
            res = local_res;
        }
        if (res <= signed(rows_.size()) / 8) {
            break;
        }
    }

    return limiters[best_index];
}

int KDVertice::calculate_limiter_efficiency(const Limiter& limit) const
{
    int res = 0;
    std::string prop = limit.get_property_name();

    for(std::multiset<const Row*>::iterator it = rows_.begin(); it !=rows_.end(); it++) {
        if (limit.is_value_matching((*it)->get_value(prop))) {
            res++;
        } else {
            res--;
        }
    }

    return abs(res);
}

std::vector<const Row*>  KDVertice::linear_filter(const QueryBoundary& query_boundary) const
{
    std::vector<const Row*> rows;

    for(std::multiset<const Row*>::iterator it = rows_.begin(); it!=rows_.end(); it++) {
        if (query_boundary.get_query_params()->is_matching(*it)) {
            rows.push_back(*it);
        }
    }

    return rows;
}

std::vector<const Row*>  KDVertice::filter_non_index_conditions(const QueryBoundary& query_boundary) const
{
    //TODO I am filtering ALL conditions
    std::vector<const Row*> rows;

    for(std::multiset<const Row*>::iterator it = rows_.begin(); it!=rows_.end(); it++) {
        if (query_boundary.get_query_params()->is_matching(*it)) {
            rows.push_back(*it);
        }
    }

    return rows;
}