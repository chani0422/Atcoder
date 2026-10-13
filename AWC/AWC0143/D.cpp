
#if !__INCLUDE_LEVEL__
#include __FILE__

//解答コードここから
//TreapVector使用例
/*
特徴：
    ・任意位置への挿入を期待 O(log N) で行える
    ・任意位置の削除を期待 O(log N) で行える
    ・添字アクセスは期待 O(log N)
    ・0-indexed
使い方：
    ・TreapVector<ll> v;
    ・要素アクセス　v[i]　期待
    ・v.front()、
    ・v.back()
    ・v.push_back(x)
    ・v.push_front(x)
    ・v.insert(pos, x)　pos 番目に x を挿入
    ・v.erase(pos)　pos 番目を削除
    ・v.pop_back()
    ・v.pop_front()
    ・v.size()
    ・v.empty()
    ・v.clear()
    ・v.begin()
    ・v.end()
    ・v.sort()
    ・v.revsort()
*/

template<class T>
class TreapVector{
	struct Node{
		T val;
		int l=-1,r=-1,sz=1;
		uint32_t pri;
		template<class U>
		Node(U&& val,uint32_t pri):val(forward<U>(val)),pri(pri){}
	};

	vector<Node> tr;
	vector<int> free_id;
	int root=-1;

	uint64_t rnd=chrono::steady_clock::now().time_since_epoch().count();

	uint32_t rng(){
		rnd^=rnd<<7;
		rnd^=rnd>>9;
		return (uint32_t)rnd;
	}

	int sz(int t) const{
		return t==-1 ? 0 : tr[t].sz;
	}

	void pull(int t){
		tr[t].sz=1+sz(tr[t].l)+sz(tr[t].r);
	}

	bool higher(int a,int b) const{
		if(tr[a].pri!=tr[b].pri)return tr[a].pri>tr[b].pri;
		return a>b;
	}

	// 先頭 k 個 / それ以降 に分割
	void split(int t,int k,int& a,int& b){
		if(t==-1){
			a=b=-1;
			return;
		}

		if(sz(tr[t].l)>=k){
			split(tr[t].l,k,a,tr[t].l);
			pull(t);
			b=t;
		}else{
			split(tr[t].r,k-sz(tr[t].l)-1,tr[t].r,b);
			pull(t);
			a=t;
		}
	}

	int merge(int a,int b){
		if(a==-1) return b;
		if(b==-1) return a;

		if(higher(a,b)){
			tr[a].r=merge(tr[a].r,b);
			pull(a);
			return a;
		}else{
			tr[b].l=merge(a,tr[b].l);
			pull(b);
			return b;
		}
	}

	template<class U>
	int new_node(U&& val){
		uint32_t pri=rng();

		if(free_id.empty()){
			tr.emplace_back(forward<U>(val),pri);
			return (int)tr.size()-1;
		}

		int id=free_id.back();
		free_id.pop_back();

		tr[id].val=forward<U>(val);
		tr[id].l=tr[id].r=-1;
		tr[id].sz=1;
		tr[id].pri=pri;

		return id;
	}

	// pos に x を挿入
	int insert_node(int t,int pos,int x){
		if(t==-1) return x;

		if(higher(x,t)){
			split(t,pos,tr[x].l,tr[x].r);
			pull(x);
			return x;
		}

		int ls=sz(tr[t].l);

		if(pos<=ls){
			tr[t].l=insert_node(tr[t].l,pos,x);
		}else{
			tr[t].r=insert_node(tr[t].r,pos-ls-1,x);
		}
		pull(t);
		return t;
	}

	// pos 番目を削除
	int erase_node(int t,int pos,int& erased){
		int ls=sz(tr[t].l);

		if(pos<ls){
			tr[t].l=erase_node(tr[t].l,pos,erased);
			pull(t);
			return t;
		}

		if(pos>ls){
			tr[t].r=erase_node(tr[t].r,pos-ls-1,erased);
			pull(t);
			return t;
		}

		erased=t;
		return merge(tr[t].l,tr[t].r);
	}

	void recycle(int t){
		tr[t].l=tr[t].r=-1;
		tr[t].sz=1;
		free_id.push_back(t);
	}

	int kth(int k) const{
		int t=root;
		while(1){
			int ls=sz(tr[t].l);
			if(k<ls){
				t=tr[t].l;
			}else if(k==ls){
				return t;
			}else{
				k-=ls+1;
				t=tr[t].r;
			}
		}
	}

	void move_out(int t,vector<T>& v){
		if(t==-1) return;
		move_out(tr[t].l,v);
		v.push_back(move(tr[t].val));
		move_out(tr[t].r,v);
	}

