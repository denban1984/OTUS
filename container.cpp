// container.cpp : Этот файл содержит функцию "main". Здесь начинается и заканчивается выполнение программы.
//

#include <iostream>
#include <map>
#include <vector>

template<typename T> struct my_allocator {

	using value_type = T;
	using pointer = T *;
	using const_pointer = const T*;
	using syze_type = std::size_t;

	my_allocator() {};

	T* allocate(size_t n) {

		auto p = std::malloc(n * sizeof(T));
		if (!p) {
			throw std::bad_alloc();
		}
		return reinterpret_cast<T*>(p);
	}

	template<typename U> struct rebind { using other = my_allocator<U>; };

	void deallocate(pointer p, [[maybe_unused]] size_t n) { std::free(p); }

	template<typename U, typename ...Args>void construct(U* p, Args&& ...args) { new(p) U(std::forward<Args>(args)...); }

	void destroy(pointer p) { p->~T(); }
};

template<typename T, int N> struct my_second_allocator {

	using value_type = T;
	using pointer = T *;
	using const_pointer = const T*;
	using syze_type = std::size_t;


	T* allocate() {

		auto p = std::malloc(N * sizeof(T));
		if (!p) {
			throw std::bad_alloc();
		}
		return reinterpret_cast<T*>(p);
	}

	template<typename U> struct rebind { using other = my_allocator<U>; };

	void deallocate(pointer p, [[maybe_unused]] size_t n) { std::free(p); }

	template<typename U, typename ...Args>void construct(U* p, Args&& ...args) { new(p) U(std::forward<Args>(args)...); }

	void destroy(pointer p) { p->~T(); }
};


template<class T, class Alloc = std::allocator<T>> class my_container {

	using Traits=std::allocator_traits<Alloc>;
	using myPtr=typename Traits::pointer;
	Alloc my_allocator;
	myPtr m_ptr = nullptr;
	size_t count_of_elements = {};

public:
	using allocator_type=Alloc;

	explicit my_container(size_t number_elem = {}, T value = {}, Alloc a = {}) :my_allocator(a) { //constructor 
		count_of_elements = number_elem;
		this->emplace(count_of_elements, value);
	}

	T& operator[](size_t i) { return *(m_ptr + i); } // access by []

	T* begin() { return m_ptr; }

	T* end() { return m_ptr + count_of_elements; }

	~my_container() { clear(); }

	template<class... Args> void emplace(size_t count, Args&& ... args) {  //allocate and construct elements

		try {
			clear();
			m_ptr = Traits::allocate(my_allocator, count);
			T* raw_ptr = static_cast<T*>(m_ptr);
			size_t i = {};

			if (count > i) {
				while (i < count) {
					Traits::construct(my_allocator, raw_ptr + i, std::forward<Args>(args)...);
					i++;
				}
			}
		}
		catch (...) {
			Traits::deallocate(my_allocator, m_ptr, count);
			throw;
		}
	}

	void clear() noexcept {
		if (m_ptr) {
			T* raw_ptr = static_cast<T*>(m_ptr);
			size_t i = {};

			if (count_of_elements > i) {
				while (i < count_of_elements) {
					Traits::destroy(my_allocator, raw_ptr + i);
					i++;
				}
			}

			Traits::deallocate(my_allocator, m_ptr, count_of_elements);
			m_ptr = nullptr;
		}
	}

};

long double fact(int N)
{
	if (N < 0) 
		return 0;
	if (N == 0)
		return 1; 
	else
		return N * fact(N - 1); 
}

int main()
{

	//std::map
	std::map<int, long double> m_map_std_alloc; //std alloc
	std::map<int, long double, std::less<int>, my_second_allocator<std::pair<const int, long double>, 10>> m_map_my_alloc;  //my alloc 

   //my_container
	int i{};

	my_container<int> m_ctr_std_alloc; //std allocator 
	my_container<int, my_allocator<int>> m_ctr_my_alloc(10);  //my_allocator

   //fill containers
	for (int i = 0; i < 10; i++) {
		m_map_my_alloc.insert(std::make_pair(i, fact(i)));
	}
	for (int i = 0; i < 10; i++) {
		m_map_std_alloc.insert(std::make_pair(i, fact(i)));
	}

	for (auto& a : m_ctr_std_alloc) {
		a = i;
		i++;
	}

	i = 0;

	for (auto& a : m_ctr_my_alloc) {
		a = i;
		i++;
	}

	// display containers

	 std::cout<<"std::map with my allocator \n";
	 for(const auto& a: m_map_my_alloc){std::cout<<a.first<<" "<<a.second<<'\n';}
	 std::cout<<'\n';
	 std::cout<<"my_container with my allocator \n";
	 for(const auto& a:m_ctr_my_alloc){std::cout<<a<<" ";}    

	return 0;
}

