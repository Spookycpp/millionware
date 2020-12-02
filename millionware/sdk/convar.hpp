#pragma once

enum e_convar_flag {
	CONVAR_FLAG_NONE = 0,
	CONVAR_FLAG_UNREGISTERED = 1 << 0,
	CONVAR_FLAG_DEVELOPMENTONLY = 1 << 1,
	CONVAR_FLAG_GAMEDLL = 1 << 2,
	CONVAR_FLAG_CLIENTDLL = 1 << 3,
	CONVAR_FLAG_HIDDEN = 1 << 4,
	CONVAR_FLAG_PROTECTED = 1 << 5,
	CONVAR_FLAG_SPONLY = 1 << 6,
	CONVAR_FLAG_ARCHIVE = 1 << 7,
	CONVAR_FLAG_NOTIFY = 1 << 8,
	CONVAR_FLAG_USERINFO = 1 << 9,
	CONVAR_FLAG_CHEAT = 1 << 14,
	CONVAR_FLAG_PRINTABLEONLY = 1 << 10,
	CONVAR_FLAG_UNLOGGED = 1 << 11,
	CONVAR_FLAG_NEVER_AS_STRING = 1 << 12,
	CONVAR_FLAG_REPLICATED = 1 << 13,
	CONVAR_FLAG_DEMO = 1 << 16,
	CONVAR_FLAG_DONTRECORD = 1 << 17,
	CONVAR_FLAG_RELOAD_MATERIALS = 1 << 20,
	CONVAR_FLAG_RELOAD_TEXTURES = 1 << 21,
	CONVAR_FLAG_NOT_CONNECTED = 1 << 22,
	CONVAR_FLAG_MATERIAL_SYSTEM_THREAD = 1 << 23,
	CONVAR_FLAG_ARCHIVE_XBOX = 1 << 24,
	CONVAR_FLAG_ACCESSIBLE_FROM_THREADS = 1 << 25,
	CONVAR_FLAG_SERVER_CAN_EXECUTE = 1 << 28,
	CONVAR_FLAG_SERVER_CANNOT_QUERY = 1 << 29,
	CONVAR_FLAG_CLIENTCMD_CAN_EXECUTE = 1 << 30,
};

class convar;
using fn_change_callback_t = void (*)(convar* var, const char* old_value, float fl_old_value);

inline int utlmemory_calc_new_allocation_count(int allocation_count, int grow_size, int new_size, int bytes_item) {
	if (grow_size)
		allocation_count = ((1 + ((new_size - 1) / grow_size)) * grow_size);
	else {
		if (!allocation_count)
			allocation_count = (31 + bytes_item) / bytes_item;

		while (allocation_count < new_size)
			allocation_count *= 2;
	}

	return allocation_count;
}

template <class t, class i = int>
class utl_memory {
public:
	t& operator[](i i) {
		return memory[i];
	}

	t* base() {
		return memory;
	}

	int num_allocated() const {
		return allocation_count;
	}

	bool is_externally_allocated() const {
		return grow_size < 0;
	}

protected:
	t* memory;
	int allocation_count;
	int grow_size;
};

template <class t>
inline t* construct(t* memory) {
	return ::new (memory) t;
}

template <class t>
inline void destruct(t* memory) {
	memory->~t();
}

template <class t, class a = utl_memory<t>>
class utl_vector {
	typedef a allocator;
	typedef t* iterator;
	typedef const t* const_iterator;
	typedef a allocator;

public:
	iterator begin() {
		return base();
	}
	const_iterator begin() const {
		return base();
	}
	iterator end() {
		return base() + count();
	}
	const_iterator end() const {
		return base() + count();
	}

	t& operator[](int i) {
		return memory[i];
	}

	t& element(int i) {
		return memory[i];
	}

	t* base() {
		return memory.base();
	}

	int& count() {
		return size;
	}

	void remove_all() {
		for (int i = size; --i >= 0;)
			destruct(&element(i));

		size = 0;
	}

	t* get_elements() {
		return elements;
	}

	void clear() {
		size = 0;
	}

	int add_to_tail() {
		return insert_before(size);
	}

	void set_size(int size) {
		size = size;
	}

	int insert_before(int elem) {
		grow_vector();
		shift_elements_right(elem);
		construct(&element(elem));

		return elem;
	}

protected:
	void grow_vector(int num = 1) {
		if (size + num > memory.num_allocated())
			memory.grow(size + num - memory.num_allocated());

		size += num;
		reset_debug_info();
	}

	void shift_elements_right(int elem, int num = 1) {
		int num_to_move = size - elem - num;
		if ((num_to_move > 0) && (num > 0))
			mem_move(&element(elem + num), &element(elem), num_to_move * sizeof(t));
	}

	allocator memory;
	int size;

	t* elements;

	inline void reset_debug_info() {
		elements = base();
	}
};

class c_convar {
	virtual void vpad_0() = 0;
	virtual void vpad_1() = 0;
	virtual void vpad_2() = 0;
	virtual void vpad_3() = 0;
	virtual void vpad_4() = 0;
	virtual void vpad_5() = 0;

public:
	virtual char* get_name() = 0;

private:
	virtual void vpad_7() = 0;
	virtual void vpad_8() = 0;
	virtual void vpad_9() = 0;
	virtual void vpad_10() = 0;
	virtual void vpad_11() = 0;

public:
	virtual float get_float() = 0;
	virtual int get_int() = 0;
	virtual void set_value_float(float value) = 0;
	virtual void set_value_int(int value) = 0;
	virtual void set_string(const char* name) = 0;

	void set_value(const char* value) {
		using original_fn = void(__thiscall*)(c_convar*, const char*);
		return (*(original_fn**)this)[14](this, value);
	}

	void set_value(float value) {
		using original_fn = void(__thiscall*)(c_convar*, float);
		return (*(original_fn**)this)[15](this, value);
	}

	void set_value(int value) {
		using original_fn = void(__thiscall*)(c_convar*, int);
		return (*(original_fn**)this)[16](this, value);
	}

	void set_value(bool value) {
		using original_fn = void(__thiscall*)(c_convar*, int);
		return (*(original_fn**)this)[16](this, static_cast<int>(value));
	}

public:
	c_convar* next;
	__int32 is_registered;
	char* name;
	char* help_string;
	__int32 flags;

private:
	char pad_0x0018[0x4];

public:
	c_convar* parent;
	char* default_value;
	char* string;
	__int32 string_length;
	float float_value;
	__int32 numerical_value;
	__int32 has_min;
	float min;
	__int32 has_max;
	float max;
	utl_vector<fn_change_callback_t> callbacks;
};

class c_convar_system {
	virtual void vpad_0() = 0;
	virtual void vpad_1() = 0;
	virtual void vpad_2() = 0;
	virtual void vpad_3() = 0;
	virtual void vpad_4() = 0;
	virtual void vpad_5() = 0;
	virtual void vpad_6() = 0;
	virtual void vpad_7() = 0;
	virtual void vpad_8() = 0;
	virtual void vpad_9() = 0;
	virtual void vpad_10() = 0;

public:
	virtual void register_con_command(c_convar* convar) = 0;
	virtual void unregister_con_command(c_convar* convar) = 0;

private:
	virtual void vpad_13() = 0;

public:
	virtual c_convar* find(const char* name) = 0;
};