//===----------------------------------------------------------------------===//
//
//                         BusTub
//
// lru_replacer.cpp
//
// Identification: src/buffer/lru_replacer.cpp
//
// Copyright (c) 2015-2019, Carnegie Mellon University Database Group
//
//===----------------------------------------------------------------------===//

#include "buffer/lru_replacer.h"

namespace bustub {

LRUReplacer::LRUReplacer(size_t num_pages) : size_{num_pages} {}

LRUReplacer::~LRUReplacer() = default;

auto LRUReplacer::Victim(frame_id_t *frame_id) -> bool {
  std::scoped_lock<std::mutex> lock(latch_);
  if (lru_list_.empty()) {
    return false;
  }
  *frame_id = lru_list_.back();
  lru_list_.pop_back();
  lru_map_.erase(*frame_id);
  return true;
}

void LRUReplacer::Pin(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);

  auto iter = lru_map_.find(frame_id);
  if (iter != lru_map_.end()) {
    lru_list_.erase(iter->second);
    lru_map_.erase(frame_id);
  }
}

void LRUReplacer::Unpin(frame_id_t frame_id) {
  std::scoped_lock<std::mutex> lock(latch_);

  auto iter = lru_map_.find(frame_id);
  if (iter == lru_map_.end()) {
    lru_list_.push_front(frame_id);
    lru_map_[frame_id] = lru_list_.begin();
    if (lru_map_.size() > this->size_) {
      frame_id_t last = lru_list_.back();
      lru_list_.pop_back();
      lru_map_.erase(last);
    }
  }
}

auto LRUReplacer::Size() -> size_t {
  std::scoped_lock<std::mutex> lock(latch_);
  return this->lru_list_.size();
}

}  // namespace bustub
