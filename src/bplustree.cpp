#include "bplustree.h"

#include <cassert>
#include <iostream>

#include "bplustree/page.h"
#include "serialize.h"

using std::cout;
using std::endl;

ostream& operator<<(ostream& os, const BPlusTreeIndexMeta& meta) {
  os << fmt::format("[root={},leaf={},free={},crc={}]", meta.root, meta.leaf,
                    meta.free, meta.crc);
  return os;
}

BPlusTreeIndex::BPlusTreeIndex(const string& index_file_path) {
  // 打开索引文件
  bool is_init = File::exist(index_file_path);
  file_ = new File(index_file_path);
  assert(file_->is_open());

  // 加载索引的元数据
  index_meta_ = new BPlusTreeIndexMeta{0, 0, 0, 0};
  if (!is_init) {
    file_->write(0, index_meta_, sizeof(BPlusTreeIndexMeta));
  } else {
    file_->read(0, index_meta_, sizeof(BPlusTreeIndexMeta));
  }
  cout << (*index_meta_) << endl;
}

BPlusTreeIndex::~BPlusTreeIndex() {
  if (index_meta_ != nullptr) {
    file_->write(0, index_meta_, sizeof(BPlusTreeIndexMeta));
    delete index_meta_;
    delete file_;
  }
}

/**
 * @brief 根据键值定位叶子节点的地址
 *
 * @param key 键
 * @param compare 比较器
 * @return address_t 叶子节点地址
 */
address_t BPlusTreeIndex::locate_leaf_node(string_view key,
                                           const Compare& compare) {
  char* buffer = new char[PAGE_SIZE];
  file_->read(index_meta_->root, buffer, PAGE_SIZE);
  auto page = reinterpret_cast<Page*>(buffer);

  address_t target_node_address = index_meta_->root;
  while (page->meta()->page_type == PageType::kInternalPage) {
    uint16_t offset = page->LowerBound(key, compare);
    auto meta = page->get_arribute<RecordMeta>(offset);
    target_node_address = *(page->get_arribute<address_t>(
        offset + sizeof(RecordMeta) + meta->key_len));
    file_->read(target_node_address, buffer, PAGE_SIZE);
  }

  delete[] buffer;
  return target_node_address;
}

bool BPlusTreeIndex::insert(string_view key, string_view val,
                            const Compare& compare) {
  address_t leaf_node_address = this->locate_leaf_node(key, compare);

  // insert_leaf_node(leaf_node_address, key, val, compare);
  return true;
}


optional<address_t> BPlusTreeIndex::InsertLeafNode(address_t page_address, string_view key, string_view val, const Compare& compare) {
  SharedBuffer<LeafPage> page(PAGE_SIZE);
  file_->read(page_address, page.buffer(), PAGE_SIZE);
  if (!page->full()) {
    page->Insert(key, val, compare);
  } else {
    auto [mid_key, other_page] = page->SplitPage();
    this->InsertInternalNode(page->meta()->parent, key, page->meta()->self, other_page->meta()->self, compare);
  }                                                   
}

optional<address_t> BPlusTreeIndex::InsertInternalNode(address_t page_address, string_view key, address_t left_child, address_t right_child, const Compare& compare) {
  SharedBuffer<InternalPage> page(PAGE_SIZE);
  
  if (page_address != 0) {
    file_->read(page_address, page.buffer(), PAGE_SIZE);
  } else {
    page->meta()->self = file_->alloc(PAGE_SIZE);
    index_meta_->root = page->meta()->self;
  }
  if (!page->full()) {
    vector<string_view> vals;
    vals.push_back(Serializer<address_t>::serialize(left_child));
    vals.push_back(Serializer<address_t>::serialize(right_child));
    page->Insert(key, vals, compare);
    return page_address;
  } else {
    auto [mid_key, other_page] = page->SplitPage();
    other_page->meta()->self = file_->alloc(PAGE_SIZE);
    this->InsertInternalNode(page->meta()->parent, key, page->meta()->self, other_page->meta()->self, compare);
    file_->write(other_page->meta()->self, other_page.buffer(), PAGE_SIZE);
  }
  file_->write(page->meta()->self, page.buffer(), PAGE_SIZE);
}

optional<address_t> BPlusTreeIndex::erase(string_view key) {
  return std::nullopt;
}
