#include <iostream>
#include <cstring>
#include <stack>    // for tree.delete_tree()
#include <vector>
#include <set>

#include <functional> // for compare

using namespace std;


template <typename T, class Compare = less<T>> // кринжа с этими class Compare
class tree {
private:
    struct unit {
        unit* prev;
        T     val;
        unit* next[2] = {0, 0};
        int h = 1;
    };

    unit zero = {0};
    Compare cmp;
    size_t quantity = 0;
    
    
public:
    tree() {}
    tree(std::initializer_list<T> const& items) { for(auto e : items) insert(e); }
    tree(const tree& other) : quantity(other.quantity) { recursive_copy(zero.next[0], other.zero.next[0]); }
    tree(tree&& other) { switch_zeros(other); }
    ~tree() { clear(); }

    void recursive_copy(unit*& a, unit* other) {
        if(!other) return;

        a = new unit{a, other->val};
        for(int i = 0; i < 2; i++) recursive_copy(a->next[i], other->next[i]);
    }

    tree& operator = (tree&& other) {
        if (this != other) switch_zeros(other);
        return *this;
    }

    void swap(tree& other) { std::swap(zero, other.zero); }


    // ========== [ ITERATORS ] ==============================
    class ConstIterator  {
    public:
        ConstIterator() : curr(0) {}
        ConstIterator(unit* a) : curr(a) {}

        const T& operator *() const { return curr->val; }
        
        ConstIterator& operator ++() { return udd( 1); }
        ConstIterator& operator --() { return udd( 0); }
        
        bool operator ==(ConstIterator b) { return curr == b.curr; }
        bool operator !=(ConstIterator b) { return curr != b.curr; }
        
        int   hight() { return curr->h; }
        unit* currr() { return curr;    }
        ConstIterator nexxt(bool bit) { return curr->next[bit]; }
    protected:
        ConstIterator& udd (bool up) {
            if( curr->next[up] ) {
                curr = curr->next[up];
                while(curr->next[!up]) curr = curr->next[!up];
                return *this;
            }

            unit* prev = curr->prev;
            while( prev->next[up] == curr ) { curr = prev; prev = curr->prev; }
            curr = prev;
            return *this;
        }
        
        unit* curr;
    };

    class Iterator : public ConstIterator {
    public:
        Iterator(unit* a) { this->curr = a; }
        T& operator *() const { return this->curr->val; }
    };


    // ========== [ SOMETHING ] ==============================
    using value_type = T;
    using       reference =     value_type&;
    using const_reference = const reference;
    using       iterator  =        Iterator;
    using const_iterator  =   ConstIterator;
    using size_type = size_t;

    
    pair<iterator, bool> insert(const T& a) {
        auto [prev, bit] = serch(a);
        if( prev != &zero && prev->val == a ) return {{prev}, 0};
        unit* new_unit  = new unit{prev, a};
        prev->next[bit] = new_unit;
        quantity++;
        insert_balancing(prev, bit);
        return {{new_unit}, 1};
    }

    void erase(iterator pos) {
        unit* prev = pos.currr();
        erase(prev, prev == prev->prev->next[1]);
        delete prev;
        quantity--;
    }
    size_type erase(const T& a) {
        iterator typoe = find(a);
        if(typoe == end()) return 0;
        erase(typoe);
        return 1;
    }
    
    iterator begin() { return extreme_node(&zero, 0); }
    iterator end()   { return &zero; }


    void merge(tree& other) {
        for(auto p = other.begin(); p != other.end(); ++p) insert(*p);
        other.clear();
    }

    iterator find(const T& a) {
        auto prev = serch(a).first;
        return (prev == &zero || prev->val != a) ? end() : prev;
    }
    bool contains(const T& a) { return find(a) != end(); }
    bool empty() { return zero.next[0] != 0; }
    size_type size() { return quantity; }
    size_type max_size() { return std::numeric_limits<size_type>::max() / sizeof(value_type) / 4; }

    void clear() { // deleting tree
        if( !zero.next[0] ) return;
        stack<unit*> pass {{(unit*)zero.next[0]}}; // типо стек с одним элементом zero.next[0]
        do {
            unit* curr = pass.top();
            pass.pop();
            for(int i = 0; i < 2; i++)
                if(curr->next[i]) pass.push(curr->next[i]);
            delete curr;
        }  while( !pass.empty() );

        zero.next[0] = 0;
        quantity = 0;
        zero.h = 1;
    }
  
private:
    void switch_zeros(tree&& other) {
        clear();
        quantity = other.quantity;

        zero.next[0] = other.zero.next[0];
        other.zero.next[0] = 0;

        zero.h = other.zero.h;
        other.zero.h = 1;
    }


