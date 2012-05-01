#ifndef KDVERTICE_H
#define KDVERTICE_H

#include "boundary.hpp"
#include "query_boundary.hpp"
#include "../table_index/table_index_info.hpp"
#include "../../dump/dump_saver.hpp"

#include <set>

class KDVertice
{
    public:
        explicit KDVertice(const TableIndexInfo* table_index_info, Boundary boundary);
        explicit KDVertice(const TableIndexInfo* table_index_info);

        void add_collection(std::vector<const Row*> rows);
        bool insert_row(const Row* row);
        bool delete_row(const Row* row);
        std::vector<const Row*> search(const QueryBoundary& boundary) const;
        void clear();

        bool contains_row(const Row* row) const ;
        void dump_all_rows(DumpSaver& dump_saver) const;

        virtual ~KDVertice();
    protected:
        void rebuild();
        std::vector<const Row*> linear_filter(const QueryBoundary& boundary) const;
    private:

        const TableIndexInfo* table_index_info_;

        // boundary of this vertice
        Boundary boundary_;

        // elements that are in this boundary
        std::set<const Row*> rows_;

        // left and right subtree and divider
        int divieder_property_number_;
        Type* divider_;
        KDVertice* left_;
        KDVertice* right_;
};

#endif // KDVERTICE_H
