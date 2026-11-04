/**
 * A5Header.h
 * @author kisslune 
 */

#ifndef ANSWERS_A5HEADER_H
#define ANSWERS_A5HEADER_H

#include "SVF-LLVM/SVFIRBuilder.h"

/// Point-to set
using PTS = std::map<unsigned, std::set<unsigned>>;

/**
 * FIFO worklist
 */
template<class T>
class WorkList
{
public:
    /// Check whether the worklist is empty.
    inline bool empty() const
    { return data_list.empty(); }

    /// Clear the worklist
    inline void clear()
    {
        data_list.clear();
        data_set.clear();
    }

    /// Push a data into the END work list.
    inline bool push(const T &data)
    {
        if (this->data_set.find(data) == data_set.end())
        {
            this->data_list.push_back(data);
            this->data_set.insert(data);
            return true;
        }
        else
            return false;
    }

    /// Pop a data from the FRONT of work list.
    inline T pop()
    {
        assert(!this->empty() && "work list is empty");
        T data = this->data_list.front();
        this->data_list.pop_front();
        this->data_set.erase(data);
        return data;
    }

protected:
    std::unordered_set<T> data_set;       ///< to avoid duplicate elements
    std::deque<T> data_list;     ///< to access the elements at both the beginning and the end
};


/// The Andersen solver
class Andersen
{
public:
    explicit Andersen(SVF::ConstraintGraph *consg) :
            consg(consg)
    {}

    /// Run pointer analysis
    void runPointerAnalysis();
    /// Dump results into a file
    void dumpResult();

protected:
    SVF::ConstraintGraph *consg;
    PTS pts;
};


#endif //ANSWERS_A5HEADER_H