	void move_in(int t,vector<T>& v,int& p){
		if(t==-1) return;
		move_in(tr[t].l,v,p);
		tr[t].val=move(v[p++]);
		move_in(tr[t].r,v,p);
	}

public:

	using value_type=T;
	using reference=T&;
	using const_reference=const T&;
	using size_type=int;
	using difference_type=ptrdiff_t;

	template<bool Const>
	class Iter{
		using Owner=conditional_t<Const,const TreapVector,TreapVector>;

		Owner* owner=nullptr;

		// root -> 現在ノード
		vector<int> path;

		int pos=0;

		void go_left(int t){
			while(t!=-1){
				path.push_back(t);
				t=owner->tr[t].l;
			}
		}

		void go_right(int t){
			while(t!=-1){
				path.push_back(t);
				t=owner->tr[t].r;
			}
		}

		void set_pos(int p){
			pos=p;
			path.clear();

			if(p==owner->size()) return;

			int t=owner->root;
			int k=p;

			while(1){
				path.push_back(t);

				int ls=owner->sz(owner->tr[t].l);

				if(k<ls){
					t=owner->tr[t].l;
				}else if(k==ls){
					return;
				}else{
					k-=ls+1;
					t=owner->tr[t].r;
				}
			}
		}

		Iter(Owner* owner,int pos):owner(owner){
			set_pos(pos);
		}

		friend class TreapVector;
		template<bool> friend class Iter;

	public:
		using iterator_category=bidirectional_iterator_tag;
		using iterator_concept=bidirectional_iterator_tag;
		using value_type=T;
		using difference_type=ptrdiff_t;

		using reference=conditional_t<Const,const T&,T&>;

		using pointer=conditional_t<Const,const T*,T*>;

		Iter()=default;

		template<bool C=Const,enable_if_t<C,int> =0>
		Iter(const Iter<false>& x):owner(x.owner),path(x.path),pos(x.pos){}

		reference operator*() const{
			return owner->tr[path.back()].val;
		}

		pointer operator->() const{
			return &**this;
		}

		Iter& operator++(){
			int cur=path.back();
			if(owner->tr[cur].r!=-1){
				go_left(owner->tr[cur].r);
			}else{
				int child=cur;
				path.pop_back();
				while(!path.empty()&& owner->tr[path.back()].r==child){
					child=path.back();
					path.pop_back();
				}
			}
			++pos;
			return *this;
		}

		Iter operator++(int){
			auto ret=*this;
			++*this;
			return ret;
		}

		Iter& operator--(){
			if(pos==owner->size()){
				go_right(owner->root);
			}else{
				int cur=path.back();
				if(owner->tr[cur].l!=-1){
					go_right(owner->tr[cur].l);
				}else{
					int child=cur;
					path.pop_back();
					while(!path.empty()&& owner->tr[path.back()].l==child){
						child=path.back();
						path.pop_back();
					}
				}
			}
			--pos;
			return *this;
		}

		Iter operator--(int){
			auto ret=*this;
			--*this;
			return ret;
		}

		// begin()+k は使えるが O(log N)
		Iter& operator+=(difference_type d){
			set_pos(pos+(int)d);
			return *this;
		}

		Iter& operator-=(difference_type d){
			return *this+=-d;
		}

		friend Iter operator+(Iter it,difference_type d){
			return it+=d;
		}

		friend Iter operator+(difference_type d,Iter it){
			return it+=d;
		}

		friend Iter operator-(Iter it,difference_type d){
			return it-=d;
		}

		friend difference_type operator-(const Iter& a,const Iter& b){
			return a.pos-b.pos;
		}

		reference operator[](difference_type d) const{
			return *(*this+d);
		}

		friend bool operator==(const Iter& a,const Iter& b){
			return a.owner==b.owner && a.pos==b.pos;
		}

		friend bool operator!=(const Iter& a,const Iter& b){
			return !(a==b);
		}

		friend bool operator<(const Iter& a,const Iter& b){
			return a.pos<b.pos;
		}

		friend bool operator>(const Iter& a,const Iter& b){
			return b<a;
		}

		friend bool operator<=(const Iter& a,const Iter& b){
			return !(b<a);
		}

		friend bool operator>=(const Iter& a,const Iter& b){
			return !(a<b);
		}
	};

	using iterator=Iter<false>;
	using const_iterator=Iter<true>;
	using reverse_iterator=std::reverse_iterator<iterator>;
	using const_reverse_iterator=std::reverse_iterator<const_iterator>;

