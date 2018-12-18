/**
 * lru_replacer.h
 *
 * Functionality: The buffer pool manager must maintain a LRU list to collect
 * all the pages that are unpinned and ready to be swapped. The simplest way to
 * implement LRU is a FIFO queue, but remember to dequeue or enqueue pages when
 * a page changes from unpinned to pinned, or vice-versa.
 */

#pragma once
#include <unordered_map>
#include "buffer/replacer.h"
#include "hash/extendible_hash.h"

namespace cmudb {

template <typename T> class LRUReplacer : public Replacer<T> {
  struct Page{
    Page() = default;
    explicit Page(T data): data(data){};
    T data;
    Page* prev = nullptr;
    Page* next = nullptr;
  };
public:
  // do not change public interface
  LRUReplacer();

  ~LRUReplacer();

  void Insert(const T &value);

  bool Victim(T &value);

  bool Erase(const T &value);

  size_t Size();

private:

  void AddToTail(Page * page);
  void EraseFromList(Page * page);
  // add your member variables here
  mutable std::mutex mutex_;
  size_t size_;
  std::unordered_map<T,Page *> buffer;
  Page* head;
  Page* tail;

};

} // namespace cmudb
