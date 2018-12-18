/**
 * LRU implementation
 */
#include "buffer/lru_replacer.h"
#include "page/page.h"

namespace cmudb {

template <typename T> LRUReplacer<T>::LRUReplacer() {
  size_ = 0;
  head = new Page();
  tail = new Page();
  head->next = tail;
  tail->prev = head;

}

template <typename T> LRUReplacer<T>::~LRUReplacer() {}

template <typename T> void LRUReplacer<T>::AddToTail(Page * page){
    auto last = tail->prev;
    last->next = page;
    page->next = tail;
    tail->prev = page;
    page->prev = last;
}

template <typename T> void LRUReplacer<T>::EraseFromList(Page * current){
  auto current_prev = current->prev;
  auto current_after = current->next;
  //remove current
  current_prev->next = current_after;
  current_after->prev = current_prev;
}
/*
 * Insert value into LRU
 */
template <typename T> void LRUReplacer<T>::Insert(const T &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  //to see if it is the newly added value
  auto it = buffer.find(value);
  if(it!=buffer.end()){
    auto current = it->second;
    EraseFromList(current);
    //move current to the tail;
    AddToTail(current);
  }
  else{
    Page * temp = new Page(value);
    AddToTail(temp);
    buffer.insert({value,temp});
    size_++;
  }


}

/* If LRU is non-empty, pop the head member from LRU to argument "value", and
 * return true. If LRU is empty, return false
 */
template <typename T> bool LRUReplacer<T>::Victim(T &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  if(size_>0){
    //fetch data
    auto first = head->next;
    value = first->data;

    //erase from table
    buffer.erase(value);
    head->next = first->next;
    first->next->prev = head;
    size_--;
    return true;
  }
  return false;
}

/*
 * Remove value from LRU. If removal is successful, return true, otherwise
 * return false
 */
template <typename T> bool LRUReplacer<T>::Erase(const T &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  auto it = buffer.find(value);
  if(it != buffer.end()){
    auto current = it->second;
    EraseFromList(current);
    buffer.erase(value);
    size_--;
    return true;
  }
  return false;
}

template <typename T> size_t LRUReplacer<T>::Size() {
  std::lock_guard<std::mutex> lock(mutex_);
  return size_;
 }

template class LRUReplacer<Page *>;
// test only
template class LRUReplacer<int>;

} // namespace cmudb
