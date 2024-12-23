@0xabcdefabcdefabcdef;

struct KeyValuePair {
  key @0 :UInt64;
  values @1 :List(Int32);
}

struct Map {
  entries @0 :List(KeyValuePair);
}
