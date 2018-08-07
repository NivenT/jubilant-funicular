#ifndef NTA_JSON_H_INCLUDED
#define NTA_JSON_H_INCLUDED

#include "nta/MyEngine.h"
#include "nta/utils.h"

#include <map>
#include <vector>
#include <queue>
#include <iterator>

namespace nta {
	namespace utils {
		class Json;
		typedef std::map<std::string, Json> JsonObject;
		typedef std::vector<Json> JsonArray;

		// The various types a Json number can take
		enum JsonNumType {
			POS_INT,
			NEG_INT,
			FLOAT
		};
		/// \todo Make as_* functions safer and more versatile
		/// \todo add math operators
		/// A Json Number
		class JsonNum {
		private:
			/// The type of number
			JsonNumType m_type;
			union {
				/// Positive integer
				uint64_t m_pos;
				/// Negative integer
				int64_t m_neg;
				/// (Double precision) floating point number
				double m_flt;
			};
		public:
			/// By default, this is 0
			JsonNum() : m_type(POS_INT), m_pos(0) {}
			JsonNum(uint64_t n) : m_type(POS_INT), m_pos(n) {}
			JsonNum(int64_t n) : m_type(NEG_INT), m_neg(n) {}
			JsonNum(double n) : m_type(FLOAT), m_flt(n) {}
			JsonNum(int n) : m_type(n >= 0 ? POS_INT : NEG_INT) {
				if (m_type == POS_INT) {
					m_pos = n;
				} else {
					m_neg = n;
				}
			}
			JsonNumType get_type() { return m_type; }
			/// \todo Signal ErrorManager if wrong type
			uint64_t as_uint() const { return m_type == FLOAT ? m_flt : m_pos; }
			int64_t as_int() const { return m_type == FLOAT ? m_flt : m_neg; }
			double as_float() const { return m_type == FLOAT ? m_flt : m_neg; }
			double as_double() const { return m_type == FLOAT ? m_flt : m_neg; }

			operator uint64_t() const { return as_uint(); }
			operator int64_t() const { return as_int(); }
			operator double() const { return as_double(); }

			bool operator==(const JsonNum& rhs) const {
				if (m_type != rhs.m_type) return false;
				switch(m_type) {
					case POS_INT: return m_pos == rhs.m_pos;
					case NEG_INT: return m_neg == rhs.m_neg;
					case FLOAT: return m_flt == rhs.m_flt;
				}
			}

			/// Return string equivalent of stored value
			std::string to_string() const { return dump(); }
			/// Same as to_string()
			std::string dump() const {
				switch(m_type) {
					case POS_INT: return nta::utils::to_string(m_pos);
					case NEG_INT: return nta::utils::to_string(m_neg);
					case FLOAT: return nta::utils::to_string(m_flt);
				}
			}
		};

		/// The various Json types
		enum JsonValueType {
			STRING  = 0,
			NUMBER  = 1,
			OBJECT  = 2,
			ARRAY   = 3,
			BOOLEAN = 4,
			NONE    = 5 // Should be called null
		};
		/// \todo Make as_* functions safer and more versatile
		/// An arbitrary Json value
		class Json {
		public:
			// I really like Keys
			/// Key unlocking the set_{begin,end}() "private" functions of class Json::iterator
		    class SetBeginEndKey {
		        friend class Json;
		        SetBeginEndKey() {}
		        SetBeginEndKey(const SetBeginEndKey&);
		        SetBeginEndKey& operator=(const SetBeginEndKey&);
		    };
		    /// Custom iterator for looping over Json values
		    /// For Arrays: loops over the elements
		    /// For Objects: loops over the values (keys accessible via Json::iterator::key())
		    /// For anything else (except None): returns the value itself (see tets/json_tests.cpp)
		    /// For None: empty iterator
		    /// c=0 gives a regular iterator and c=1 gives a const_iterator
		    template<bool c=0>
			class iterator {
			public:
				using value_type = typename std::conditional<c, const Json, Json>::type;
				using difference_type = std::ptrdiff_t;
				using pointer = typename std::conditional<c, const Json*, Json*>::type;
				using reference = typename std::conditional<c, const Json&, Json&>::type;
				using iterator_category = std::bidirectional_iterator_tag;
			private:
				pointer m_json;
				union {
					JsonObject::iterator m_obj_iter;
					JsonArray::iterator m_arr_iter;
				};
			public:
				iterator() : m_json(nullptr) {}
				iterator(pointer json) : m_json(json) {
					assert(json != nullptr);
					switch(m_json->get_type()) {
						case OBJECT: m_obj_iter = JsonObject::iterator(); break;
						case ARRAY: m_arr_iter = JsonArray::iterator(); break;
					}
				}
				reference operator*() const {
					switch(m_json->get_type()) {
						case OBJECT: return m_obj_iter->second;
						case ARRAY: return *m_arr_iter;
						default: return *m_json;
					}
				}
				pointer operator->() const {
					switch(m_json->get_type()) {
						case OBJECT: return &m_obj_iter->second;
						case ARRAY: return &*m_arr_iter;
						default: return m_json;
					}
				}
				iterator& operator++() {
					switch(m_json->get_type()) {
						case OBJECT: ++m_obj_iter; break;
						case ARRAY: ++m_arr_iter; break;
						default: ++m_json; break;
					}
					return *this;
				}
				iterator operator++(int) {
					auto ret = *this;
					operator++();
					return ret;
				}
				iterator& operator--() {
					switch(m_json->get_type()) {
						case OBJECT: --m_obj_iter; break;
						case ARRAY: --m_arr_iter; break;
						default: --m_json; break;
					}
					return *this;
				}
				iterator operator--(int) {
					auto ret = *this;
					operator--();
					return ret;
				}
				bool operator==(const iterator& rhs) const {
					if (m_json->get_type() != rhs.m_json->get_type()) return false;
					switch(m_json->get_type()) {
						case OBJECT: return m_obj_iter == rhs.m_obj_iter;
						case ARRAY: return m_arr_iter == rhs.m_arr_iter;
						default: return m_json == rhs.m_json;
					}
				}
				bool operator!=(const iterator& rhs) const {
					return !operator==(rhs);
				}

