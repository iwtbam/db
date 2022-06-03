#pragma once

#include <fmt/format.h>

#include <iostream>
#include <optional>
#include <string_view>

#include "basetype.h"
#include "file.h"

using std::optional;
using std::ostream;
using std::string_view;

class File;

struct BPlusTreeIndexMeta {
  uint32_t root;
  uint32_t leaf;
  uint32_t free;
  uint32_t crc;
};

ostream& operator<<(ostream& os, const BPlusTreeIndexMeta& meta);

class LeafPage;
class InternalPage;

class BPlusTreeIndex {
 public:
  BPlusTreeIndex(const string& index_file_path);
  ~BPlusTreeIndex();

 public:
  bool insert(string_view key, string_view val, const Compare& compare);
  optional<address_t> erase(string_view key);

 private:
  address_t locate_leaf_node(string_view key, const Compare& compare);

  optional<address_t> InsertLeafNode(address_t page_address, string_view key,
                                     string_view val, const Compare& compare);

  optional<address_t> InsertInternalNode(address_t page_address,
                                         string_view key, address_t left_child,
                                         address_t right_child,
                                         const Compare& compare);

 private:
  File* file_;
  BPlusTreeIndexMeta* index_meta_;
};

// class BPlusTree {

// public:
//     static const size_t NOT_EXIST = 0;
// public:
//     BPlusTree(PageManager* page_manager, CompareFunction cmp);
//     void insert(const Element* key, const Element* val);
//     optional<address_t> erase(const Element* key);

// private:
//     optional<size_t> insert_leaf_node(address_t node_address, const Element*
//     key, const Element* val); optional<size_t> insert_internal_node(address_t
//     node_address, const Element* key, const Element* val);

// private:
//     bool insert_kv_into_leaf_node(PageHeader* header, RecordMeta* meta, const
//     Element* key, const Element* val);

// private:
//     address_t locate_leaf_node(const Element* key);
//     RecordMeta* locate_key(const Element* key, char* page);
//     RecordMeta* lower(const void *key, void* buffer);

// private:
//     address_t root;
//     PageManager* page_manager_;
//     CompareFunction cmp_;
// };