#include <list>

#include "hash/extendible_hash.h"
#include "page/page.h"

namespace cmudb {

/*
 * constructor
 * array_size: fixed array size for each bucket
 */
 template <typename K, typename V>
 ExtendibleHash<K, V>::ExtendibleHash(size_t size):
     bucket_size_(size), bucket_count_(0), depth(0) {
   buckets_.emplace_back(new Bucket(0, 0));
   // initial: 1 bucket
   bucket_count_ = 1;
 }

/*
 * helper function to calculate the hashing address of input key
 */
template <typename K, typename V>
size_t ExtendibleHash<K, V>::HashKey(const K &key) {
  return std::hash<K>()(key);
}

/*
 * helper function to return global depth of hash table
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetGlobalDepth() const {

  std::lock_guard<std::mutex> lock(mutex_);
  return this->depth;
}

/*
 * helper function to return local depth of one specific bucket
 * NOTE: you must implement this function in order to pass test
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetLocalDepth(int bucket_id) const {
  std::lock_guard<std::mutex> lock(mutex_);
  if(buckets_[bucket_id]) {
        return buckets_[bucket_id]->depth;
    }
    return -1;
}

/*
 * helper function to return current number of bucket in hash table
 */
template <typename K, typename V>
int ExtendibleHash<K, V>::GetNumBuckets() const {
  std::lock_guard<std::mutex> lock(mutex_);
  return bucket_count_;
}

/*
 * lookup function to find value associate with input key
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Find(const K &key, V &value) {
  std::lock_guard<std::mutex> lock(mutex_);
  //get hash:
  size_t hash_code = HashKey(key);
  //get bucket
  size_t bucket_position = hash_code & ((1 << depth) - 1);

  //see if key in the map
  bool res = false;
  if(buckets_[bucket_position]) {
        if(buckets_[bucket_position]->items.find(key) != buckets_[bucket_position]->items.end()) {
            value = buckets_[bucket_position]->items[key];
            res = true;
        }
    }
  return res;
}

/*
 * delete <key,value> entry in hash table
 * Shrink & Combination is not required for this project
 */
template <typename K, typename V>
bool ExtendibleHash<K, V>::Remove(const K &key) {
  std::lock_guard<std::mutex> lock(mutex_);
  //get hash:
  size_t hash_code = HashKey(key);
  //get bucket
  size_t bucket_position = hash_code & ((1 << depth) - 1);

  auto cnt = 0;

  if(buckets_[bucket_position]) {
        cnt = buckets_[bucket_position]->items.erase(key);
    }
  return cnt != 0;
}

/*
 * insert <key,value> entry in hash table
 * Split & Redistribute bucket when there is overflow and if necessary increase
 * global depth
 */
template <typename K, typename V>
void ExtendibleHash<K, V>::Insert(const K &key, const V &value) {

std::lock_guard<std::mutex> lock(mutex_);
size_t hash_code = HashKey(key);
//get bucket
size_t bucket_position = hash_code & ((1 << depth) - 1);

if (buckets_[bucket_position] == nullptr) {
    buckets_[bucket_position] = std::make_shared<Bucket>(bucket_position, depth);
    ++bucket_count_;
  }

auto bucket = buckets_[bucket_position];

//case 1: key found so we update the value

if(bucket->items.find(key)!=bucket->items.end()){
  bucket->items[key] = value;
  return;
}

bucket->items.insert({key,value});

//case for Split
if(bucket->items.size()>bucket_size_){
  auto old_index = bucket->id;
  auto old_depth = bucket->depth;
  auto new_bucket = split(bucket);

  if(new_bucket == nullptr) {
      bucket->depth = old_depth;
      return;
  }

  //case for increase the global_depth
  if(new_bucket->depth>depth){

    size_t size = buckets_.size();
    size_t new_size = 1<<(bucket->depth);

    depth = bucket->depth;
    buckets_.resize(new_size);

    for(size_t i = 0; i < size; i++){
      if(buckets_[i] != bucket){
        for(size_t j = i + size; j< new_size; j += size){
            buckets_[j] =  buckets_[i];
        }
      }
    }
  }
  // update to right index: for split point
   if (bucket->id != old_index) {
     buckets_[old_index].reset();
     buckets_[bucket->id] = bucket;
   }
  buckets_[new_bucket->id] = new_bucket;


}
// std::cout<<"insert: "<<key<<std::endl;
// std::cout<<"depth: "<<depth<<std::endl;
// for(size_t i = 0; i <buckets_.size();i++){
//   if(!buckets_[i]){
//     continue;
//   }
//   std::cout<<"id: "<<buckets_[i]->id<<std::endl;
//   std::cout<<"size: "<<buckets_[i]->items.size()<<std::endl;
// }
// std::cout<<"+++++++++++++++"<<depth<<std::endl;


}


/*
 * helper function to split a bucket when is full, overflow if necessary
 */
template <typename K, typename V>
std::shared_ptr<typename ExtendibleHash<K, V>::Bucket>
ExtendibleHash<K, V>::split(std::shared_ptr<Bucket> &b){
  auto res = std::make_shared<Bucket>(0, b->depth);
  while(res->items.size()==0){
    res->depth++;
    b->depth++;

    for (auto it = b->items.begin(); it != b->items.end();) {
      auto key = it->first;
      auto hash_key = HashKey(key);
      auto new_pos = hash_key & (1<<(res->depth-1));
      if(new_pos){
        res->items.insert(*it);
        res->id = hash_key & ((1 << b->depth) - 1);
        it = b->items.erase(it);
      }else{
        it++;
      }

    }
    if(b->items.empty()) {
         b->items.swap(res->items);
         b->id = res->id;
     }
  }
  bucket_count_++;
  return res;
}


template class ExtendibleHash<page_id_t, Page *>;
template class ExtendibleHash<Page *, std::list<Page *>::iterator>;
// test purpose
template class ExtendibleHash<int, std::string>;
template class ExtendibleHash<int, std::list<int>::iterator>;
template class ExtendibleHash<int, int>;
} // namespace cmudb