    // ========== [ MANIPULATING with node's ] ==============================
    int geth(unit* curr) { return curr ? curr->h : 0; }
    inline int Delta(unit* curr) { return geth(curr->next[0]) - geth(curr->next[1]); }

    unit* extreme_node(unit* start, bool bit) {
        while(start->next[bit]) start = start->next[bit];
        return start;
    }

    inline void comunication(unit* a, bool bit, unit* b) { // служебная функция
        if( (a->next[bit] = b) ) b->prev = a;
    }
    
    inline void rotation(unit* q, int bit) { // bit - какую сторону надо поднимать
        unit* prev = q->prev, *p = q->next[bit];

        comunication(q,  bit, p->next[!bit]);
        comunication(p, !bit, q);
        comunication(prev, prev->next[1] == q, p);
    }
    

    // ========== [ SEARCH ] ==============================
    pair<unit*, bool> serch(T a) {
        unit* prev = &zero;
        bool  bit  = 0;

        while ( prev->next[bit] ) {
            prev = prev->next[bit];
            if( prev->val == a ) break;
            bit = cmp(prev->val, a);
        }
        return {prev, bit};
    }
    

    // ========== [ INSERT ] ==============================
    void balancing(unit* pos, bool bit, int bit_b) { // bit - в какой стороне перевес
        pos->h --;

        if( (!bit && bit_b) || (bit && !bit_b) ) { // двойной поворот
            pos->next[bit]->h --;
            pos->next[bit]->next[!bit]->h ++;
            rotation( pos->next[bit], !bit );
        }

        rotation(pos, bit);
    }

    void insert_balancing(unit* pos, bool bit) {
        int bit_b, newh = 2;
        unit* endd = &zero; // чтоб компилятор не думал
        while( pos != endd && pos->h != newh ) {
            if( newh-1 - geth(pos->next[!bit]) == 2 ) { balancing(pos, bit, bit_b); return; }

            pos->h = newh++;
            bit_b = bit;
            bit   = (pos->prev->next[1] == pos);
            pos =  pos->prev;
        }

        if( pos == endd ) pos->h = newh;
    }


    // ========== [ ERASE  ] ==============================
    void stupid_balancing(unit* pos) {
        while ( 1 ) {
            pos->h = max(geth(pos->next[0]), geth(pos->next[1])) + 1;
            if( pos == &zero ) return;
            
            int delta = Delta(pos);
            if( abs(delta) < 2 ) { pos = pos->prev; continue;  }
            
            int delta_b = Delta(pos->next[delta < 0]);
            
            pos->h -= 1 + (delta_b != 0); // q
            
            if( (delta == 2 && delta_b == -1) || (delta == -2 && delta_b == 1) ) { // двойной поворот
                pos->next[delta < 0]->h--; // p
                pos->next[delta < 0]->next[!(delta < 0)]->h++; // t
                
                rotation( pos->next[delta < 0], !(delta < 0) );
                
            } else pos->next[delta < 0]->h += (delta_b == 0); // p
            
            rotation(pos, (delta < 0) );

            if( !delta_b ) return;
            pos = pos->prev;
        }
    }
    
    void erase(unit* curr, int bit) { // служебная функция
        unit* prev = curr->prev;

        for(int i = 0; i < 2; i++) // если ноль или один ребенок
            if( curr->next[!i] == 0 ) {
                comunication(prev, bit, curr->next[i]);
                stupid_balancing(prev);
                return;
            }

        // два ребенка => перевешивать указатели
        unit* lmx = extreme_node(curr->next[0], 1); // максимум в левом дереве от удаляемого узла

        bool clasic = (lmx != curr->next[0]);
        unit* plmx = clasic ? lmx->prev : lmx;

        comunication(lmx->prev, lmx != curr->next[0], lmx->next[0]);
        comunication(prev, bit, lmx);

        for(int i = 0; i < 2; i++) comunication(lmx, i, curr->next[i]);

        int temp_h = lmx->h;
        lmx->h = curr->h;

        stupid_balancing(plmx);
    }

};

