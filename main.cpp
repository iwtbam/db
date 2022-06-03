#include <ctime>
#include <iostream>
#include <string_view>

#include "bplustree.h"
#include "bufferpool.h"
#include "file.h"
#include "fmt/format.h"
#include "hello.h"
#include "bplustree/page.h"
#include "reflection.h"
#include "memory/buffer.h"
#include "serialize.h"
#include <type_traits>
#include <string>

using std::cout;
using std::endl;
using std::is_base_of_v;
using std::to_string;

struct Test {
    int a;
    int b;
};

string_view GetKey(LeafPage* page, uint16_t record_address) {
    RecordMeta* meta = page->get_arribute<RecordMeta>(record_address);
    string_view key = {page->base_address() + record_address + sizeof(RecordMeta), meta->key_len};
    cout << (*meta) << " " << key << endl;
    return key;
}

int main() { 

    // string data = Serializer<uint16_t>::serialize(10299);
    // cout << data.size() << endl;
    // cout << Serializer<uint16_t>::deserialize(data) << endl;


    auto cmp = [](string_view v1, string_view v2) -> int {
        if (v1 == v2) {
            return 0;
        }
        return v1 < v2 ? 1 : -1;
    };

    LeafPage* page_ = new LeafPage();
    for (int i = 0; i < 10; i++) {
      string key = "key_test";
      page_->Insert(key, "val" + to_string(i), cmp);
    }

    page_->scan_use();

    // page_->scan_slots();
    // GetKey(page_, page_->FloorSearch("key_test", cmp));
    // cout << page_->LocateSlot("key_test" + to_string(48), cmp) << endl;

    return 0; 
}