	TreapVector()=default;

	TreapVector(initializer_list<T> init){
		reserve(init.size());
		for(const auto& x:init){
			push_back(x);
		}
	}

	int size() const{
		return sz(root);
	}

	bool empty() const{
		return root==-1;
	}

	int capacity() const{
		return (int)tr.capacity();
	}

	void reserve(int n){
		tr.reserve(n);
		free_id.reserve(n);
	}

	void clear(){
		tr.clear();
		free_id.clear();
		root=-1;
	}


	T& operator[](int i){
		return tr[kth(i)].val;
	}

	const T& operator[](int i) const{
		return tr[kth(i)].val;
	}

	T& at(int i){
		if(i<0 || i>=size())throw out_of_range("TreapVector::at");
		return (*this)[i];
	}

	const T& at(int i) const{
		if(i<0 || i>=size())throw out_of_range("TreapVector::at");
		return (*this)[i];
	}

	T& front(){
		return (*this)[0];
	}

	const T& front() const{
		return (*this)[0];
	}

	T& back(){
		return (*this)[size()-1];
	}

	const T& back() const{
		return (*this)[size()-1];
	}

	template<class U>
	iterator insert(int pos,U&& val){
		int x=new_node(forward<U>(val));
		root=insert_node(root,pos,x);
		return iterator(this,pos);
	}

	template<class U>
	iterator insert(const_iterator pos,U&& val){
		return insert(pos.pos,forward<U>(val));
	}

	template<class... Args>
	iterator emplace(int pos,Args&&... args){
		return insert(pos,T(forward<Args>(args)...));
	}

	template<class... Args>
	iterator emplace(const_iterator pos,Args&&... args){
		return emplace(pos.pos,forward<Args>(args)...);
	}


	void push_back(const T& x){
		int t=new_node(x);
		root=merge(root,t);
	}

	void push_back(T&& x){
		int t=new_node(move(x));
		root=merge(root,t);
	}

	template<class... Args>
	void emplace_back(Args&&... args){
		push_back(T(forward<Args>(args)...));
	}


	void push_front(const T& x){
		int t=new_node(x);
		root=merge(t,root);
	}

	void push_front(T&& x){
		int t=new_node(move(x));
		root=merge(t,root);
	}

	template<class... Args>
	void emplace_front(Args&&... args){
		push_front(T(forward<Args>(args)...));
	}


	iterator erase(int pos){
		int erased;
		root=erase_node(root,pos,erased);
		recycle(erased);
		return iterator(this,pos);
	}

	iterator erase(const_iterator pos){
		return erase(pos.pos);
	}

	void pop_back(){
		int erased;
		root=erase_node(root,size()-1,erased);
		recycle(erased);
	}

	void pop_front(){
		int erased;
		root=erase_node(root,0,erased);
		recycle(erased);
	}

	iterator begin(){
		return iterator(this,0);
	}

	iterator end(){
		return iterator(this,size());
	}

	const_iterator begin() const{
		return const_iterator(this,0);
	}

	const_iterator end() const{
		return const_iterator(this,size());
	}

	const_iterator cbegin() const{
		return const_iterator(this,0);
	}

	const_iterator cend() const{
		return const_iterator(this,size());
	}


	reverse_iterator rbegin(){
		return reverse_iterator(end());
	}

	reverse_iterator rend(){
		return reverse_iterator(begin());
	}

	const_reverse_iterator rbegin() const{
		return const_reverse_iterator(end());
	}

	const_reverse_iterator rend() const{
		return const_reverse_iterator(begin());
	}

	const_reverse_iterator crbegin() const{
		return const_reverse_iterator(cend());
	}

	const_reverse_iterator crend() const{
		return const_reverse_iterator(cbegin());
	}

	// ソート済みのとき、x以上となる最初の位置を返す
template<class Compare = less<T>>
int lower_bound_pos(const T& x, Compare comp = Compare{}) const {
    int t = root;
    int offset = 0;
    int ans = size();

    while(t != -1) {
        int leftSize = sz(tr[t].l);

        // tr[t].val >= x
        if(!comp(tr[t].val, x)) {
            ans = offset + leftSize;
            t = tr[t].l;
        } else {
            // tr[t].val < x
            offset += leftSize + 1;
            t = tr[t].r;
        }
    }

    return ans;
}

	template<class Compare=less<T>>
	void sort(Compare comp=Compare{}){
		vector<T> v;
		v.reserve(size());

		move_out(root,v);

		std::sort(v.begin(),v.end(),comp);

		int p=0;
		move_in(root,v,p);
	}

