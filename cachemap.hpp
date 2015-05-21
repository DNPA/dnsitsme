#ifndef _PBDNS_CACHE_MAP_HPP
#define _PBDNS_CACHE_MAP_HPP
#include <map>
#include <deque>
/* The cachemap is a simple and non optimized implemtation of a chaching map.
   A chachemap will hold only the N most recently accessed entities and will drop older
   entities that it considders as having expired. */
template <typename K,typename V>
class cachemap {
    std::map<K,V> mMap; //The real map.
    std::deque<K> mDeque; //A deque used so we know what we need to delete from the map.
    std::map<K,size_t> mDoubles; //An other map used to avoid deleting map entities that are pressent more than once in the deque.
    size_t mMaxSize; //Maximum number of entities in the map.
  public:
    //Helper class to allow square braced notation to be used both as rval and as lval.
    class entity {
         cachemap<K,V> *mCmap;
         K mKey;
       public:
         entity(cachemap<K,V> *cmap,K &key):mCmap(cmap),mKey(key){
         }
         ~entity() {
         }
         //Cast operator to allow usage of square braced notation as rval.
         //  int x = foomap["bla"];
         operator V() {
           return mCmap->mMap[mKey];
         };
         //Assignment operator for usage of square braced notation as lval.
         //   foomap["bla"] = 17;
         V  operator=(V val) {
           //Make sure we keep track of how many times we overwrite map[$key] so we don't delete vallues prematurely.
           if (mCmap->mMap.find(mKey) == mCmap->mMap.end()) {
              mCmap->mDoubles[mKey] = 1;
           } else  {
              mCmap->mDoubles[mKey] += 1;
           }
           //Set the new vallue for our key.
           mCmap->mMap[mKey] = val;
           //Keep a list of what was put into the map in what order.
           mCmap->mDeque.push_back(mKey);
           //Check if we may need to forget something.
           if (mCmap->mDeque.size() > mCmap->mMaxSize) {
              //Fetch the oldest entry from the dequeu
              K delkey=mCmap->mDeque.front();
              mCmap->mDoubles[delkey] -= 1;
              //Check if there isn't a newer value with this key in the map that should prohibit us from deleting the entry.
              if (mCmap->mDoubles[delkey] == 0) {
                 //Forget a map entry
                 mCmap->mDoubles.erase(delkey);
                 mCmap->mMap.erase(delkey);
              }
              //Drop the oldest key in the deque
              mCmap->mDeque.pop_front();
           }
           return val;
         }
    };
    cachemap(size_t maxsize): mMaxSize(maxsize){
    }
    ~cachemap(){
    }
    //Square braces operator returns a helper object as to be usable both as rval and as lval.
    entity operator[](K key){
       return entity(this,key); 
    };
    //Helper method to check if a key is still there.
    bool hasKey(K key) {
       return (mMap.find(key) != mMap.end()); 
    };
};
#endif