				std::string key() {
					switch(m_json->get_type()) {
						case OBJECT: return m_obj_iter->first;
						default: return "";
					}
				}
				std::string value() {
					switch(m_json->get_type()) {
						case OBJECT: return m_obj_iter->second;
						case ARRAY: return *m_arr_iter;
						default: return *m_json;
					}
				}

				void set_begin(SetBeginEndKey _) {
					switch(m_json->get_type()) {
						case OBJECT: m_obj_iter = m_json->m_obj->begin(); break;
						case ARRAY: m_arr_iter = m_json->m_arr->begin(); break;
						case NONE: ++m_json; break;
					}
				}
				void set_end(SetBeginEndKey _) {
					switch(m_json->get_type()) {
						case OBJECT: m_obj_iter = m_json->m_obj->end(); break;
						case ARRAY: m_arr_iter = m_json->m_arr->end(); break;
						default: ++m_json; break;
					}
				}
			};
		private:
			enum JsonTokenType {
				SYMBOL, NUMTKN
			};
			/// Used for tokenizing strings before parsing them into Json objects
			struct JsonToken {
				JsonToken() : type(SYMBOL), str(nullptr) {}
				JsonToken(char c) : type(SYMBOL) {
					str = (char*)malloc(2);
					str[0] = c;
					str[1] = '\0';
				}
				JsonToken(crstring str) : type(SYMBOL) {
					this->str = strdup(str.c_str());
				}
				JsonToken(const JsonToken& other) {
					operator=(other);
				}
				~JsonToken() {
					if (type == SYMBOL && str) free(str);
				}

				JsonToken& operator=(const JsonToken& other) {
					type = other.type;
					if (type == SYMBOL) {
						str = other.str ? strdup(other.str) : nullptr;
					} else {
						num = other.num;
					}
				}
				bool operator==(const JsonToken& rhs) const {
					return type == rhs.type && ( type == SYMBOL ?
						strcmp(str, rhs.str) == 0 : num == rhs.num);
				}
				bool operator!=(const JsonToken& rhs) const {
					return !(*this == rhs);
				}

				JsonTokenType type;
				union {
					char* str;
					JsonNum num;
				};
			};

			Json(JsonValueType type) : m_type(type) {}

			/// Attempts to retreive a string token from the beginning of str
			static bool lex_string(std::string& str, JsonToken& ret);
			/// Attempts to retreive a number token from the beginning of str
			static bool lex_number(std::string& str, JsonToken& ret);
			/// Attempts to retreive a boolean token from the beginning of str
			static bool lex_bool(std::string& str, JsonToken& ret);
			/// Attempts to retreive a null token from the beginning of str
			static bool lex_null(std::string& str, JsonToken& ret);

			/// Converts curr into a stream of tokens
			static std::queue<JsonToken> tokenize(std::string curr);
			/// Converts stream of tokens into a Json value
			static Json parse_tokens(std::queue<JsonToken>& tokens);

			JsonValueType m_type;
			union {
				struct {
					char* m_str;
					// Is this ever used anywhere?
					std::size_t m_len;
				};
				JsonNum m_num;
				JsonObject* m_obj;
				JsonArray* m_arr;
				bool m_bool;
			};
		public:
			Json() : m_type(NONE) {}
			Json(crstring str) : m_type(STRING), m_str(strdup(str.c_str())), m_len(str.size()) {}
			Json(const char* str) : m_type(STRING), m_str(strdup(str)), m_len(strlen(str)) {}
			Json(JsonNum num) : m_type(NUMBER), m_num(num) {}
			Json(double num) : m_type(NUMBER), m_num(num) {}
			Json(int num) : m_type(NUMBER), m_num(num) {}
			Json(JsonObject obj) : m_type(OBJECT), m_obj(new JsonObject(obj)) {}
			Json(JsonArray arr) : m_type(ARRAY), m_arr(new JsonArray(arr)) {}
			Json(bool b) : m_type(BOOLEAN), m_bool(b) {}
			Json(const std::initializer_list<Json>& data);
			Json(const Json& other);
			Json(Json&& other);
			~Json();