	template<class Compare=less<T>>
	void revsort(Compare comp=Compare{}){
		vector<T> v;
		v.reserve(size());

		move_out(root,v);

		std::sort(v.begin(),v.end(),[&](const T& a,const T& b){return comp(b,a);});

		int p=0;
		move_in(root,v,p);
	}
};



//問題
int main() {
    ios::sync_with_stdio(false);cin.tie(nullptr);
    ll n,Q; cin >> n >> Q;
    TreapVector<pair<ll,ll>> S;
    vector<ll> score(n);
    rep(i,0,n) {
        ll a; cin >> a;
        score[i] = a;
        S.pb({a,i});
    }
    // sort(nall(S));
    S.sort();
    rep(i,0,Q) {
        ll t,v; cin >> t >> v;
        t--;
        pair<ll,ll> old = {score[t],t};
        auto it = S.lower_bound_pos(old);
        S.erase(it);
        score[t] = v;
        pair<ll,ll> New = {score[t],t};
        auto itt = S.lower_bound_pos(New);
        S.insert(itt,New);
        ll mid = (n-1)/2;
        ll point = S[mid].fi;
        pair<ll,ll> p = {point,-1LL};
        auto ittt = S.lower_bound_pos(p);
        cout << ittt << "\n";
}
    return 0;
}
//解答コードここまで

#else

// === LIB_BEGIN ===
#include <bits/stdc++.h>
#include <atcoder/all>
using namespace std;
using namespace atcoder;

const long long inf = (1LL<<60);

#define rep(i,a,b) for(ll i=(a); i<(b); i++)
#define rrep(i,a,b) for(ll i=(a); i>=(b); i--)
#define pb push_back
#define popb() pop_back()
#define fi first
#define se second
#define yn(x) do { if(x) cout << "Yes\n"; else cout << "No\n"; } while(0)
#define nall(a) (a).begin(),(a).end()
#define rall(a) (a).rbegin(),(a).rend()
#define MIN(x) *min_element(nall(x))
#define MAX(x) *max_element(nall(x))

using ll = long long;
using ld = long double;
using ull = unsigned long long;
using lll = __int128_t;

template<class T> using vc   = vector<T>;
template<class T> using vvc  = vector<vc<T>>;
template<class T> using vvvc = vector<vvc<T>>;

using Graph= vector<vector<ll>>;
using Grid = vector<vector<char>>;


//====== ここから入出力系 ======
template<class A, class B>
ostream& operator<<(ostream& os, const pair<A,B>& p) {
    return os << "(" << p.first << "," << p.second << ")";
}

//引数を空白区切りで出力
template<class T, class... Ts>
void out(const T& a, const Ts&... b){
    cout << a;
    ((cout << ' ' << b), ...);
    cout << '\n';
}
//引数を1行づつ出力
template<class T, class... Ts>
void outendl(const T& a, const Ts&... b){
    cout << a;
    ((cout << endl << b), ...); 
    cout << '\n';
}
//引数を順に読み込む
template <class... T>
void input(T &...a) {
	(cin >> ... >> a);
}

// __int128_tでも入出力できるようにする
// cout 用
ostream& operator<<(ostream& os, lll x) {
    if (x == 0) return os << '0';
    if (x < 0) { os << '-'; x = -x; } // 通常ケース

    string s;
    while (x > 0) {
        int d = (int)(x % 10);
        s.push_back(char('0' + d));
        x /= 10;
    }
    reverse(s.begin(), s.end());
    return os << s;
}

// cin 用
istream& operator>>(istream& is, lll& x) {
    string s;
    is >> s;
    bool neg = false;
    int i = 0;
    if (!s.empty() && s[0] == '-') { neg = true; i = 1; }

    lll val = 0;
    for (; i < (int)s.size(); i++) {
        val = val * 10 + (s[i] - '0');
    }
    x = neg ? -val : val;
    return is;
}

//===== ここから便利系 =====
//ans = min(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmin(T& a, const T& b) {
    if (b < a) { a = b; return true; }
    return false;
}
//ans = max(ans,x)をやる。ついでに戻り値もtrue,falseで返す。
template <class T>
bool chmax(T& a, const T& b) {
    if (a < b) { a = b; return true; }
    return false;
}

//===== ここからデバッグ系 =====
template<class Array2D>
void For2(const Array2D& a) {
    for (const auto& row : a) {
        for (const auto& v : row) cout << v << " ";
        cout << "\n";
    }
}
template <class Array1D>
void For(const Array1D& x) {
    for (const auto& a : x) cout << a << " ";
    cout << '\n';
}

// === LIB_END ===

#endif
