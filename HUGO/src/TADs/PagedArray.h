#pragma once
#include <vector>
#include <array>
#include <stdexcept>
#include <iterator>

/**
 * @tparam T Type of elements
 * @tparam PageSize Number of elements per page
 */
template<typename T, std::size_t PageSize>
class PagedArray{
    std::size_t totalSize = 0;
    std::vector<std::array<T,PageSize>> pages;

    void checkRange(std::size_t index) const{
        if(index >= totalSize){
            throw  std::out_of_range("invalid index");
        } 
    }
public:    
    PagedArray() = default;
    ~PagedArray() = default;

    /**
     * @brief Add element to the end of the array
     * @param element Element to add
     * @return Index where the element was stored
     */
    std::size_t push_back(const T& element){
        if(pages.empty() || totalSize == pages.size() * PageSize){
                pages.emplace_back(); //add new page
        }
        pages[totalSize/PageSize][totalSize%PageSize] = element;
        return totalSize++;
    }
    std::size_t push_back(T&& element){
        if(pages.empty() || totalSize == pages.size() * PageSize){
                pages.emplace_back(); //add new page
        }
        pages[totalSize/PageSize][totalSize%PageSize] = std::move(element);
        return totalSize++;
    }

    /**
     * Remove an element at index by swapping with the last element
     * @param index The index to remove
     */
    void remove(std::size_t index){
        checkRange(index);
        std::size_t lastIndex = totalSize - 1;
        if (index != lastIndex) {
            //swap with last element
            pages[index/PageSize][index%PageSize] = std::move(pages[lastIndex/PageSize][lastIndex%PageSize]);
        }
        --totalSize;
        
        //free last page if it becomes empty
        if (totalSize % PageSize == 0 && !pages.empty()) {
            pages.pop_back();
        }
    }

    //------access---------------
    T& operator[] (std::size_t index){
        checkRange(index);
        return pages[index/PageSize][index%PageSize];
    }
    const T& operator[] (std::size_t index) const{
        checkRange(index);
        return pages[index/PageSize][index%PageSize];
    }
    T& at(std::size_t index){
        checkRange(index);
        return pages[index/PageSize][index%PageSize];
    }
    const T& at(std::size_t index) const {
        checkRange(index);
        return pages[index/PageSize][index%PageSize];
    }

    //----SizeInfo--------------------
    std::size_t size() const {return totalSize;}
    std::size_t pageSize() const {return PageSize;}
    bool empty() const {return totalSize == 0;}

    void clear() {
        pages.clear();
        totalSize = 0;
    }

    //---------ITERATOR-------------------------------
    /**
     * Iterator implementation
     * @tparam U Type of elements (T or const T)
     * @tparam Array The array type (PagedArray or const PagedArray)
     */
    template <typename U, typename Array> 
    class Iterator{
    private:
        Array* array;
        std::size_t index;
    public:
        //Definitions for compatibility with algorithms
        using iterator_category = std::forward_iterator_tag;
        using difference_type = std::ptrdiff_t;
        using value_type = std::remove_const_t<U>; //type of elements (without const)
        using pointer = U*;
        using reference = U&;

        Iterator(Array* arr, std::size_t idx)
            :array(arr), index(idx){}
        
        //-----operators----------
        U& operator*() {return (*array)[index];}
        U* operator->() { return &(*array)[index];}
        Iterator& operator++() { ++index; return *this;}
        Iterator operator++(int) {
            Iterator tmp = *this;
            ++index;
            return tmp;
        }

        bool operator==(const Iterator& other) const {
            return index == other.index && array == other.array;
        }
        bool operator!=(const Iterator& other) const {
            return !(*this==other);
        }
    };  
    //Iterator type alias
    using iterator = Iterator<T, PagedArray<T,PageSize>>;
    using constIterator = Iterator<const T, const PagedArray<T,PageSize>>;
    
    iterator begin() { return iterator(this, 0);}
    iterator end() {return iterator(this, totalSize);}

    constIterator begin() const { return constIterator(this, 0); }
    constIterator end() const { return constIterator(this, totalSize); }

    constIterator cbegin() const { return constIterator(this, 0);}
    constIterator cend() const {return constIterator(this, totalSize);}
};