			static Json string(crstring str) { 
				Json s(STRING); 
				s.m_str = strdup(str.c_str());
				s.m_len = str.size(); 
				return s; 
			}
			static Json string(const char* str) { 
				Json s(STRING); 
				s.m_str = strdup(str);
				s.m_len = strlen(str);
				return s; 
			}
			static Json number(JsonNum num) { 
				Json n(NUMBER); 
				n.m_num = num; 
				return n; 
			}
			static Json object(JsonObject obj) { 
				Json o(OBJECT); 
				o.m_obj = new JsonObject(obj); 
				return o; 
			}
			static Json array(JsonArray arr) { 
				Json a(ARRAY); 
				a.m_arr = new JsonArray(arr); 
				return a; 
			}
			static Json boolean(bool boolean) { 
				Json b(BOOLEAN); 
				b.m_bool = boolean; 
				return b; 
			}
			static Json null() { return Json(NONE); }

			/// Parses Json value from a string
			static Json parse(crstring json);

			JsonValueType get_type() const { return m_type; }
			bool is_string() const { return m_type == STRING; }
			bool is_number() const { return m_type == NUMBER; }
			bool is_object() const { return m_type == OBJECT; }
			bool is_array() const { return m_type == ARRAY; }
			bool is_bool() const { return m_type == BOOLEAN; }
			bool is_null() const { return m_type == NONE; }

			std::string as_string() const;
			JsonNum as_number() const;
			uint64_t as_uint() const { return as_number().as_uint(); }
			int64_t as_int() const { return as_number().as_int(); }
			double as_float() const { return as_number().as_float(); }
			double as_double() const { return m_num.as_double(); }
			JsonObject as_object() const { return *m_obj; }
			JsonArray as_array() const { return *m_arr; }
			bool as_bool() const;

			std::size_t size() const;
			bool is_empty() const { return size() == 0; }
			/// Returns false if m_type != ARRAY (or NONE)
			/// If m_type == NONE, converts this into an ARRAY
			bool push_back(const Json& val);
			/// Adds everything stored in other into this
			/// Returns this
			Json& merge(const Json& other);

			/// Returns a string representation of the value
			/// Note: never set second parameter
			std::string dump(std::size_t indent = 0, std::size_t offset = 0) const;

			operator std::string() const { return as_string(); }
			operator JsonNum() const { return as_number(); }
			operator uint64_t() const { return as_uint(); }
			operator int64_t() const { return as_int(); }
			operator double() const { return as_double(); }
			operator JsonObject() const { return as_object(); }
			operator JsonArray() const { return as_array(); }
			operator bool() const { return as_bool(); }

			Json& operator=(const Json& other);
			Json& operator=(Json&& other);
			bool operator==(const Json& other) const;
			bool operator!=(const Json& other) const { return !(*this == other); }
			bool operator==(const std::string& other) const { return *this == Json::string(other); }
			bool operator!=(const std::string& other) const { return !(*this == other); }
			bool operator==(const char* other) const { return *this == Json::string(other); }
			bool operator!=(const char* other) const { return !(*this == other); }
			bool operator==(const JsonNum& other) const { return *this == Json::number(other); }
			bool operator!=(const JsonNum& other) const { return !(*this == other); }
			bool operator==(const int& other) const { return *this == Json::number(other); }
			bool operator!=(const int& other) const { return !(*this == other); }
			bool operator==(const JsonObject& other) const { return *this == Json::object(other); }
			bool operator!=(const JsonObject& other) const { return !(*this == other); }
			bool operator==(const JsonArray& other) const { return *this == Json::array(other); }
			bool operator!=(const JsonArray& other) const { return !(*this == other); }
			bool operator==(const bool& other) const { return *this == Json::boolean(other); }
			bool operator!=(const bool& other) const { return !(*this == other); }

			/// When m_type == NONE, first converts this into an Object
			Json& operator[](crstring key);
			Json& operator[](crstring key) const;
			Json& operator[](const char* key) { return operator[](std::string(key)); }
			Json& operator[](const char* key) const { return operator[](std::string(key)); }
			Json& operator[](std::size_t idx);
			Json& operator[](std::size_t idx) const;
			Json& operator[](int idx) { return operator[]((std::size_t)idx); }
			Json& operator[](int idx) const { return operator[]((std::size_t)idx); }

			iterator<0> begin() {
				iterator<0> ret(this);
				ret.set_begin(SetBeginEndKey());
				return ret;
			}
			iterator<1> cbegin() const {
				iterator<1> ret(this);
				ret.set_begin(SetBeginEndKey());
				return ret;
			}
			iterator<0> end() {
				iterator<0> ret(this);
				ret.set_end(SetBeginEndKey());
				return ret;
			}
			iterator<1> cend() const {
				iterator<1> ret(this);
				ret.set_end(SetBeginEndKey());
				return ret;
			}
		};
	}
}

#endif // NTA_JSON_H_INCLUDED